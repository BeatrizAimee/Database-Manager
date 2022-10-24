#include "funcoes_auxiliares.h"
#include "funcoes_checagem.h"
#include "funcoes_de_abertura.h"
#include "funcoes_de_busca.h"
#include "funcoes_de_escrita.h"
#include "funcoes_de_leitura.h"
#include "funcoes_fornecidas.h"

/*

Função responsável por realizar a funcionalidade 1, em que são criados dois
ponteiros, um para registro da dados e outro para registro de cabeçalho, em
seguida é criado um buffer temporário para armazenar as linhas do arquivo csv.
Dois ponteiros um para um arquivo de entrada e outro para saída são criados, e
dois contadores, um para a quantidade de registros lidos e outro para o número
de páginas de disco são criados. Em seguida, é escrito no arquivo de saída o
cabeçalho inicial usando a função (escrever_no_arquivo_cabecalho), e primeira
linha do arquivo csv é "pulada" utilizando 2 (fgets).

Em seguida, é feito um loop (while), enquanto (fgets) não retornar nada (o que
significa que não há mais linhas para ler no arquivo csv), o contador de
registros aumenta, e os dados são lidos do arquivo e gravados na struct pela
função (ler_dados), e em seguida escrito no arquivo utilizando a função
(escrever_no_arquivo_dados).

Em seguida, o nro de páginas de disco é calculado pela função
(calcula_pag_disco), e o registro de cabeçalho é atualizado no campo "status",
indicando que o arquivo não está corrompido, o campo "proxRRN" recebe o contador
de registros, indicando o RRN para um próximo registro a ser gravado, e o nro de
páginas de disco ocupadas é atualizado. Um (fseek) retorna para o início do
arquivo, e o registro de cabeçalho é sobrescrito utilizando a função
(escrever_no_arquivo_cabecalho).

Por fim, o arquivo de entrada e saída são fechados com (fclose).

*/
void comando1(char *nome_do_arquivo_entrada, char *nome_do_arquivo_saida) {

  FILE *arquivo_entrada = abrir_leitura(nome_do_arquivo_entrada);
  if (arquivo_entrada == NULL) return;
  FILE *arquivo_saida = abrir_escrita_binario(nome_do_arquivo_saida);
  if (arquivo_saida == NULL) return;

  char buffer[TAM_REG_DADOS];
  reg_dados *novo_reg_dados = cria_registro_dados();
  reg_cabecalho *novo_reg_cabecalho = cria_registro_cabecalho();

  int cont_registros = 0;

  escrever_no_arquivo_cabecalho(arquivo_saida, novo_reg_cabecalho); // incializa cabeçalho
  fgets(buffer, 64, arquivo_entrada);                // pula primeira linha
  fgets(buffer, 64, arquivo_entrada);                // pula primeira linha

  while(fgets(buffer, 64, arquivo_entrada)){
    cont_registros++; // conta registros
    ler_dados(buffer, novo_reg_dados);
    escrever_no_arquivo_dados(arquivo_saida, novo_reg_dados);
  }

  novo_reg_cabecalho->nroPagDisco = calcula_pag_disco(cont_registros); // calcula o número de páginas de disco
  strcpy(novo_reg_cabecalho->status, "1"); // altera valores do registro de cabeçalho conforme necessidade
  novo_reg_cabecalho->proxRRN = cont_registros;

  fseek(arquivo_saida, 0, SEEK_SET); // volta para inicio do arquivo

  escrever_no_arquivo_cabecalho(arquivo_saida, novo_reg_cabecalho); // atualiza cabeçalho

  free(novo_reg_dados);
  free(novo_reg_cabecalho);

  fclose(arquivo_entrada);
  fclose(arquivo_saida);

  binarioNaTela(nome_do_arquivo_saida);
}

