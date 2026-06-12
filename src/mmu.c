#include "../include/mmu.h"
#include <stdio.h>
#include <stdlib.h>

MMU* inicializarMMU(int t1, int t2, int t3) {
    MMU* mmu = (MMU*) malloc(sizeof(MMU));
    mmu->tamC1 = t1; mmu->tamC2 = t2; mmu->tamC3 = t3;
    
    mmu->c1 = (BlocoMemoria*) malloc(t1 * sizeof(BlocoMemoria));
    mmu->c2 = (BlocoMemoria*) malloc(t2 * sizeof(BlocoMemoria));
    mmu->c3 = (BlocoMemoria*) malloc(t3 * sizeof(BlocoMemoria));
    
    for(int i = 0; i < t1; i++) mmu->c1[i].endBloco = -1;
    for(int i = 0; i < t2; i++) mmu->c2[i].endBloco = -1;
    for(int i = 0; i < t3; i++) mmu->c3[i].endBloco = -1;
    
    mmu->hitsC1 = mmu->missesC1 = 0;
    mmu->hitsC2 = mmu->missesC2 = 0;
    mmu->hitsC3 = mmu->missesC3 = 0;
    mmu->acessosRAM = mmu->custoTotal = 0;
    mmu->relogioLRU = 0;
    
    return mmu;
}

void liberarMMU(MMU* mmu) {
    free(mmu->c1);
    free(mmu->c2);
    free(mmu->c3);
    free(mmu);
}

static int buscarNaCache(BlocoMemoria* cache, int tamanho, int endBloco) {
    for(int i = 0; i < tamanho; i++) {
        if(cache[i].endBloco == endBloco) return i;
    }
    return -1;
}

static void copiarBloco(BlocoMemoria* origem, BlocoMemoria* destino) {
    destino->endBloco = origem->endBloco;
    destino->atualizado = origem->atualizado;
    for(int i = 0; i < 4; i++) destino->palavras[i] = origem->palavras[i];
}

static void levarParaCache(BlocoMemoria* cache, int tamanho, BlocoMemoria* blocoSubindo, BlocoMemoria* ram, unsigned long long relogio) {
    int idx = buscarNaCache(cache, tamanho, blocoSubindo->endBloco);
    if(idx != -1) {
        cache[idx].contadorUso = relogio;
        return;
    }
    
    for(int i = 0; i < tamanho; i++) {
        if(cache[i].endBloco == -1) {
            copiarBloco(blocoSubindo, &cache[i]);
            cache[i].contadorUso = relogio;
            return;
        }
    }
    
    int lruIdx = 0;
    unsigned long long menorUso = cache[0].contadorUso;
    for(int i = 1; i < tamanho; i++) {
        if(cache[i].contadorUso < menorUso) {
            menorUso = cache[i].contadorUso;
            lruIdx = i;
        }
    }
    
    if(cache[lruIdx].atualizado && cache[lruIdx].endBloco != -1) {
        copiarBloco(&cache[lruIdx], &ram[cache[lruIdx].endBloco]);
        ram[cache[lruIdx].endBloco].atualizado = false;
    }
    
    copiarBloco(blocoSubindo, &cache[lruIdx]);
    cache[lruIdx].contadorUso = relogio;
}

int buscarNasMemorias(MMU* mmu, int enderecoGlobal, bool isEscrita, int valorEscrita, RAM* ram) {
    int endBloco = enderecoGlobal / 4;
    int offset = enderecoGlobal % 4;
    mmu->relogioLRU++;
    
    // 1. C1
    int idx = buscarNaCache(mmu->c1, mmu->tamC1, endBloco);
    if(idx != -1) {
        mmu->hitsC1++; mmu->custoTotal += 1;
        mmu->c1[idx].contadorUso = mmu->relogioLRU;
        if(isEscrita) { mmu->c1[idx].palavras[offset] = valorEscrita; mmu->c1[idx].atualizado = true; }
        return mmu->c1[idx].palavras[offset];
    }
    mmu->missesC1++;
    
    // 2. C2
    idx = buscarNaCache(mmu->c2, mmu->tamC2, endBloco);
    if(idx != -1) {
        mmu->hitsC2++; mmu->custoTotal += 10;
        mmu->c2[idx].contadorUso = mmu->relogioLRU;
        levarParaCache(mmu->c1, mmu->tamC1, &mmu->c2[idx], ram->blocos, mmu->relogioLRU);
        if(isEscrita) { mmu->c2[idx].palavras[offset] = valorEscrita; mmu->c2[idx].atualizado = true; }
        return mmu->c2[idx].palavras[offset];
    }
    mmu->missesC2++;
    
    // 3. C3
    idx = buscarNaCache(mmu->c3, mmu->tamC3, endBloco);
    if(idx != -1) {
        mmu->hitsC3++; mmu->custoTotal += 100;
        mmu->c3[idx].contadorUso = mmu->relogioLRU;
        levarParaCache(mmu->c2, mmu->tamC2, &mmu->c3[idx], ram->blocos, mmu->relogioLRU);
        levarParaCache(mmu->c1, mmu->tamC1, &mmu->c3[idx], ram->blocos, mmu->relogioLRU);
        if(isEscrita) { mmu->c3[idx].palavras[offset] = valorEscrita; mmu->c3[idx].atualizado = true; }
        return mmu->c3[idx].palavras[offset];
    }
    mmu->missesC3++;
    
    // 4. RAM
    mmu->acessosRAM++; mmu->custoTotal += 1000;
    BlocoMemoria* blocoRAM = &ram->blocos[endBloco];
    if(isEscrita) { blocoRAM->palavras[offset] = valorEscrita; blocoRAM->atualizado = true; }
    
    levarParaCache(mmu->c3, mmu->tamC3, blocoRAM, ram->blocos, mmu->relogioLRU);
    levarParaCache(mmu->c2, mmu->tamC2, blocoRAM, ram->blocos, mmu->relogioLRU);
    levarParaCache(mmu->c1, mmu->tamC1, blocoRAM, ram->blocos, mmu->relogioLRU);
    
    return blocoRAM->palavras[offset];
}

void imprimirEstatisticas(MMU* mmu) {
    int t1 = mmu->hitsC1 + mmu->missesC1;
    int t2 = mmu->hitsC2 + mmu->missesC2;
    int t3 = mmu->hitsC3 + mmu->missesC3;
    
    printf("| C1 Hit: %6.2f%% | C2 Hit: %6.2f%% | C3 Hit: %6.2f%% | RAM Acc: %4d | Custo: %7d |\n",
           t1 > 0 ? ((double)mmu->hitsC1/t1)*100 : 0,
           t2 > 0 ? ((double)mmu->hitsC2/t2)*100 : 0,
           t3 > 0 ? ((double)mmu->hitsC3/t3)*100 : 0,
           mmu->acessosRAM, mmu->custoTotal);
}