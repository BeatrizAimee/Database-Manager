#include "funcoes_auxiliares.h"
#include "funcoes_fornecidas.h"
#include "funcoes_de_leitura.h"
#include "funcoes_de_escrita.h"

char* zstrtok(char *str, const char *delim) {
    static char *s_str = NULL;   /* var to store last address */
    char *p;

    if (delim == NULL || (str == NULL && s_str == NULL)){
        return NULL;
    }

    if (str == NULL){
        str = s_str;
    }

    /* if delim is not contained in str, return str */
    if ((p = strstr(str,delim)) == NULL) {
        s_str = NULL;
        return str;
    }

    /* 
    * check for consecutive delimiters
    * if first char is delim, return delim
    */
    if (str[0] == delim[0]) {
        s_str++;
        return (char *)delim;
    }

    /* terminate the string */
    *p = '\0';

    /* save the rest of the string */
    if ((p+1) != NULL) {
        s_str = (p + 1);
    } else {
        s_str = NULL;
    }

        return str;
}

/*

Função responsável por receber um ponteiro para um registro de dados, um contador da posição do campo lido,
e um token que representa uma string do arquivo csv separado por vírgula

É feito um switch case para verificar o valor que indica a posição do campo, dado que os campos estão sempre na
mesma ordem. Portanto para cada valor da posição, de 0 a 6, é feita a operação desejada:

Se 0: representa o idConecta, nunca nulo, portanto a função (atoi) converte para inteiro a string (token)
e coloca na variável tmp, em seguida o valor é colocado na struct

Se 1 ou 2: refere-se aos campos de tamanho varíavel, portanto, verifica-se primeiro se o primeiro caracter é ",", caso seja
o campo tem valor nulo e é copiado um '|' e um '\0' para a string. Caso contrário, é copiada a string a partir de (token),
e na posição seguinte é colocado o indicador de fim de campo '|', e na à frente um '\0' para controle interno do programa.

Para todos outros casos, é feita a mesma verificação do primeiro caracter de (token), e seguindo a mesma lógica, caso seja
',', é copiado no campo o indicador de valor nulo para inteiros (-1) ou '$' indicador de lixo para campos de tamanho fixo,
mais um '\0' para controle interno do programa. Caso contrário é copiada a string para a struct, e no caso de campos
referentes à inteiros, é utilizada a função (atoi) para converter (token) para um inteiro, e assim o valor é colocado
na struct.

Apenas para o caso 6, referente ao campo velocidade, a verificação é diferente, pois neste caso, em situações que o campo é
nulo, não há dois caracteres ',' adjacentes, portanto a verificação é feita utilizando a função (atoi), em que se for
retornado 0, isto é, se (token) não é conversível para um inteiro, então é nulo, e portanto coloca-se -1. Caso contrário,
o valor obtido é convertido e armazenado na struct.

*/
void gravar_dados(reg_dados* reg, int pos_campo, char* token){
    int tmp;

    switch(pos_campo){
        case 0:
            tmp = atoi(token);
            reg->idConecta = tmp;
            break;
        case 1:
            if(token[0] != ','){
                strcpy(reg->nomePoPs, token);
                strcat(reg->nomePoPs, "|");
            }
            else{
                strcpy(reg->nomePoPs, "|");
            }
            break;
        case 2:
            if(token[0] != ','){
                strcpy(reg->nomePais, token);
                strcat(reg->nomePais, "|");
            }
            else{
                strcpy(reg->nomePais, "|");
            }
            break;
        case 3:
            if(token[0] != ','){
                strcpy(reg->siglaPais, token);
            }
            else{
                strcpy(reg->siglaPais, "$$");
            }
            break;
        case 4:
            if(token[0] != ','){
                tmp = atoi(token);
                reg->idPoPsConectado = tmp;
            }
            else{
                reg->idPoPsConectado = -1;
            }
            break;
        case 5:
            if(token[0] != ','){
                strcpy(reg->unidadeMedida, token);
            }
            else{
                strcpy(reg->unidadeMedida, "$");
            }
            break;
        case 6:
            tmp = atoi(token);
            if(tmp == 0){
                reg->velocidade = -1;
            }
            else{
                reg->velocidade = tmp;
            }
            break;
    }
}


/*

Função responsável por calcular a quantidade de páginas de disco ocupadas pelos registros de dados e registro de cabeçalho,
declara um inteiro modulo e numero de paginas de disco, modulo é o resto do cálculo da quantidade de páginas de disco a partir
do número de registros obtido durante a leitura do arquivo de entrada. Em seguida, nro de paginas de disco é a divisão inteira
da quantidade de paginas de disco.

É verificado se modulo é diferente de 0, se é diferente de 0, então o nro de paginas é aumentado em 1, dado que o número
não inteiro, caso contrário, o número de páginas permanece inalterado. Após a verificação, o número de páginas de disco é
aumentado em 1 para levar em conta o registro de cabeçalho

Nro de páginas é retornado como inteiro

*/
int calcula_pag_disco(int cont_registros){
    int modulo = 0;
    int nroPagDisco = 0;

    modulo = (TAM_REG_DADOS*cont_registros)%TAM_PAG_DISCO;
    nroPagDisco = (int)(TAM_REG_DADOS*cont_registros)/TAM_PAG_DISCO;

    if(modulo != 0){
        nroPagDisco++;
    }
    nroPagDisco++;

    return nroPagDisco;
}