/*

Função responsável por realizar a funcionalidade 2, em que são criados dois
ponteiros, um para registro da dados e outro para registro de cabeçalho. Em
seguida é lido o registro de cabeçalho e checada a consistencia do arquivo.

Se nao existirem registros, ou seja, o arquivo só possui a pagina de disco referente ao arquivo de cabeçalho, é devolvido a mensagem 'Registro Inexistente' e é dado um return. 

Caso contrario, existem arquivos a ser lidos, e é feito um loop (while). Esse faz a funçao confere_registro (que confere se o registro foi removido e printa-o caso negativo) enquanto nao retornar a flag que o arquivo acabou, ou seja, le os registros e printa-os enquanto houverem registros. 

Entao, printa-se o numero de paginas de disco lidas, da-se free nos ponteiros e fecha-se o arquivo

*/

void comando2(char *nome_do_arquivo_entrada){

  FILE *arquivo_entrada = abrir_leitura_binario(nome_do_arquivo_entrada);
  if(arquivo_entrada == NULL) return;

  reg_dados *novo_reg_dados = cria_registro_dados();
  reg_cabecalho *novo_reg_cabecalho = cria_registro_cabecalho();

  ler_reg_cabecalho(arquivo_entrada, novo_reg_cabecalho);

  if(checa_consistencia(novo_reg_cabecalho) != 0){
    free(novo_reg_dados);
    free(novo_reg_cabecalho);
    fclose(arquivo_entrada);
    return;
  }
  if(novo_reg_cabecalho->nroPagDisco == 1){ // se nao existirem registros
    printf("Registro inexistente.\n\n");
    printf("Numero de paginas de disco: %d\n\n", novo_reg_cabecalho->nroPagDisco); //printa numero de paginas de disco

    free(novo_reg_dados);
    free(novo_reg_cabecalho);

    fclose(arquivo_entrada);
    return;
  }

  while(confere_remocao(novo_reg_dados, arquivo_entrada) != 0); // enquanto o confere remoçao não retornar 0, ainda há registros a serem lidos

  printf("Numero de paginas de disco: %d", novo_reg_cabecalho->nroPagDisco); // printa numero de paginas de disco
  printf("\n\n");

  free(novo_reg_dados);
  free(novo_reg_cabecalho);

  fclose(arquivo_entrada);
}

