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
            break;
        case 2:
            comando2(nome_do_arquivo_entrada);
            break;
        case 3:
            comando3(nome_do_arquivo_entrada);
            break;
        case 4:
            comando4(nome_do_arquivo_entrada);
            break;
        case 5:
            comando5(nome_do_arquivo_entrada);
            break;
        case 6:
            comando6(nome_do_arquivo_entrada);
            break;
    }
    return 0;
}