#include "topologiaRede.h"
#include "funcionalidades.h"

int main(){

    char nome_do_arquivo_entrada[TAM_NOME_ARQUIVO];
    char nome_do_arquivo_saida[TAM_NOME_ARQUIVO];
    int comando = 0;

    scanf("%d", &comando);
    scanf("%s", nome_do_arquivo_entrada);

    switch(comando){
        case 1:
            scanf("%s", nome_do_arquivo_saida);
            comando1(nome_do_arquivo_entrada, nome_do_arquivo_saida);
        case 2:
            comando2(nome_do_arquivo_entrada);
    }

    return 0;
}