/*

Função responsável por realizar a funcionalidade 3, em que são criados dois
ponteiros, um para registro da dados e outro para registro de cabeçalho. Em
seguida é lido o registro de cabeçalho e checada a consistencia do arquivo. Tambem sao criadass e inicializadas variaveis inteiras para armazenar o numero de buscas realizadas, a posiçao referente ao campo que será buscado (cada número indica uma posição e representa um campo), o valor que o campo possui, se inteiro, e o numero de registros encontrados nas buscas. Por ultimo uma variavel de tipo char é criada para armazenar o conteudo do campo, se este for uma string. 

Então, é recebido o numero de buscas, e enquanto as buscas não acabarem ocorre um loop "for". Dentro dele, a variavel pos_campo passa a receber o inteiro retornado da função ler_campo(), que representa um campo especifico. 

Se pos_campo for 0, 2 ou 4, significa que o campo armazena um inteiro. Em seguida, é inicializada a variavel inteira num_RRN, apenas para completar os argumentos necessarios para a função le_arquivo(). Então o conteudo do campo é lido e guardado na variavel valor e o numero de registros encontrados é ressetado como 0. Continuando, entra-se em um loop while(1), que realiza a seguinte serie: le um registro com a função le_arquivo() e verifica se nao é enviada a flag que o arquivo terminou. Se o arquivo não termina (o retorno é diferente de 0), é verificado se o registro existe. Se ele existe, é feita uma comparação do conteudo buscado com o conteudo do registro lido com a função compara_campo_int(), e se o retorno é 1 significa que o registro buscado foi encontrado. Então, se o registro foi encontrado é feito o print do registro desejado, por meio da função printa_busca(), e o numero de registros encontrados aumenta. Agora, se na primeira checagem o retorno é 0, indicando que o arquivo terminou, verifica-se se nao foram encontrados registros, ou seja, se o numero de registros encontrados continua 0. Se for o caso, é enviada a mensagem "Registro inexistente". Por fim, o loop é quebrado. 

Se pos_campo for 1, 3, 5 ou 6, significa que o campo armazena uma string. Em seguida, são realizados os mesmos passos da condição anterior, com a diferença que o conteúdo do campo é lido com a função scan_quote_string() e guardado na variavel buffer, e a comparaçao do conteudo buscado é feita pela função compara_campo_string(). 

Depois de encontrar os resultados de uma busca, é feito um fseek para o inicio da segunda pagina de disco (pulando o registro de cabeçalho), e o processo se repete, até o fim do loop for(). 

Saindo do loop é dado free() nos ponteiros alocados e o arquivo é fechado
*/
void comando3(char *nome_do_arquivo_entrada){

  FILE *arquivo_entrada = abrir_leitura_binario(nome_do_arquivo_entrada);
  if(arquivo_entrada == NULL) return;

  int num_buscas = 0; // numero de buscas a serem feitas
  int pos_campo = -1; // posição do campo recebido
  char buffer[24];    // buffer de string para o campo recebido
  int valor = 0;      // buffer de inteiro para o campo recebido
  int num_registros_encontrados = 0; // conta quantos registros foram encontrados nas buscas

  reg_dados *novo_reg_dados = cria_registro_dados();
  reg_cabecalho *novo_reg_cabecalho = cria_registro_cabecalho(); // cria um novo registro de cabeçalho

  ler_reg_cabecalho(arquivo_entrada,novo_reg_cabecalho); // lê o registro de cabeçalho

  if(checa_consistencia(novo_reg_cabecalho) != 0){ // se está inconsistente, retorna
    free(novo_reg_dados);
    free(novo_reg_cabecalho);
    fclose(arquivo_entrada);
    return;
  }

  scanf("%d", &num_buscas);
  for(int i = 0; i < num_buscas; i++){ // enquanto as buscas não acabarem
    printf("Busca %d\n", (i+1));
    pos_campo = ler_campo(); // pega a posição do campo que está sendo buscado
    if(pos_campo == 0 || pos_campo == 2 || pos_campo == 4) { // se for um campo de inteiro
      int num_RRN = -1;
      scanf("%d", &valor);// pega input do valor do campo
      num_registros_encontrados = 0;         // reseta contador para nova busca
      while(1){
        if(le_arquivo(novo_reg_dados, arquivo_entrada, &num_RRN) != 0){ // enquanto não termina o arquivo
          if(novo_reg_dados->removido[0] != '1') {
            if(compara_campo_inteiro(pos_campo, valor, novo_reg_dados) == 1){ // se o registro foi encontrado
              printa_registro(novo_reg_dados);
              num_registros_encontrados++;
            }
          }
        } 
        else{ // se ler tudo e não achar sai do loop
          if(num_registros_encontrados == 0) printf("Registro inexistente.\n\n");
          break;
        }
      }
    }
    else if(pos_campo == 1 || pos_campo == 3 || pos_campo == 5 || pos_campo == 6){ // se for campo de string
      int num_RRN = -1;
      scan_quote_string(buffer);
      num_registros_encontrados = 0;
      while(1){
        if(le_arquivo(novo_reg_dados, arquivo_entrada, &num_RRN) != 0) {
          if(novo_reg_dados->removido[0] != '1'){
            if (compara_campo_string(pos_campo, buffer, novo_reg_dados) == 1){      
              printa_registro(novo_reg_dados);// printa o resultado da busca
              num_registros_encontrados++; // aumenta o número de registros encontrados
            }
          }
        }
        else { // se ler tudo e não achar sai do loop
          if(num_registros_encontrados == 0) printf("Registro inexistente.\n\n"); // se não houveram registros encontrados printa inexistente
          break; // quebra o loop
        }
      }
    }
    printf("Numero de paginas de disco: %d\n\n", novo_reg_cabecalho->nroPagDisco); //printa numero de paginas de disco
    fseek(arquivo_entrada, TAM_PAG_DISCO, SEEK_SET); //volta pro inicio do arquivo após o registro de cabeçalho para nova busca
  }
  free(novo_reg_dados);
  free(novo_reg_cabecalho);
  fclose(arquivo_entrada);
}


