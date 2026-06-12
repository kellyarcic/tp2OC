#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../include/cpu.h"
#include "../include/ram.h"
#include "../include/mmu.h"

// Função que gera um programa de teste na memória simulando repetições de endereços
Instrucao* gerarProgramaSimulado(int qtdInstrucoes, double taxaRepeticao, int *tamPrograma) {
    Instrucao *programa = (Instrucao*) malloc(sizeof(Instrucao) * qtdInstrucoes);
    
    // Conjunto de endereços "populares" para simular a repetição
    int enderecosComuns[10] = {4, 8, 12, 16, 20, 24, 28, 32, 36, 40};
    
    for (int i = 0; i < qtdInstrucoes - 1; i++) {
        // Decide se vai repetir um endereço comum ou usar um endereço novo (aleatório)
        double r = (double)rand() / RAND_MAX;
        int add1, add2, add3;
        
        if (r < taxaRepeticao) {
            // Sorteia dentro dos endereços que sempre são repetidos
            add1 = enderecosComuns[rand() % 10];
            add2 = enderecosComuns[rand() % 10];
            add3 = enderecosComuns[rand() % 10];
        } else {
            // Sorteia um endereço qualquer na RAM
            add1 = (rand() % 50) * 4;
            add2 = (rand() % 50) * 4;
            add3 = (rand() % 50) * 4;
        }
        
        // Sorteia uma operação entre SUM (0), SUB (1), MOV (2 ou 3) ou LOAD (4)
        int op = rand() % 5;
        programa[i] = (Instrucao){op, add1, add2, add3};
    }
    
    // A última instrução precisa ser o HALT
    programa[qtdInstrucoes - 1] = (Instrucao){-1, 0, 0, 0};
    
    *tamPrograma = qtdInstrucoes;
    return programa;
}

void rodarSimulacao(double taxaRepeticao, const char* nomeJanela) {
    int tamanhoRAM = 1024; 
    int tamPrograma = 0;
    int qtdInstrucoes = 10000; // Simula 10 mil instruções para dar um resultado estatístico bom

    // Gera o programa simulando a taxa de repetição pedida pelo professor
    Instrucao *programaInstrucoes = gerarProgramaSimulado(qtdInstrucoes, taxaRepeticao, &tamPrograma);

    int maquinas[5][3] = {
        {8, 16, 32},    // M1
        {32, 64, 128},  // M2
        {16, 64, 256},  // M3
        {8, 32, 128},   // M4
        {16, 32, 64}    // M5
    };


    for(int i = 0; i < 5; i++) {
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
    srand(time(NULL)); // Inicializa a semente aleatória

    rodarSimulacao(0.50, "50%% DE REPETICAO");
    rodarSimulacao(0.75, "75%% DE REPETICAO");
    rodarSimulacao(0.90, "90%% DE REPETICAO");

    return 0;
}