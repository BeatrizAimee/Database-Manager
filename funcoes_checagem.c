#include "funcoes_checagem.h"

/*

Checa a consistência de um arquivo, isto é, se a posição [0] do 
campo "status" do registro de cabeçalho é diferente de '1', 
isto é, o arquivo está inconsistente, é printado uma mensagem 
de erro e retornado 1. Caso contrário é retornado 0.

*/
int checa_consistencia(reg_cabecalho* reg){
    if(reg->status[0] != '1'){//se for diferente de 1, ou seja, inconsistente
        printf("Falha no processamento do arquivo.\n");
        return 1;//retorna 1
    }
    return 0;//caso contrário, retorna 0
}

/*

Checa se um registro de dados é marcado como removido, isto é, 
se a posição [0] do campo "removido" for diferente de '1', 
ou seja, não removido, é printado uma mensagem de falha, e é 
retornado 1. Caso contrário, é retornado 0.

*/
int checa_remocao(reg_dados* reg){
    if(reg->removido[0] != '1'){
        printf("Falha no processamento do arquivo.\n");
        return 1;
    }
    return 0;
}