/*

Função responsável por realizar a funcionalidade 4, em que são criados dois
ponteiros, um para registro da dados e outro para registro de cabeçalho. Em
seguida é lido o registro de cabeçalho e checada a consistencia do arquivo. Tambem sao criadass e inicializadas variaveis inteiras para armazenar o numero de buscas realizadas, a posiçao referente ao campo que será buscado (cada número indica uma posição e representa um campo), o valor que o campo possui, se inteiro, e o numero de registros removidos. Por ultimo uma variavel de tipo char é criada para armazenar o conteudo do campo, se este for uma string. 

Então, é recebido o numero de buscas, e enquanto as buscas não acabarem ocorre um loop "for". Dentro dele, a variavel pos_campo passa a receber o inteiro retornado da função ler_campo(), que representa um campo especifico. 

Se pos_campo for 0, 2 ou 4, significa que o campo armazena um inteiro. Em seguida, é inicializada a variavel inteira num_RRN, que guarda o numero do ultimo RRN lido. Continuando, entra-se em um loop while(), que realiza a seguinte serie enquanto a função le_arquivo() nao retorna a flag que o arquivo terminou: é conferido se o registro foi removido. Caso negativo, é feita uma comparação do conteudo buscado com o conteudo do registro lido com a função compara_campo_int(), e se o retorno é 1 significa que o registro buscado foi encontrado. Então, se o registro foi encontrado ele é removido, por meio da função apaga_registro(), e o numero de registros removidos aumenta. 

Se pos_campo for 1, 3, 5 ou 6, significa que o campo armazena uma string. Em seguida, são realizados os mesmos passos da condição anterior, com a diferença que o conteúdo do campo é lido com a função scan_quote_string() e guardado na variavel buffer, e a comparaçao do conteudo buscado é feita pela função compara_campo_string(). 

Depois de encontrar os resultados de uma busca, é feito um fseek para o inicio do arquivo e o processo se repete, até o fim do loop for(). 

Saindo do loop é atualizado na RAM o numero de registros removidos, no cabeçalho, e em seguida escrito no cabeçalho com a função escrever_no_arquivo_cabecalho(). Entao é dado free() nos ponteiros alocados e o arquivo é fechado
*/

void comando4(char *nome_do_arquivo_entrada){

  FILE *arquivo_entrada = abrir_leitura_e_escrita_binario(nome_do_arquivo_entrada);
  if(arquivo_entrada == NULL) return;

  int num_buscas = 0; // numero de buscas a serem feitas
  int pos_campo = -1; // posição do campo recebido
  char buffer[24];    // buffer de string para o campo recebido
  int valor = 0;      // buffer de inteiro para o campo recebido
  int num_registros_removidos = 0; // conta quantos registros foram removidos nas buscas

  reg_dados *novo_reg_dados = cria_registro_dados();
  reg_cabecalho *novo_reg_cabecalho = cria_registro_cabecalho(); // cria um novo registro de cabeçalho
  ler_reg_cabecalho(arquivo_entrada, novo_reg_cabecalho); // lê o registro de cabeçalho

  if(checa_consistencia(novo_reg_cabecalho) != 0){ // se está inconsistente, retorna
    free(novo_reg_dados);
    free(novo_reg_cabecalho);
    fclose(arquivo_entrada);
    return;
  }

  scanf("%d", &num_buscas);

  for(int i = 0; i < num_buscas; i++){ // enquanto as buscas não acabarem
    pos_campo = ler_campo(); // pega a posição do campo que está sendo buscado

    if(pos_campo == 0 || pos_campo == 2 || pos_campo == 4) { // se for um campo de inteiro
      int num_RRN = -1;//num_registros_removidos = 0
      scanf("%d", &valor); // pega input do valor do campo

      while(le_arquivo(novo_reg_dados, arquivo_entrada, &num_RRN) != 0){ // enquanto não termina o arquivo
        if(novo_reg_dados->removido[0] == '0'){//se o registro nao foi removido
          if(compara_campo_inteiro(pos_campo, valor, novo_reg_dados) == 1){ // se o registro foi encontrado
            apaga_registro(arquivo_entrada, novo_reg_dados, novo_reg_cabecalho, &num_RRN);
            num_registros_removidos++;
          }
        }
      }
    }
    else if(pos_campo == 1 || pos_campo == 3 || pos_campo == 5 || pos_campo == 6){ // se for campo de string
      int num_RRN = -1;
      scan_quote_string(buffer);

      while(le_arquivo(novo_reg_dados, arquivo_entrada, &num_RRN) != 0){
        if(novo_reg_dados->removido[0] == '0') {
          if(compara_campo_string(pos_campo, buffer, novo_reg_dados) == 1){
            apaga_registro(arquivo_entrada, novo_reg_dados, novo_reg_cabecalho, &num_RRN);
            num_registros_removidos++;
          }
        }
      }
    }
    fseek(arquivo_entrada, TAM_PAG_DISCO, SEEK_SET);
  }

  novo_reg_cabecalho->nroRegRem += num_registros_removidos;
  fseek(arquivo_entrada, 0, SEEK_SET);
  escrever_no_arquivo_cabecalho(arquivo_entrada, novo_reg_cabecalho);

  free(novo_reg_dados);
  free(novo_reg_cabecalho);

  fclose(arquivo_entrada);

  binarioNaTela(nome_do_arquivo_entrada);
}

