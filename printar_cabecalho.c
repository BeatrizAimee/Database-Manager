#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


#define TAM_PAG_DISCO 960
#define TAM_MAX_CAMPO_VARIAVEL 32
#define TAM_REG_DADOS 64
#define TAM_REG_CABECALHO 21
#define TAM_NOME_ARQUIVO 20


typedef struct reg_cabecalho{

    char status[2];
    int topo;
    int proxRRN;
    int nroRegRem;
    int nroPagDisco;
    int qttCompacta;
  
}reg_cabecalho;

typedef struct reg_dados{

    char removido[2];
    int encadeamento;
    int idConecta;
    char siglaPais[3];
    int idPoPsConectado;
    char unidadeMedida[2];
    int velocidade;
    char nomePoPs[TAM_MAX_CAMPO_VARIAVEL];
    char nomePais[TAM_MAX_CAMPO_VARIAVEL];
  
}reg_dados;

reg_dados* cria_registro_dados(){

    reg_dados* novo_reg;
    novo_reg = (reg_dados*)malloc(sizeof(reg_dados));

    if(novo_reg == NULL){
        return NULL;
    }

    strcpy(novo_reg->removido, "0");
    novo_reg->encadeamento = -1;

    return novo_reg;
}

reg_cabecalho* cria_registro_cabecalho(){

    reg_cabecalho* novo_reg;
    novo_reg = (reg_cabecalho*)malloc(sizeof(reg_cabecalho));

    if(novo_reg == NULL){
        return NULL;
    }

    strcpy(novo_reg->status, "0");
    novo_reg->topo = -1;
    novo_reg->proxRRN = 0;
    novo_reg->nroRegRem = 0;
    novo_reg->nroPagDisco = 0;
    novo_reg->qttCompacta = 0;

    return novo_reg;
}

FILE* abrir_leitura_e_escrita_binario(char* nome_do_arquivo){
    FILE* arquivo;
    arquivo = fopen(nome_do_arquivo, "rb+");

    if(arquivo == NULL){
        printf("Falha no processamento do arquivo.\n");
        return 0;
    }
    return arquivo;
}

void ler_reg_cabecalho(FILE* arquivo, reg_cabecalho* reg){
    fread(reg->status, sizeof(char), 1, arquivo);
    reg->status[1] = '\0';
    fread(&reg->topo, sizeof(int), 1, arquivo);
    fread(&reg->proxRRN, sizeof(int), 1, arquivo);//lê o próximo RRN disponível
    fread(&reg->nroRegRem, sizeof(int), 1, arquivo);//lê o número de registros removidos
    fread(&reg->nroPagDisco, sizeof(int), 1, arquivo);//lê o número de páginas de disco
    fread(&reg->qttCompacta, sizeof(int), 1, arquivo);//lê o qttCompacta
    fseek(arquivo, (TAM_PAG_DISCO - TAM_REG_CABECALHO), SEEK_CUR);//pula o lixo
}

void le_todos_campos_do_registro(reg_dados* reg, FILE* arquivo_entrada){
    le_campos_tam_fixo(reg,arquivo_entrada);
    le_campos_tam_variavel(reg,arquivo_entrada);
}

void le_campos_tam_fixo(reg_dados* reg, FILE* arquivo_entrada){

  // le campos fixos
    fread(&reg->encadeamento, sizeof(int), 1, arquivo_entrada);
    fread(&reg->idConecta,sizeof(int), 1, arquivo_entrada);
    fread(reg->siglaPais,sizeof(char)*2, 1, arquivo_entrada);
    reg->siglaPais[2]='\0';
    fread(&reg->idPoPsConectado,sizeof(int), 1, arquivo_entrada);
    fread(reg->unidadeMedida,sizeof(char), 1, arquivo_entrada);
    reg->unidadeMedida[1]='\0';
    fread(&reg->velocidade,sizeof(int), 1, arquivo_entrada);

}

