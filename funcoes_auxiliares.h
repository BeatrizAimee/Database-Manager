#ifndef __AUXILIARES_H__
#define __AUXILIARES_H__

#include "topologiaRede.h"

char* zstrtok(char*, const char*);
void gravar_dados(reg_dados*, int, char*);
int calcula_pag_disco(int);
void ler_dados(char*, reg_dados*);
void ler_registros_dados_teclado(reg_dados*);

#endif