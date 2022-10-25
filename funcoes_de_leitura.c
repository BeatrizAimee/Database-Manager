#include "funcoes_de_leitura.h"
#include "funcoes_auxiliares.h"
#include "funcoes_fornecidas.h"
/*

Função responsável por ler o registro de cabeçalho de um arquivo binário e colocar numa struct. Recebe um ponteiro para
arquivo e um ponteiro para uma struct do tipo (reg_cabecalho). Os campos são lidos em sequência conforme o número de bytes
ocupados. É colocado na segunda posição de "status" do registro um '\0' para tratamento interno do programa como string.
Ao fim, é dado um (fseek) para o fim do registro de cabeçalho, dado que o tamanho é sempre fixo em uma página de disco,
utilizando o cálculo do tamanho da página de disco - o tamanho do registro de cabeçalho.

*/
void ler_reg_cabecalho(FILE* arquivo, reg_cabecalho* reg){
    fread(reg->status, sizeof(char), 1, arquivo);
    reg->status[1] = '\0';
    fread(&reg->topo, sizeof(int), 1, arquivo);
    fread(&reg->proxRRN, sizeof(int), 1, arquivo);//lê o próximo RRN disponível
    fread(&reg->nroRegRem, sizeof(int), 1, arquivo);//lê o número de registros removidos
    fread(&reg->nroPagDisco, sizeof(int), 1, arquivo);//lê o número de páginas de disco
    fread(&reg->qttCompacta, sizeof(int), 1, arquivo);//lê o qttCompacta
    fseek(arquivo, (TAM_PAG_DISCO - TAM_REG_CABECALHO), SEEK_CUR);//pula o lixo
}

/*

Função responsável por ler todos os campos do registro. Recebe um ponteiro para 
arquivo e um ponteiro para uma struct do tipo (reg_dados). 
Então, sao lidos os campos de tamanho fixo com a função le_campos_tam_fixo() e 
os campos de tamanho variavel com a função le_campos_tam_variavel()

*/
void le_registro(reg_dados* reg, FILE* arquivo_entrada){
  le_campos_tam_fixo(reg,arquivo_entrada);
  le_campos_tam_variavel(reg,arquivo_entrada);
}


/*

Função responsável por ler os campos de tamanho fixo de um registro de dados. Recebe um ponteiro para
arquivo e um ponteiro para uma struct do tipo (reg_dados). 
Os campos são lidos em sequência conforme o número de bytes ocupados, com a função fread().

*/
void le_campos_tam_fixo(reg_dados* reg, FILE* arquivo_entrada){

  // le campos fixos
    fread(&reg->encadeamento, sizeof(int), 1, arquivo_entrada);
    fread(&reg->idConecta,sizeof(int), 1, arquivo_entrada);
    fread(reg->siglaPais,sizeof(char)*2, 1, arquivo_entrada);
    reg->siglaPais[2]='\0';
    fread(&reg->idPoPsConectado,sizeof(int), 1, arquivo_entrada);
    fread(reg->unidadeMedida,sizeof(char), 1, arquivo_entrada);
    reg->unidadeMedida[1]='\0';
    fread(&reg->velocidade,sizeof(int), 1, arquivo_entrada);

}