/*

Função responsável por conferir se o registro é removido ou não, e caso negativo printar o registro. Recebe o registro de dados e o arquivo de entrada. Declara-se o inteiro num_RRN para mandar como argumento para a função le_arquivo, e o inteiro status que recebe a flag retornada pela função le_arquivo() 

Se o status recebeu 1, significa que não é o fim do arquivo. Então, é conferido se o registro foi removido, e caso negativo chama-se a função printa_registro() para printar o registro. O retorno, então, é o repasse da flag do status, 1. 

Se o status recebeu 0, significa que o arquivo chegou ao fim, e a flag recebida pelo status é retornada. 

*/
int confere_remocao(reg_dados* reg, FILE* arquivo_entrada){

    int num_RRN = 0;
    int status = le_arquivo(reg, arquivo_entrada, &num_RRN);
    if(status==1){//nao é o fim do arquivo

        if (reg->removido[0] == '0'){ //registro não removido
            printa_registro(reg);
        }
        return status;
    }
    else{
        return status;
    }
}

/*

Função responsável por compactar o arquivo. Recebe o registro de dados, o arquivo de entrada, o arquivo de saida e o contador de registros. Declara-se o inteiro num_RRN para mandar como argumento para a função le_arquivo, e o inteiro status que recebe a flag retornada pela função le_arquivo() 

Se o status recebeu 1, significa que não é o fim do arquivo. Então, é conferido se o registro foi removido, e caso negativo o contador de registro aumenta em um e o registro lido é escrito no arquivo de saida com a função escrever_no_arquivo_dados(). O retorno, então, é o repasse da flag do status, 1. 

Se o status recebeu 0, significa que o arquivo chegou ao fim, e a flag recebida pelo status é retornada. 

*/
int compacta_arquivo( reg_dados* reg, FILE* arquivo_entrada, FILE* arquivo_saida, int*contador_reg){
    //se o arquivo não está no final, le registro
    int num_RRN = 0;
    int status = le_arquivo(reg, arquivo_entrada, &num_RRN);
    
    if (status == 1){
        if (reg->removido[0]=='0'){
            (*contador_reg)++;
            //registro não removido, escreve registro;
            escrever_no_arquivo_dados(arquivo_saida, reg);
        }
        return status;
    }
    else{
        return status;
    }
}

/*
Função responsável por remover logicamente um registro. Recebe o registro de dados, o arquivo de entrada, o registro de saida e o RRN do registro. 

Primeiro muda-se na RAM o removido do registro de dados de '0' para '1'. Então, o numero no encadeamento, no registro de dados, passa a ser o numero do topo, que esta no registro de cabecalho. Já o topo passa a receber o valor do RRN do registro removido, criando uma pilha. 

Então, é declarada a variavel inteira do byteoffset, que é em seguida calculada, variando com o valor do RRN. É feito um fseek para o byteoffset do RRN desejado, atualizado no arquivo o "removido" com a função escreve_string_no_arquivo() e o encadeamento com um fwrite(). O resto do espaço ate o final do registro é por fim preenchido com lixo, em um loop for() que faz o fwrite() do caracter $.
*/
void apaga_registro(FILE*arquivo_entrada, reg_dados* reg_dados, reg_cabecalho*reg_cabecalho, int* RRN ){
  
    reg_dados->removido[0] = '1';
    reg_dados->encadeamento = reg_cabecalho->topo;
    reg_cabecalho->topo = *RRN;
   
    int byteoffset = 0;
    
    byteoffset = TAM_PAG_DISCO + (TAM_REG_DADOS)*(*RRN);

    fseek(arquivo_entrada, byteoffset, SEEK_SET);

    escreve_string_no_arquivo(reg_dados->removido, arquivo_entrada, strlen(reg_dados->removido));
    fwrite(&reg_dados->encadeamento, sizeof(int), 1, arquivo_entrada);

    for(int i = 0; i < (TAM_REG_DADOS - 5); i++){
        fwrite("$", sizeof(char), 1, arquivo_entrada);
    }
}

/*

Função responsável por remover o arquivo. Recebe o nome do arquivo temporario e o nome do arquivo de entrada. São inializadas as variaveis inteiras renomear e remover como 1. 

Primeiro a variavel remover recebe o retorno da função remove(), que remove o arquivo enviado, o arquivo de entrada, e retorna 0 se a operação foi realizada com sucesso. Em seguida, a variavel renomear recebe o retorno da função rename(), que renomeia o arquivo enviado no primeiro argumento (arquivo de saida) para o nome do segundo argumento (arquivo original de entrada) e retorna 0 se a operação for realizada com sucesso

Se ambas operações tem sucesso, ou seja, renomear e remover são iguais a 0, é printado o binario do arquivo compactado com a função binarioNaTela. Caso contrário é printado "Falha no processamento do arquivo". 

*/
void remover_arquivo(char* nome_temp, char* nome_do_arquivo_entrada){
    int renomear = 1;
    int remover = 1;

    remover = remove(nome_do_arquivo_entrada);
    renomear = rename(nome_temp, nome_do_arquivo_entrada);

    if (renomear==0 && remover==0){
        binarioNaTela(nome_do_arquivo_entrada);
    }
    else{
        printf("Falha no processamento do arquivo.\n");
    }
}