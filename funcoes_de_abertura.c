#include "funcoes_de_abertura.h"

/*

Função responsável por retornar um ponteiro de arquivo a partir de uma string que contém o nome do arquivo, o abrindo
no modo para leitura. Se a abertura não obteve sucesso, a função é quebrada e a mensagem "Falha no processamento
do arquivo" é exibida.

*/
FILE* abrir_leitura(char* nome_do_arquivo){
    FILE* arquivo;
    arquivo = fopen(nome_do_arquivo, "r");

    if(arquivo == NULL){
        printf("Falha no processamento do arquivo.\n");
        return 0;
    }
    return arquivo;
}

/*

Função responsável por retornar um ponteiro de arquivo a partir de uma string que contém o nome do arquivo, o abrindo
no modo para leitura em binário. Se a abertura não obteve sucesso, a função é quebrada e a mensagem "Falha no processamento
do arquivo" é exibida.

*/
FILE* abrir_leitura_binario(char* nome_do_arquivo){
    FILE* arquivo;
    arquivo = fopen(nome_do_arquivo, "rb");

    if(arquivo == NULL){
        printf("Falha no processamento do arquivo.\n");
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
        printf("Falha no processamento do arquivo.\n");
        return 0;
    }
    return arquivo;
}

/*

Função responsável por retornar um ponteiro de arquivo a partir de uma string que contém o nome do arquivo, o abrindo
no modo para leitura e escrita em binário. Se a abertura não obteve sucesso, a função é quebrada e a mensagem "Falha no processamento
do arquivo" é exibida.

*/
FILE* abrir_leitura_e_escrita_binario(char* nome_do_arquivo){
    FILE* arquivo;
    arquivo = fopen(nome_do_arquivo, "rb+");

    if(arquivo == NULL){
        printf("Falha no processamento do arquivo.\n");
        return 0;
    }
    return arquivo;
}