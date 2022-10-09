#include "abertura_e_leitura_arquivos.h"
#include "funcoes_auxiliares.h"

/*

Função responsável por realizar a funcionalidade 1, em que são criados dois ponteiros, um para registro da dados e outro
para registro de cabeçalho, em seguida é criado um buffer temporário para armazenar as linhas do arquivo csv. Dois ponteiros
um para um arquivo de entrada e outro para saída são criados, e dois contadores, um para a quantidade de registros lidos e outro
para o número de páginas de disco são criados. Em seguida, é escrito no arquivo de saída o cabeçalho inicial usando a função
(escrever_no_arquivo_cabecalho), e primeira linha do arquivo csv é "pulada" utilizando 2 (fgets).

Em seguida, é feito um loop (while), enquanto (fgets) não retornar nada (o que significa que não há mais linhas para ler
no arquivo csv), o contador de registros aumenta, e os dados são lidos do arquivo e gravados na struct pela função (ler_dados),
e em seguida escrito no arquivo utilizando a função (escrever_no_arquivo_dados).

Em seguida, o nro de páginas de disco é calculado pela função (calcula_pag_disco), e o registro de cabeçalho é atualizado
no campo "status", indicando que o arquivo não está corrompido, o campo "proxRRN" recebe o contador de registros, indicando
o RRN para um próximo registro a ser gravado, e o nro de páginas de disco ocupadas é atualizado. Um (fseek) retorna para
o início do arquivo, e o registro de cabeçalho é sobrescrito utilizando a função (escrever_no_arquivo_cabecalho).

Por fim, o arquivo de entrada e saída são fechados com (fclose).

*/
void comando1(char* nome_do_arquivo_entrada, char* nome_do_arquivo_saida){
    reg_dados* novo_reg_dados = cria_registro_dados();
    reg_cabecalho* novo_reg_cabecalho = cria_registro_cabecalho();

    char buffer[TAM_REG_DADOS];

    FILE* arquivo_entrada = abrir_leitura(nome_do_arquivo_entrada);
    if(arquivo_entrada == NULL){
        return 0;
    }
    FILE* arquivo_saida = abrir_escrita_binario(nome_do_arquivo_saida);
    if(arquivo_saida == NULL){
        return 0;
    }

    int cont_registros = 0;
    int nroPagDisco = 0;

    escrever_no_arquivo_cabecalho(arquivo_saida, novo_reg_cabecalho);//incializa cabeçalho
    fgets(buffer, 64, arquivo_entrada);//pula primeira linha
    fgets(buffer, 64, arquivo_entrada);//pula primeira linha

    while(fgets(buffer, 64, arquivo_entrada)){

        cont_registros++;//conta registros
        ler_dados(buffer, novo_reg_dados);
        escrever_no_arquivo_dados(arquivo_saida, novo_reg_dados);
    }

    nroPagDisco = calcula_pag_disco(cont_registros);//calcula o número de páginas de disco

    strcpy(novo_reg_cabecalho->status, "1");//altera valores do registro de cabeçalho conforme necessidade
    novo_reg_cabecalho->proxRRN = cont_registros;
    novo_reg_cabecalho->nroPagDisco = nroPagDisco;

    fseek(arquivo_saida, 0, SEEK_SET);//volta para inicio do arquivo

    escrever_no_arquivo_cabecalho(arquivo_saida, novo_reg_cabecalho);//atualiza cabeçalho

    fclose(arquivo_entrada);
    fclose(arquivo_saida);
}

