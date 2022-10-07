#include "abertura_e_leitura_arquivos.h"
/*

Função responsável por retornar um ponteiro de arquivo a partir de uma string que contém o nome do arquivo, o abrindo
no modo para leitura. Se a abertura não obteve sucesso, a função é quebrada e a mensagem "Falha no processamento
do arquivo" é exibida.

*/
FILE* abrir_leitura(char* nome_do_arquivo){
    FILE* arquivo;

    arquivo = fopen(nome_do_arquivo, "r");

    if(arquivo == NULL){
        printf("Falha no processamento do arquivo \n");
        return 0;
    }

    return arquivo;
}

//abre um arquivo a partir de dado nome para leitura e verifica se obteve sucesso
FILE* abrir_leitura_binario(char* nome_do_arquivo){
    FILE* arquivo;

    arquivo = fopen(nome_do_arquivo, "rb");

    if(arquivo == NULL){
        printf("Falha no processamento do arquivo \n");
        return 0;
    }

    return arquivo;
}

/*

Função responsável por retornar um ponteiro de arquivo a partir de uma string que contém o nome do arquivo, o abrindo
no modo para escrita em binário. Se a abertura não obteve sucesso, a função é quebrada e a mensagem "Falha no processamento
do arquivo" é exibida.

*/
FILE* abrir_escrita_binario(char* nome_do_arquivo){
    FILE* arquivo;

    arquivo = fopen(nome_do_arquivo, "wb");

    if(arquivo == NULL){
        printf("Falha no processamento do arquivo \n");
        return 0;
    }

    return arquivo;
}

/*

Função que recebe uma string, um ponteiro de arquivo, e o tamanho de uma string,
escreve usando frwrite e o tamanho informado no arquivo apontado

*/
void escreve_string_no_arquivo(char* str, FILE* arquivo, int tam){
    fwrite(str, sizeof(char), tam, arquivo);
}

/*

Função que escreve os dados da struct do registro de dados no arquivo, recebe um ponteiro de arquivo e um ponteiro para
registro de dados. Chama a função escreve_string_no_arquivo para campos referentes à strings, e fwrite para campos
referentes à inteiros. 

Como o registro de dados deve ocupar 64 bytes, é feito o cálculo do tamanho do registro atual, isto é, 20 (campos fixos)
+ o tamanho das strings referentes aos campos variáveis. Em seguida, o tamanho em bytes da quantidade de lixo a ser
preenchida no registro é calculada por tamanho do registro de dados - tamanho do registro atual, e o loop for é usado
para preenchimento com '$'.

*/
void escrever_no_arquivo_dados(FILE* arquivo, reg_dados* reg){

    int tam_reg_atual = 0;
    int tam_lixo = 0;

    escreve_string_no_arquivo(reg->removido, arquivo, strlen(reg->removido));
    fwrite(&reg->encadeamento, sizeof(int), 1, arquivo);
    fwrite(&reg->idConecta, sizeof(int), 1, arquivo);
    escreve_string_no_arquivo(reg->siglaPais, arquivo, strlen(reg->siglaPais));
    fwrite(&reg->idPoPsConectado, sizeof(int), 1, arquivo);
    escreve_string_no_arquivo(reg->unidadeMedida, arquivo, strlen(reg->unidadeMedida));
    fwrite(&reg->velocidade, sizeof(int), 1, arquivo);
    escreve_string_no_arquivo(reg->nomePoPs, arquivo, strlen(reg->nomePoPs));
    escreve_string_no_arquivo(reg->nomePais, arquivo, strlen(reg->nomePais));

    tam_reg_atual = 20 + strlen(reg->nomePoPs) + strlen(reg->nomePais);
    tam_lixo = (TAM_REG_DADOS - tam_reg_atual);

    for(int i = 0; i < tam_lixo; i++){
        fwrite("$", sizeof(char), 1, arquivo);
    }
}

/*

Função que escreve os dados da struct do registro de cabeçalho no arquivo, recebe um ponteiro de arquivo e um ponteiro para
registro de cabeçalho. Chama a função escreve_string_no_arquivo para campos referentes à strings, e fwrite para campos
referentes à inteiros. 

Como o registro de cabeçalho deve ocupar o tamanho de uma página de disco (960 bytes), é feito o cálculo do valor do tamanho
da página de disco - o tamanho do registro de cabeçalho (que é fixo), para utilizar-se como condição de parada num loop
que escreve '$' até completar o tamanho necessário.

*/
void escrever_no_arquivo_cabecalho(FILE* arquivo, reg_cabecalho* reg){

    escreve_string_no_arquivo(reg->status, arquivo, strlen(reg->status));
    fwrite(&reg->topo, sizeof(int), 1, arquivo);
    fwrite(&reg->proxRRN, sizeof(int), 1, arquivo);
    fwrite(&reg->nroRegRem, sizeof(int), 1, arquivo);
    fwrite(&reg->nroRegRem, sizeof(int), 1, arquivo);
    fwrite(&reg->qttCompacta, sizeof(int), 1, arquivo);
    
    for(int i = 0; i < (TAM_PAG_DISCO - TAM_REG_CABECALHO); i++){
        fwrite("$", sizeof(char), 1, arquivo);
    }
}