/*

Função responsável por realizar a funcionalidade 5. Recebe uma string com o nome
do arquivo de entrada que será processado. o arquivo de entrada é aberto, e são
declarados um inteiro referente ao byte_offset, um inteiro referente ao número
de registros que serão adicionados, e um inteiro referente ao número total de
registros do arquivo. Um novo registro de dados e de cabeçalho é criado. Em
seguida, a função (ler_reg_cabecalho) é chamada, para ler o registro de
cabeçalho do arquivo de entrada, e é checado se o status do arquivo é '0' ou
'1'.  Se for '0', isto é, inconsistente, uma mensagem de erro é exibida e a
função retorna 0. Após isto, o número de registros adicionados é pego do
teclado.

Em seguida, um loop (for) é criado, que roda o número de vezes que um registro
será adicionado, inicializando-se sempre com a chamada da função
(ler_registros_dados_teclado), que adquire os valores dos campos dos registros
de entrada. É verificado se o topo do registro de cabeçalho do arquivo é '-1' ou
não. Se sim, isto significa que não há registros removidos no arquivo, e é então
calculado o byte_offset, somando-se o valor ocupado pelo registro de cabeçalho
ao próximo RRN disponível vezes o tamanho do registro de dados. É dado um
(fseek) para esta posição no arquivo, e a função (escrever_no_arquivo_dados) é
chamada, para escrever no arquivo o novo registro. Em seguida, o campo (proxRRN)
é atualizado para o próximo RRN, e é calculado o número total de registros do
arquivo, a partir da posição atual utilizando-se (ftell) menos o tamanho de uma
página de disco para levar em conta o registro de cabeçalho, e tudo isto
dividido pelo tamanho de um registro de dados.

Caso o topo != -1, há registros removidos no arquivo. Portanto, calcula-se o
byte_offset da mesma maneira que no primeiro caso, porém com a diferença de que
ao invés de utilizar-se o proxRRN, utiliza-se o topo da pilha como indicador do
RRN do registro que será sobrescrito. É dado (fseek) para a posicação deste
registro, lê-se a informação do campo "removido", e caso seja '1', indicador de
que o campo está realmente removido, lê se o encadeamento, o campo do topo
recebe o encadeamento daquele registro, assim desempilhando os RRNs dos
registros. Em seguida, utiliza-se (fseek) para retornar 5 bytes, para o início
do registro, e utilizando a funçãp (escrever_ no_arquivo_dados), sobrescreve-se
o registro apagado. Caso o campo "removido" não tenha o valor '1', é retornada
uma mensagem de erro.

Em seguida, é verificado se o número de registros totais é diferente de 0, e se
for, significa que novos registros foram adicionados, e utilizando a função
(calcula_pag_disco), o número de páginas de disco do registro de cabeçalho é
atualizado. Por fim, o "status" do registro é marcado como '1', indicando estar
consistente, retorna-se para o início do arquivo com (fseek), e o registro de
cabeçalho é sobrescrito com a função (escrever_no_arquivo_cabecalho), e o
arquivo é fechado com (fclose).

*/
void comando5(char *nome_do_arquivo_entrada){

  FILE *arquivo_entrada = abrir_leitura_e_escrita_binario(nome_do_arquivo_entrada);
  if(arquivo_entrada == NULL) return;

  int num_registros_total = 0; // total de registros
  int num_registros_adicionados = 0;
  int byte_offset = 0;

  reg_cabecalho *novo_reg_cabecalho = cria_registro_cabecalho();
  reg_dados *novo_reg_dados = cria_registro_dados();

  ler_reg_cabecalho(arquivo_entrada, novo_reg_cabecalho);

  if(checa_consistencia(novo_reg_cabecalho) != 0){
    free(novo_reg_dados);
    free(novo_reg_cabecalho);
    fclose(arquivo_entrada);
    return;
  }

  scanf("%d", &num_registros_adicionados);

  for(int i = 0; i < num_registros_adicionados; i++){ // joga o lido do teclado na struct

    ler_registros_dados_teclado(novo_reg_dados);

    // verifica se topo é -1, se é -1, então não há registros removidos
    if(novo_reg_cabecalho->topo == -1){
      byte_offset = TAM_PAG_DISCO + ((novo_reg_cabecalho->proxRRN) * TAM_REG_DADOS);//calcula o byteoffset
      fseek(arquivo_entrada, byte_offset, SEEK_SET);//vai para nova posição
      escrever_no_arquivo_dados(arquivo_entrada, novo_reg_dados);//escreve no arquivo de dados
      novo_reg_cabecalho->proxRRN++;// atualiza o proximo RRN
      num_registros_total = (ftell(arquivo_entrada) - TAM_PAG_DISCO)/TAM_REG_DADOS;//remove a pagina de disco do reg cabecalho
    }
    else if(novo_reg_cabecalho->topo != -1){


      byte_offset = TAM_PAG_DISCO + ((novo_reg_cabecalho->topo) * TAM_REG_DADOS); // calcula o byteoffset
      fseek(arquivo_entrada, byte_offset, SEEK_SET); // vai para nova posição

      fread(novo_reg_dados->removido, sizeof(char), 1, arquivo_entrada); // lê se o registro é removido
      novo_reg_dados->removido[1] = '\0';


      if(checa_remocao(novo_reg_dados) == 0){ // se o registro estiver removido, sobrescreve
        fread(&novo_reg_dados->encadeamento, sizeof(int), 1, arquivo_entrada); // pega o encadeamento, isto é, novo espaço livre
        novo_reg_cabecalho->topo = novo_reg_dados->encadeamento;     // desempilha no topo
        fseek(arquivo_entrada, -5, SEEK_CUR); // volta pro início do registro
        novo_reg_dados->removido[0] = '0';    // retorna status de não removido
        novo_reg_dados->encadeamento = -1; //reseta o encadeamento
        novo_reg_cabecalho->nroRegRem--;
        escrever_no_arquivo_dados(arquivo_entrada, novo_reg_dados); // escreve no arquivo de dados
      }
      else{ // se registro não estiver removido, erro
        free(novo_reg_dados);
        free(novo_reg_cabecalho);
        printf("Falha no processamento do arquivo.\n");
        fclose(arquivo_entrada);
        return;
      }
    }
  }
  if(num_registros_total != 0){
    novo_reg_cabecalho->nroPagDisco = calcula_pag_disco(num_registros_total); // atualiza a quantidade de páginas de disco
  }

  strcpy(novo_reg_cabecalho->status, "1"); // declara arquivo como status 1
  fseek(arquivo_entrada, 0, SEEK_SET);     // volta para inicio do arquivo
  escrever_no_arquivo_cabecalho(arquivo_entrada, novo_reg_cabecalho); // reescreve registro de cabeçalho

  free(novo_reg_dados);
  free(novo_reg_cabecalho);

  fclose(arquivo_entrada);

  binarioNaTela(nome_do_arquivo_entrada);
}

