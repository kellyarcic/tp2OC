#include "../include/cpu.h"
#include <stdio.h>

void carregarPrograma(CPU *cpu, Instrucao *programa, int tamanho){
    cpu->programa = programa;
    cpu->tamanhoPrograma = tamanho;
}

void executarCPU(CPU *cpu, RAM *ram, MMU *mmu){
    cpu->opcode = 0;
    cpu->pc = 0;
    while(cpu->opcode != -1 && cpu->pc < cpu->tamanhoPrograma){
        Instrucao atual = cpu->programa[cpu->pc];

        cpu->opcode = atual.opcode;
        switch(cpu->opcode){
            case -1:
                //printf("\n[HALT]\n");
                break;

            case 0: // SUM
                cpu->registrador1 = buscarNasMemorias(mmu, atual.add1, false, 0, ram);
                cpu->registrador2 = buscarNasMemorias(mmu, atual.add2, false, 0, ram);
                cpu->registrador1 += cpu->registrador2;
                buscarNasMemorias(mmu, atual.add3, true, cpu->registrador1, ram);
                #if DEBUG_CPU
                printf("\n[SUM]");
                printf("\nRAM[%d] + RAM[%d] -> RAM[%d]\n", atual.add1, atual.add2, atual.add3);
                #endif
                //printf("\n[SUM]");
                //printf("\nRAM[%d] + RAM[%d] -> RAM[%d]\n",atual.add1,atual.add2,atual.add3);
                break;

            case 1: // SUB
                cpu->registrador1 = buscarNasMemorias(mmu, atual.add1, false, 0, ram);
                cpu->registrador2 = buscarNasMemorias(mmu, atual.add2, false, 0, ram);
                cpu->registrador1 -= cpu->registrador2;
                buscarNasMemorias(mmu, atual.add3, true, cpu->registrador1, ram);

                //printf("\n[SUB]");
                //printf("\nRAM[%d] - RAM[%d] -> RAM[%d]\n",atual.add1,atual.add2,atual.add3);
                break;

            case 2: // MOV REG -> RAM
                if(atual.add1 == 1){
                    buscarNasMemorias(mmu, atual.add2, true, cpu->registrador1, ram);
                }
                else{
                    buscarNasMemorias(mmu, atual.add2, true, cpu->registrador2, ram);
                }
                //printf("\n[MOV REG -> RAM]\n");
                break;

            case 3: // MOV RAM -> REG
                if(atual.add1 == 1){
                    cpu->registrador1 = buscarNasMemorias(mmu, atual.add2, false, 0, ram);
                }
                else{
                    cpu->registrador2 = buscarNasMemorias(mmu, atual.add2, false, 0, ram);
                }
                //printf("\n[MOV RAM -> REG]\n");
                break;
                
            default:
                printf("\nOpcode invalido.\n");
                break;
        }
        cpu->pc++;
    }
}