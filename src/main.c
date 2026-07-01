#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../include/cpu.h"
#include "../include/ram.h"
#include "../include/mmu.h"


Instrucao* lerProgramaDoArquivo(const char* nomeArquivo, int *tamPrograma) {
    FILE *arquivo = fopen(nomeArquivo, "r");
    if (arquivo == NULL) {
        printf("Erro: Nao foi possivel abrir o arquivo %s. Garanta que ele esteja na pasta tp2Oc/\n", nomeArquivo);
        exit(1);
    }

    int qtdInstrucoes = 10000; 
    Instrucao *programa = (Instrucao*) malloc(sizeof(Instrucao) * qtdInstrucoes);
    
    int i = 0;
    int op, add1, add2, add3;
    
    while (fscanf(arquivo, "%d:%d:%*d:%d:%*d:%d:%*d", &op, &add1, &add2, &add3) != EOF && i < qtdInstrucoes) {
        programa[i] = (Instrucao){op, add1, add2, add3};
        i++;
    }

    // Garante que a última instrução seja o HALT caso o arquivo acabe antes
    if (i > 0 && programa[i - 1].opcode != -1) {
        programa[i - 1] = (Instrucao){-1, 0, 0, 0};
    }
    
    fclose(arquivo);
    *tamPrograma = i;
    return programa;
}

void rodarSimulacaoDoArquivo(const char* nomeArquivo) {
    int tamanhoRAM = 1024; 
    int tamPrograma = 0;

    Instrucao *programaInstrucoes = lerProgramaDoArquivo(nomeArquivo, &tamPrograma);

    int maquinas[5][3] = {
        {8, 16, 32},    // M1
        {32, 64, 128},  // M2
        {16, 64, 256},  // M3
        {8, 32, 128},   // M4
        {16, 32, 64}    // M5
    };

   
    for(int i = 0; i < 1; i++) {
        RAM ram;
        criarRAMVazia(&ram, tamanhoRAM);
        
        MMU* mmu = inicializarMMU(maquinas[i][0], maquinas[i][1], maquinas[i][2]);
        
        CPU cpu;
        carregarPrograma(&cpu, programaInstrucoes, tamPrograma);
        
        executarCPU(&cpu, &ram, mmu);
        
        printf("Maquina M%d ", i + 1);
        imprimirEstatisticas(mmu);

        liberarMMU(mmu);
        destruirRAM(&ram);
    }
    printf("=================================================================================\n");

    free(programaInstrucoes);
}

int main() {
    
    rodarSimulacaoDoArquivo("instructions2.txt");

    return 0;
}