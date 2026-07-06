#ifndef INSTRUCAO_H
#define INSTRUCAO_H

typedef enum{
    HALT = -1,
    SUM = 0,
    SUB = 1,
    MOV_REG_RAM = 2,
    MOV_RAM_REG = 3
} Opcode;

typedef struct {
    Opcode opcode;
    int add1;
    int add2;
    int add3;
} Instrucao;

#endif