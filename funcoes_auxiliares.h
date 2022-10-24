#ifndef __AUXILIARES_H__
#define __AUXILIARES_H__

#include "topologiaRede.h"

char* zstrtok(char*, const char*);
void gravar_dados(reg_dados*, int, char*);
int compacta_arquivo( reg_dados*, FILE*, FILE*, int*);
void apaga_registro(FILE*, reg_dados* , reg_cabecalho*, int*);
void remover_arquivo(char*, char*);
int confere_remocao(reg_dados*, FILE*);
int calcula_pag_disco(int);


#endif