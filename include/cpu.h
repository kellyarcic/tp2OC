#ifndef CPU_H
#define CPU_H

#include "instrucao.h"
#include "ram.h"
#include "mmu.h"

// Mantém a estrutura base do seu TP1, adicionando controle de instruções
typedef struct {
    int registrador1;
    int registrador2;
    int pc;
    Instrucao *programa;
    int opcode;
    int tamanhoPrograma;
} CPU;

void carregarPrograma(CPU *cpu, Instrucao *programa, int tamanho);
void executarCPU(CPU *cpu, RAM *ram, MMU *mmu);

#endif