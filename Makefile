CC = gcc
MAIN = main.c
AUXFILES = funcoes_de_abertura.c funcoes_de_busca.c funcoes_de_escrita.c funcoes_de_leitura.c funcionalidades.c funcoes_auxiliares.c funcoes_checagem.c funcoes_fornecidas.c topologiaRede.c
EXECNAME = main

all:
	${CC} -o  ${EXECNAME} ${AUXFILES} ${MAIN}

run:
	./${EXECNAME}