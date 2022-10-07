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
    FILE* arquivo_saida = abrir_escrita_binario(nome_do_arquivo_saida);

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

int printa_registro(reg_dados* reg, FILE* arquivo_entrada){

    //se o arquivo não está no final, printa o registro
    if(fread(reg->removido,sizeof(char), 1, arquivo_entrada) != 0){
        char buffer[1]='0';
        char nome_pops[TAM_MAX_CAMPO_VARIAVEL];
        char nome_pais[TAM_MAX_CAMPO_VARIAVEL];
        int contador1=0,contador2=0,tam_lixo;

        if (reg->removido == '0'){

            fread(reg->encadeamento, sizeof(int), 1, arquivo_entrada);
            fread(reg->idConecta,sizeof(int), 1, arquivo_entrada);
            fread(reg->siglaPais,sizeof(char)*2, 1, arquivo_entrada);
            fread(reg->idPoPsConectado,sizeof(int), 1, arquivo_entrada);
            fread(reg->unidadeMedida,sizeof(char), 1, arquivo_entrada);
            fread(reg->velocidade,sizeof(int), 1, arquivo_entrada);
            
            while (buffer!='|')
            {
                fscanf(arquivo_entrada,"%c",&buffer);

                nome_pops[contador1]=buffer;
                contador1+=1;
                nome_pops[contador1]='/0';

            }

            buffer[0] = '0';

             while (buffer!='|')
            {
                fscanf(arquivo_entrada,"%c",&buffer);
                
                nome_pais[contador2]=buffer;
                contador2+=1;
                nome_pais[contador2]='/0';
            }
            
            tam_lixo=TAM_REG_DADOS-contador1-contador2-20;

            if (tam_lixo!=0){
            char lixo[tam_lixo];
            fread(lixo,sizeof(char)*tam_lixo, 1, arquivo_entrada);}
        
            printf("Identificador do ponto: %d\n", reg->idConecta);

            if (nome_pops[0] != '|'){
                printf("Nome do ponto: %s\n", nome_pops);}
            if (nome_pais[0] != '|'){
                printf("Pais de localizacao: %s\n", nome_pais);}
            if (reg->siglaPais[0] != '$'){
                printf("Sigla do pais: %s\n", reg->siglaPais);}
            if (reg->idPoPsConectado !=-1){
                printf("Identificador do ponto conectado: %d\n", reg->idPoPsConectado);}
            if (reg->velocidade != -1){
            printf("Velocidade de transmissao: %d %s\n", reg->velocidade, reg->unidadeMedida);}

            printf("/n");

        }
        else{

            char buffer[1]='0';
            char nome_pops[TAM_MAX_CAMPO_VARIAVEL];
            char nome_pais[TAM_MAX_CAMPO_VARIAVEL];
            int contador1=0,contador2=0,tam_lixo;

            fread(reg->encadeamento, sizeof(int), 1, arquivo_entrada);
            fread(reg->idConecta,sizeof(int), 1, arquivo_entrada);
            fread(reg->siglaPais,sizeof(char)*2, 1, arquivo_entrada);
            fread(reg->idPoPsConectado,sizeof(int), 1, arquivo_entrada);
            fread(reg->unidadeMedida,sizeof(char), 1, arquivo_entrada);
            fread(reg->velocidade,sizeof(int), 1, arquivo_entrada);
            
            while (buffer!='|')
            {
                fscanf(arquivo_entrada,"%c",&buffer);

                nome_pops[contador1]=buffer;
                contador1+=1;
                nome_pops[contador1]='/0';

            }

            buffer[0] = '0';

             while (buffer!='|')
            {
                fscanf(arquivo_entrada,"%c",&buffer);
                
                nome_pais[contador2]=buffer;
                contador2+=1;
                nome_pais[contador2]='/0';
            }
            
            tam_lixo=TAM_REG_DADOS-contador1-contador2-20;

            if (tam_lixo!=0){
            char lixo[tam_lixo];
            fread(lixo,sizeof(char)*tam_lixo, 1, arquivo_entrada);}
        }
    }
    //se o arquivo chega no final, retorna a flag de parada
    else{
        return 0;
    }

    
}

void comando2(char*nome_do_arquivo_entrada){

    FILE* arquivo_entrada = abrir_leitura(nome_do_arquivo_entrada);

    reg_dados* novo_reg_dados =  cria_registro_dados();
    reg_cabecalho* novo_reg_cabecalho = cria_registro_cabecalho();

    fread(novo_reg_cabecalho->status, sizeof(char), 1, arquivo_entrada);
    fread(novo_reg_cabecalho->topo, sizeof(int), 1, arquivo_entrada);
    fread(novo_reg_cabecalho->proxRRN, sizeof(int), 1, arquivo_entrada);
    fread(novo_reg_cabecalho->nroRegRem, sizeof(int), 1, arquivo_entrada);
    fread(novo_reg_cabecalho->nroPagDisco, sizeof(int), 1, arquivo_entrada);
    fread(novo_reg_cabecalho->qttCompacta, sizeof(int), 1, arquivo_entrada);

    while (printa_registro(novo_reg_dados, arquivo_entrada) != 0);
    
}