void le_campos_tam_variavel(reg_dados* reg, FILE* arquivo_entrada){
    char buffer = '0'; // armazena character

    int pos_nomePoPs=0; // posição na string de nomePops
    int pos_nomePais=0; // posição na string de nomePais
    int tam_lixo; // tamanho do lixo 

                
    
    //le campos variaveis
    while (buffer !='|' && buffer != '$') // 
    {
        fread(&buffer, sizeof(char), 1, arquivo_entrada); // le o char

        reg->nomePoPs[pos_nomePoPs]=buffer; //constroi a string
        pos_nomePoPs+=1; //atualiza a posição
                
    }
        
    buffer = '0'; //reseta o buffer

        while (buffer !='|' && buffer != '$')
    {
        //fscanf(arquivo_entrada,"%c",buffer);
        fread(&buffer, sizeof(char), 1, arquivo_entrada); // le o char
                
        reg->nomePais[pos_nomePais]=buffer; //constroi a string
        pos_nomePais+=1; //atualiza a posição
                

    }
             
    reg->nomePoPs[pos_nomePoPs-1]='\0'; // substitui o | por um \0
    reg->nomePais[pos_nomePais-1]='\0';

           
    tam_lixo=TAM_REG_DADOS-pos_nomePoPs-pos_nomePais-20; //descobre o tamanho do lixo 
         
    if (tam_lixo!=0){  //confere se existe lixo

    char lixo[tam_lixo];  //cria a string lixo
    fread(lixo,sizeof(char)*tam_lixo, 1, arquivo_entrada); //le o lixo

    }
}


int le_arquivo(reg_dados* reg, FILE* arquivo_entrada, int*num_rrn){
    if(fread(reg->removido,sizeof(char), 1, arquivo_entrada) != 0){//se não chegou ao fim
        reg->removido[1] = '\0';
        le_todos_campos_do_registro(reg,arquivo_entrada);
        (*num_rrn)++;
        return 1;
    }
    return 0;
}

void printa_registro(reg_dados* reg){

    printf("Identificador do ponto: %d\n", reg->idConecta);
    printf("Nome do ponto: %s\n", reg->nomePoPs);//confere se não é nulo
    printf("Pais de localizacao: %s\n", reg->nomePais);
    printf("Sigla do pais: %s\n", reg->siglaPais);
    printf("Identificador do ponto conectado: %d\n", reg->idPoPsConectado);
    printf("Velocidade de transmissao: %d %sbps\n", reg->velocidade, reg->unidadeMedida);
    printf("\n");
}

int confere_remocao(reg_dados* reg, FILE* arquivo_entrada){

    int num_RRN = 0;
    int status = le_arquivo(reg, arquivo_entrada, &num_RRN);
    if(status==1){//nao é o fim do arquivo
    printa_registro(reg);
    return status;
    }
    else{
        return status;
    }
}


int main(){

    reg_cabecalho* reg_cabecalho_certo = cria_registro_cabecalho();
    reg_cabecalho* reg_cabecalho_nosso = cria_registro_cabecalho();
    reg_dados* novo_reg_dados_certo = cria_registro_dados();
    reg_dados* novo_reg_dados_nosso = cria_registro_dados();

    FILE* arq_certo = abrir_leitura_e_escrita_binario("binario12.bin");
    FILE* arq_nosso = abrir_leitura_e_escrita_binario("teste.bin");

    ler_reg_cabecalho(arq_certo, reg_cabecalho_certo);

    printf("Arquivo certo:\n");
    printf("Status: %s\n", reg_cabecalho_certo->status);
    printf("Topo: %d\n", reg_cabecalho_certo->topo);
    printf("proxRRN: %d\n", reg_cabecalho_certo->proxRRN);
    printf("nroRegRem: %d\n", reg_cabecalho_certo->nroRegRem);
    printf("nroPagDisco: %d\n", reg_cabecalho_certo->nroPagDisco);
    printf("qttCompacta: %d\n\n", reg_cabecalho_certo->qttCompacta);

    while(confere_remocao(novo_reg_dados_certo, arq_certo) != 0);

    ler_reg_cabecalho(arq_nosso, reg_cabecalho_nosso);

    printf("Arquivo nosso:\n");
    printf("Status: %s\n", reg_cabecalho_nosso->status);
    printf("Topo: %d\n", reg_cabecalho_nosso->topo);
    printf("proxRRN: %d\n", reg_cabecalho_nosso->proxRRN);
    printf("nroRegRem: %d\n", reg_cabecalho_nosso->nroRegRem);
    printf("nroPagDisco: %d\n", reg_cabecalho_nosso->nroPagDisco);
    printf("qttCompacta: %d\n\n", reg_cabecalho_nosso->qttCompacta);


    while(confere_remocao(novo_reg_dados_nosso, arq_nosso) != 0);

    return 0;
}