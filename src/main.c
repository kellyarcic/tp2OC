#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../include/cpu.h"
#include "../include/ram.h"
#include "../include/mmu.h"
#include "../include/config.h"

Instrucao* lerProgramaDoArquivo(const char* nomeArquivo, int *tamPrograma) {
    FILE *arquivo = fopen(nomeArquivo, "r");
    if (arquivo == NULL) {
        printf("Erro: Nao foi possivel abrir o arquivo %s. Garanta que ele esteja na pasta tp2Oc/\n", nomeArquivo);
        exit(1);
    }

    int qtdInstrucoes = QTD_INSTRUCOES; 
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
    int tamanhoRAM = TAMANHO_RAM; 
    int tamPrograma = 0;

    Instrucao *programaInstrucoes = lerProgramaDoArquivo(nomeArquivo, &tamPrograma);

    int maquinas[5][3] = {
        {M1_C1, M1_C2, M1_C3},
        {M2_C1, M2_C2, M2_C3},
        {M3_C1, M3_C2, M3_C3},
        {M4_C1, M4_C2, M4_C3},
        {M5_C1, M5_C2, M5_C3}
    };

   //rodar  a quantd de maquinas 
    for(int i = 0; i < 5; i++) {
        printf("\n----------------------\n");
        printf("Iniciando M%d\n",i+1);
        RAM ram;
        criarRAMVazia(&ram, tamanhoRAM);
        
        MMU* mmu = inicializarMMU(maquinas[i][0], maquinas[i][1], maquinas[i][2]);
        
        CPU cpu;
        carregarPrograma(&cpu, programaInstrucoes, tamPrograma);
        
        clock_t inicio = clock();
        executarCPU(&cpu, &ram, mmu);
        clock_t fim = clock();
        double tempoExecucao = (double)(fim - inicio) / CLOCKS_PER_SEC;
        printf("Estatisticas M%d\n",i+1);
        
        imprimirEstatisticas(mmu, i +1, tempoExecucao);

        liberarMMU(mmu);
        destruirRAM(&ram);
    }
    

    free(programaInstrucoes);
}

int main() {
    
    rodarSimulacaoDoArquivo(ARQUIVO_INSTRUCOES);

    return 0;
}