/*

Função responsável por ler os campos de tamanho variavel de um registro de dados. 
Recebe um ponteiro para arquivo e um ponteiro para uma struct do tipo (reg_dados). 
Inicialmente é inicializado uma variavel do tipo char, com nome buffer, como '0'. 
Tambem são declarados os inteiros pos_nomePoPs e pos_nomePais, que indicam a posiçao 
nas respectivas strings, inicializados como 0, e o inteiro tam_lixo, que indica o tamanho do lixo. 

Entao, é feito um loop while() para cada um dos campos variaveis. Enquanto o buffer, 
que guarda o char lido não for | (que mostra que o campo terminou) nem $ 
(no caso em que o registro for removido e só houver lixo) é feita a sequencia:  é 
lido um byte, ao campo desejado, na posição do contaodor pos, é adicionado o char 
lido (buffer), e a pos é atualizada (aumenta-se 1). Antes de ler um novo campo, a 
variavel buffer é resetada para '0'. 

Apos ler os campos, o |, na RAM, é removido e substituido por um /0, indicador que a 
string chegou ao fim. Então, o tamanho do lixo é calculado com base no tamanho dos contadores 
de posição, tamanho do registro e tamanho dos campos de tamanho fixo. Por fim, se o lixo existe, 
ou seja, seu tamanho é diferente de 0, é declarada uma string buffer de lixo e o lixo é lido com um fread(). 

*/
void le_campos_tam_variavel(reg_dados* reg, FILE* arquivo_entrada){
char buffer = '0'; // armazena character

    int pos_nomePoPs=0; // posição na string de nomePops
    int pos_nomePais=0; // posição na string de nomePais
    int tam_lixo; // tamanho do lixo 

                
    
    //le campos variaveis
    while (buffer !='|' && buffer != '$') // 
    {
        fread(&buffer, sizeof(char), 1, arquivo_entrada); // le o char

        reg->nomePoPs[pos_nomePoPs]=buffer; //constroi a string
        pos_nomePoPs+=1; //atualiza a posição
                
    }
        
    buffer = '0'; //reseta o buffer

        while (buffer !='|' && buffer != '$')
    {
        //fscanf(arquivo_entrada,"%c",buffer);
        fread(&buffer, sizeof(char), 1, arquivo_entrada); // le o char
                
        reg->nomePais[pos_nomePais]=buffer; //constroi a string
        pos_nomePais+=1; //atualiza a posição
                

    }
             
    reg->nomePoPs[pos_nomePoPs-1]='\0'; // substitui o | por um \0
    reg->nomePais[pos_nomePais-1]='\0';

           
    tam_lixo=TAM_REG_DADOS-pos_nomePoPs-pos_nomePais-20; //descobre o tamanho do lixo 
         
    if (tam_lixo!=0){  //confere se existe lixo

    char lixo[tam_lixo];  //cria a string lixo
    fread(lixo,sizeof(char)*tam_lixo, 1, arquivo_entrada); //le o lixo

    }
}

/*

Função responsável por ler o arquivo completo. Recebe um ponteiro para arquivo e um ponteiro para 
uma struct do tipo (reg_dados), alem de um ponteiro de inteiro num_RRN, que atualiza o RRN lido. 

Se o fread() feito para o primeiro campo do registro nao retornar 0 significa que o arquivo não 
chegou ao fim. Logo, sera adicionado um \0 na RAM na segunda posição da string removido, marcando seu fim, 
e serão lidos todos os campos do registro com a função le_todos_campos_do_registro(). 
Então, o numero do RRN aumenta, já  que será lido um novo registro, e é retornado 1.

Se o fread() para o primeiro campo retornar 0 significa que o arquivo chegou ao fim, e 
a flag é passada a frente por meio de um retorno 0. 

*/
int le_arquivo(reg_dados* reg, FILE* arquivo_entrada, int*num_rrn){
    if(fread(reg->removido,sizeof(char), 1, arquivo_entrada) != 0){//se não chegou ao fim
        reg->removido[1] = '\0';
        le_registro(reg,arquivo_entrada);
        (*num_rrn)++;
        return 1;
    }
    return 0;
}

/*

Função responsável por ler os dados dos registros do arquivo csv. Recebe um buffer e um registro de dados, uma string (token)
é criada, e um marcador da posição do campo lido do registro é inicializada como zero. Em seguida, utilizando a função
(zstrtok), a string (token) recebe do buffer (linha do arquivo csv) uma string criada do início até o primeiro separador 
','.

Enquanto token não retornar (NULL), os dados são gravados numa struct com a função (gravar_dados), a posição do campo é 
aumentada a cada leitura de modo a demarcar qual campo da struct será preenchido, e a string (token) é atualizada recebendo
a partir de (NULL)(indicador da função zstrktok de continuar a partir da última posição válida) a próxima string do registro.

*/
void ler_dados(char* buffer, reg_dados* novo_reg_dados){   
    char* token;
    int pos_campo = 0;
    token = separador("separa_proximo");

    if(token[strlen(token)-1] == ' '){
        token[strlen(token)-1] = '\0';
    }

    while(token){//enquanto não for NULL
        gravar_dados(novo_reg_dados, pos_campo, token);//preenche na struct
        pos_campo++;//aumenta contador de campos 
        token = separador("separa_proximo");//pega próximo campo
        if(token == NULL){
          break;
        }
        if(token[strlen(token)-1] == ' '){
            token[strlen(token)-1] = '\0';
        }
    }
}

