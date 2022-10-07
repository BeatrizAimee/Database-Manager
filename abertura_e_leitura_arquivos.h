#ifndef __ABERTURA_H__
#define __ABERTURA_H__

#include "topologiaRede.h"

FILE* abrir_leitura(char*);
FILE* abrir_leitura_binario(char*);
FILE* abrir_escrita_binario(char*);
void escreve_string_no_arquivo(char*, FILE*, int);
void escrever_no_arquivo_dados(FILE*, reg_dados*);
void escrever_no_arquivo_cabecalho(FILE*, reg_cabecalho*);

#endif