#include "funcoes_auxiliares.h"

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
    token = zstrtok(buffer, ",");

    while(token){//enquanto não for NULL
        gravar_dados(novo_reg_dados, pos_campo, token);//preenche na struct
        pos_campo++;//aumenta contador de campos 
        token = zstrtok(NULL, ",");//pega próximo campo
        if(token == NULL){
          break;
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

    char* buffer[24];
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