/*

Função responsável por ler registros como entradas do teclado. Recebe um ponteiro de struct do tipo (reg_dados). É declarado
um buffer, e um inteiro temporário, os campos são lidos em sequência. Primeiro idConecta, seguido pela utilização da função
(scan_quote_string) para os campos string que estão entre "". A string sem "" é colocado no buffer, e é verificado
utilizando (strlen) se a string é vazia ou não. Se for vazia, isto indica que o campo é NULO, e no registro em memória
principal ele é tratado conforme as especificações para campos nulos, fixos ou variáveis, isto é, é colocado '|' para campos
variáveis, e '$' para os fixos. Para os campos sem "", isto é, os inteiros, utiliza-se (scanf) para string, e é colocado no
buffer. Em seguida, é feita a conversão para inteiro utilizando (atoi) e a variável tmp, e este valor é então colocado no
registro. Caso (atoi) retorne 0, o campo é nulo, e -1 é colocado no registro.

*/
void ler_registros_dados_teclado(reg_dados* reg){

    char buffer[24];
    int tmp = 0;


    scanf("%d", &reg->idConecta);//idConecta (nunca nulo)

    scan_quote_string(buffer);//nomePoPs
    if(strlen(buffer) == 0){
        strcpy(reg->nomePoPs, "|");
    }
    else{
        strcat(buffer, "|");
        strcpy(reg->nomePoPs, buffer);
    }

    scan_quote_string(buffer);//nomePais
    if(strlen(buffer) == 0){
        strcpy(reg->nomePais, "|");
    }
    else{
        strcat(buffer, "|");
        strcpy(reg->nomePais, buffer);
    }

    scan_quote_string(buffer);//siglaPais
    if(strlen(buffer) == 0){
        strcpy(reg->siglaPais, "$$");
    }
    else{
        strcpy(reg->siglaPais, buffer);
    }

    scanf("%s", buffer);//idPoPsConectado
    tmp = atoi(buffer);
    if(tmp == 0){
        reg->idPoPsConectado = -1;
    }
    else{
        reg->idPoPsConectado = tmp;
    }
 
    scan_quote_string(buffer);//unidadeMedida
    if(strlen(buffer) == 0){
        strcpy(reg->unidadeMedida, "$");
    }
    else{
        strcpy(reg->unidadeMedida, buffer);
    }

 
    scanf("%s", buffer);//pega velocidade
    tmp = atoi(buffer);
    if(tmp == 0){
        reg->velocidade = -1;
    }
    else{
        reg->velocidade = tmp;
    }
}

/*

Função responsável por atribuir a um buffer uma string do teclado utilizando (scanf). É feita uma comparação utilizando (strcmp) para
verificar qual o campo que será inserido. A partir desta verificação, é retornado um inteiro, começando a partir de 0 e indo até 6, referente à posição do campo dentro do registro.

*/
int ler_campo(){
    char buffer[24];
    scanf("%s", buffer);//pega o nome do campo

    if(strcmp(buffer, "idConecta") == 0) return 0;
    else if(strcmp(buffer, "siglaPais") == 0) return 1;
    else if(strcmp(buffer, "idPoPsConectado") == 0) return 2;
    else if(strcmp(buffer, "unidadeMedida") == 0) return 3;
    else if(strcmp(buffer, "velocidade") == 0) return 4;
    else if(strcmp(buffer, "nomePoPs") == 0) return 5;
    else if(strcmp(buffer, "nomePais") == 0) return 6;
    else return -1;
}