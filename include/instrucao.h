#ifndef INSTRUCAO_H
#define INSTRUCAO_H

typedef struct {
    int opcode;
    int add1;//onde pegar o primeiro numero
    int add2;//onde pegar o segundo numero
    int add3;//onde salvar
} Instrucao;

//encerra o programa
#define HALT -1

//operacoes basicas
#define SUM 0
#define SUB 1

//movimentacao
#define MOV_REG_RAM 2
#define MOV_RAM_REG 3

//carregamento externo
#define LOAD 4

//copia registrador
#define COPY 5

#endif
