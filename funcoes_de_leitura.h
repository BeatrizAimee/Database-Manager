#ifndef __LEITURA_H__
#define __LEITURA_H__

#include "topologiaRede.h"

//leitura do registro
void le_registro(reg_dados*, FILE*);
void le_campos_tam_fixo(reg_dados*, FILE*);
void le_campos_tam_variavel(reg_dados*, FILE*);

int le_arquivo(reg_dados*, FILE*,int*);
void ler_reg_cabecalho(FILE*, reg_cabecalho*);

//leituras auxiliares
void ler_dados(char*, reg_dados*);
void ler_registros_dados_teclado(reg_dados*);
int ler_campo();

#endif