#ifndef __ESCRITA_H__
#define __ESCRITA_H__

#include "topologiaRede.h"

//escreve no arquivo
void escreve_string_no_arquivo(char*, FILE*, int);
void escrever_no_arquivo_dados(FILE*, reg_dados*);
void escrever_no_arquivo_cabecalho(FILE*, reg_cabecalho*);
void atualizar_reg_cabecalho(reg_cabecalho*, FILE*, int*);

//escreve na tela
void printa_registro(reg_dados*);
#endif