/*

Função responsável por realizar a funcionalidade 5. Recebe uma string com o nome do arquivo de entrada que será processado.
o arquivo de entrada é aberto, e são declarados um inteiro referente ao byte_offset, um inteiro referente ao número
de registros que serão adicionados, e um inteiro referente ao número total de registros do arquivo. Um novo registro de 
dados e de cabeçalho é criado. Em seguida, a função (ler_reg_cabecalho) é chamada, para ler o registro de cabeçalho do 
arquivo de entrada, e é checado se o status do arquivo é '0' ou '1'.  Se for '0', isto é, inconsistente, uma mensagem de
erro é exibida e a função retorna 0. Após isto, o número de registros adicionados é pego do teclado. 

Em seguida, um loop (for) é criado, que roda o número de vezes que um registro será adicionado, inicializando-se sempre com
a chamada da função (ler_registros_dados_teclado), que adquire os valores dos campos dos registros de entrada. É verificado
se o topo do registro de cabeçalho do arquivo é '-1' ou não. Se sim, isto significa que não há registros removidos no arquivo,
e é então calculado o byte_offset, somando-se o valor ocupado pelo registro de cabeçalho ao próximo RRN disponível vezes o
tamanho do registro de dados. É dado um (fseek) para esta posição no arquivo, e a função (escrever_no_arquivo_dados) é chamada,
para escrever no arquivo o novo registro. Em seguida, o campo (proxRRN) é atualizado para o próximo RRN, e é calculado o número
total de registros do arquivo, a partir da posição atual utilizando-se (ftell) menos o tamanho de uma página de disco para levar
em conta o registro de cabeçalho, e tudo isto dividido pelo tamanho de um registro de dados.

Caso o topo != -1, há registros removidos no arquivo. Portanto, calcula-se o byte_offset da mesma maneira que no primeiro caso,
porém com a diferença de que ao invés de utilizar-se o proxRRN, utiliza-se o topo da pilha como indicador do RRN do registro que
será sobrescrito. É dado (fseek) para a posicação deste registro, lê-se a informação do campo "removido", e caso seja '1', indicador
de que o campo está realmente removido, lê se o encadeamento, o campo do topo recebe o encadeamento daquele registro, assim desempilhando
os RRNs dos registros. Em seguida, utiliza-se (fseek) para retornar 5 bytes, para o início do registro, e utilizando a funçãp (escrever_
no_arquivo_dados), sobrescreve-se o registro apagado. Caso o campo "removido" não tenha o valor '1', é retornada uma mensagem de erro.

Em seguida, é verificado se o número de registros totais é diferente de 0, e se for, significa que novos registros foram adicionados, e
utilizando a função (calcula_pag_disco), o número de páginas de disco do registro de cabeçalho é atualizado. Por fim, o "status" do registro
é marcado como '1', indicando estar consistente, retorna-se para o início do arquivo com (fseek), e o registro de cabeçalho é sobrescrito com
a função (escrever_no_arquivo_cabecalho), e o arquivo é fechado com (fclose).

*/
void comando5(char* nome_do_arquivo_entrada){

    FILE* arquivo_entrada = abrir_leitura_e_escrita_binario(nome_do_arquivo_entrada);
    if(arquivo_entrada == NULL){
        return 0;
    }
    int num_registros_total = 0;//total de registros
    int num_registros_adicionados = 0;
    int byte_offset = 0;

    reg_cabecalho* novo_reg_cabecalho = cria_registro_cabecalho();
    reg_dados* novo_reg_dados = cria_registro_dados();

    ler_reg_cabecalho(arquivo_entrada, novo_reg_cabecalho);
    //checa se o status está consistente
    if(novo_reg_cabecalho->status[0] == '0'){
        printf("Arquivo inconsistente\n");
        return 0;
    }

    scanf("%d", &num_registros_adicionados);

    for(int i = 0; i < num_registros_adicionados; i++){//joga o lido do teclado na struct

        ler_registros_dados_teclado(novo_reg_dados);
    
        //verifica se topo é -1, se é -1, então não há registros removidos
        if(novo_reg_cabecalho->topo == -1){
            byte_offset = TAM_PAG_DISCO + ((novo_reg_cabecalho->proxRRN)*TAM_REG_DADOS);//calcula o byteoffset
            fseek(arquivo_entrada, byte_offset, SEEK_SET);//vai para nova posição

            escrever_no_arquivo_dados(arquivo_entrada, novo_reg_dados);//escreve no arquivo de dados 
            novo_reg_cabecalho->proxRRN++;//atualiza o proximo RRN
            num_registros_total = (ftell(arquivo_entrada) - TAM_PAG_DISCO)/TAM_REG_DADOS;//remove a pagina de disco do reg cabecalho
        }
        else if(novo_reg_cabecalho->topo != -1){

            byte_offset = TAM_PAG_DISCO + ((novo_reg_cabecalho->topo)*TAM_REG_DADOS);//calcula o byteoffset
            fseek(arquivo_entrada, byte_offset, SEEK_SET);//vai para nova posição

            fread(novo_reg_dados->removido[0], sizeof(char), 1, arquivo_entrada);//lê se o registro é removido
            novo_reg_dados->removido[1] = '\0';

            if(novo_reg_dados->removido[0] == '1'){//se é removido
                fread(&novo_reg_dados->encadeamento, sizeof(int), 1, arquivo_entrada);//pega o encadeamento, isto é, novo espaço livre
                novo_reg_cabecalho->topo = novo_reg_dados->encadeamento;//desempilha no topo
                fseek(arquivo_entrada, -5, SEEK_CUR);//volta pro início do registro
                escrever_no_arquivo_dados(arquivo_entrada, novo_reg_dados);//escreve no arquivo de dados 
            }
            else{
                printf("Posição de registro removido inválida\n");
                return 0;
            }
        }
    }
    if(num_registros_total != 0){
        novo_reg_cabecalho->nroPagDisco = calcula_pag_disco(num_registros_total);//atualiza a quantidade de páginas de disco 
    }
    strcpy(novo_reg_cabecalho->status, "1");//declara arquivo como status 1
    fseek(arquivo_entrada, 0, SEEK_SET);//volta para inicio do arquivo
    escrever_no_arquivo_cabecalho(arquivo_entrada, novo_reg_cabecalho);//reescreve registro de cabeçalho

    fclose(arquivo_entrada);
}