/*

Função responsável por realizar a funcionalidade 6. Recebe uma string com o nome
do arquivo de entrada que será processado. Inicializa-se um variavel inteira contador de registros como 0. Então o arquivo é aberto para leitura e escrita e sao criados dois ponteiros, um para o registro de dados e um para o registro de cabeçalho. Então, o registro de cabecalho é lido com a funçao ler_reg_cabecalho(), e sua concistencia é checada. 

Se o topo, no registro de cabeçalho, for igual a -1, significa que não há registros removidos, logo não há o que ser compactado. Assim, o numero de compactações é aumentado em 1 e o status ressetado para '1', é feito um fseek para o inicio do arquivo e atualizado o registro de cabeçalho com a função escrever_no_arquivo_cabecalho(). Então o arquivo é fechado e o binario é printado com a função binarioNaTela().

Se o topo for diferente de -1, significa que há registros removidos. Então, é aberto para escrita um arquivo temporário "temp.bin", e escrito o registro de cabeçalho no novo arquivo. Seguindo, entra-se em um loop while() que desfragmenta o arquivo de entrada, escrevendo os registros nao removidos no arquivo temporário, enquanto a função compacta_arquivo() nao retorna a flag que o arquivo chegou ao fim. 

Por fim, o registro de cabeçalho do arquivo temporário é atualizado com a função atualizar_reg_cabecalho(), e o arquivo temporario é fechado. Então, o arquivo de entrada original é removido usando a função remover_arquivo(), que tambem renomeia o arquivo temporário para o nome do arquivo original. É dado free() nos ponteiros utilizados e fechado o arquivo de entrada. 

*/
void comando6(char *nome_do_arquivo_entrada){

  int contador_reg = 0;

  FILE *arquivo_entrada = abrir_leitura_e_escrita_binario(nome_do_arquivo_entrada);
  if(arquivo_entrada == NULL) return;

  reg_dados *novo_reg_dados = cria_registro_dados();
  reg_cabecalho *novo_reg_cabecalho = cria_registro_cabecalho();

  ler_reg_cabecalho(arquivo_entrada, novo_reg_cabecalho);

  if (checa_consistencia(novo_reg_cabecalho) != 0){
    free(novo_reg_dados);
    free(novo_reg_cabecalho);
    fclose(arquivo_entrada);
    return;
  }

  if (novo_reg_cabecalho->topo == -1){ // se o registro de cabeçalho é -1, então não há registros removidos
    novo_reg_cabecalho->qttCompacta++;   // aumenta numero de compactações
    novo_reg_cabecalho->status[0] = '1'; // reseta status
    fseek(arquivo_entrada, 0, SEEK_SET); // volta pro inicio
    escrever_no_arquivo_cabecalho(arquivo_entrada, novo_reg_cabecalho); // reescreve registro de cabeçalho
    fclose(arquivo_entrada);                // fecha arquivos
    binarioNaTela(nome_do_arquivo_entrada); // binariona Tela
    return;
  }
  else{ // se o registro de cabeçalho é diferente de -1, então há registros removidos
    FILE *arquivo_saida = abrir_escrita_binario("temp.bin");
    if (arquivo_saida == NULL) return;

    escrever_no_arquivo_cabecalho(arquivo_saida, novo_reg_cabecalho); // escreve registro de cabeçalho no novo arquivo
    while (compacta_arquivo(novo_reg_dados, arquivo_entrada, arquivo_saida, &contador_reg) != 0); // enquanto o processa registro não retornar 0, ainda há registros a serem lidos
    atualizar_reg_cabecalho(novo_reg_cabecalho, arquivo_saida, &contador_reg); // atualiza registro de cabeçalho do arquivo de saída
    fclose(arquivo_saida);
    remover_arquivo("temp.bin", nome_do_arquivo_entrada);
  }

  free(novo_reg_dados);
  free(novo_reg_cabecalho);

  fclose(arquivo_entrada);
}