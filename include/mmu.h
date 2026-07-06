#ifndef MMU_H
#define MMU_H

#include "bloco.h"
#include "ram.h"

typedef struct {
    BlocoMemoria* c1; // Cache L1
    BlocoMemoria* c2; // Cache L2
    BlocoMemoria* c3; // Cache L3
    int tamC1, tamC2, tamC3;

    // Contadores para estatisticas
    int hitsC1, missesC1;
    int hitsC2, missesC2;
    int hitsC3, missesC3;
    int acessosRAM;
    int custoTotal;
    
    unsigned long long relogioLRU; // Marcador de tempo lógico para o LRU
} MMU;

MMU* inicializarMMU(int t1, int t2, int t3);
void liberarMMU(MMU* mmu);
int buscarNasMemorias(MMU* mmu, int enderecoGlobal, bool isEscrita, int valorEscrita, RAM* ram);
void imprimirEstatisticas(MMU* mmu, int maquina);

#endif