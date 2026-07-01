#include "../include/mmu.h"
#include <stdio.h>
#include <stdlib.h>


//cria e  configura a estrutura da MMU(UCM)
MMU* inicializarMMU(int t1, int t2, int t3) {
    MMU* mmu = (MMU*) malloc(sizeof(MMU));
    mmu->tamC1 = t1; mmu->tamC2 = t2; mmu->tamC3 = t3;
    
    mmu->c1 = (BlocoMemoria*) malloc(t1 * sizeof(BlocoMemoria));
    mmu->c2 = (BlocoMemoria*) malloc(t2 * sizeof(BlocoMemoria));
    mmu->c3 = (BlocoMemoria*) malloc(t3 * sizeof(BlocoMemoria));
    
    //tds os blocos comecam com -1 para indicar que as caches comecam vazias
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
//procura o numero do bloco
static int buscarNaCache(BlocoMemoria* cache, int tamanho, int endBloco) {
    for(int i = 0; i < tamanho; i++) {
        if(cache[i].endBloco == endBloco) return i;
    }
    return -1;
}
//clona o conteudo
static void copiarBloco(BlocoMemoria* origem, BlocoMemoria* destino) {
    destino->endBloco = origem->endBloco;
    destino->atualizado = origem->atualizado;
    for(int i = 0; i < 4; i++) {
        destino->palavras[i] = origem->palavras[i];
    }
}

//insere um bloco na cache aplicando as regras de LRU e White back
static void levarParaCache(BlocoMemoria* cache, int tamanho, BlocoMemoria* blocoSubindo, BlocoMemoria* ram, unsigned long long relogio) {
    int idx = buscarNaCache(cache, tamanho, blocoSubindo->endBloco); //linha onde um elemento foi encontrado
    if(idx != -1) {
        cache[idx].contadorUso = relogio;
        return;
    }
    //procura se existe alguma linha vazia (== -1)
    for(int i = 0; i < tamanho; i++) {
        if(cache[i].endBloco == -1) {
            copiarBloco(blocoSubindo, &cache[i]);
            cache[i].contadorUso = relogio;
            return;
        }
    }
    
    //LRU
    int lruIdx = 0;
    unsigned long long menorUso = cache[0].contadorUso;
    for(int i = 1; i < tamanho; i++) {
        if(cache[i].contadorUso < menorUso) {
            menorUso = cache[i].contadorUso;
            lruIdx = i;//guarda o indice que sera expulsa
        }
    }
    //WRITE-BACK
    //teste antes de jogar no lixo
    if(cache[lruIdx].atualizado && cache[lruIdx].endBloco != -1) {
        copiarBloco(&cache[lruIdx], &ram[cache[lruIdx].endBloco]); //salva de volta na RAM antes de joga-la fora
        ram[cache[lruIdx].endBloco].atualizado = false;
    }
    
    copiarBloco(blocoSubindo, &cache[lruIdx]);
    cache[lruIdx].contadorUso = relogio;
}

//busca em cascatas
int buscarNasMemorias(MMU* mmu, int enderecoGlobal, bool isEscrita, int valorEscrita, RAM* ram) {
    int endBloco = enderecoGlobal / 4;
    int offset = enderecoGlobal % 4;
    mmu->relogioLRU++;
    
    // C1
    int idx = buscarNaCache(mmu->c1, mmu->tamC1, endBloco);
    if(idx != -1) { //cache hit na c1
        mmu->hitsC1++; mmu->custoTotal += 1;
        mmu->c1[idx].contadorUso = mmu->relogioLRU;
        // Se for uma instrucao de escrita (MOV), altera o dado apenas na C1 
        if(isEscrita) {
            mmu->c1[idx].palavras[offset] = valorEscrita; 
            mmu->c1[idx].atualizado = true; }
        return mmu->c1[idx].palavras[offset];//retorn a palavra para a cpu
    }
    mmu->missesC1++;
    
    // C2
    idx = buscarNaCache(mmu->c2, mmu->tamC2, endBloco);
    if(idx != -1) { //cache hit na c2
        mmu->hitsC2++; mmu->custoTotal += 10;
        mmu->c2[idx].contadorUso = mmu->relogioLRU;
        
        // Sobe o bloco encontrado da C2 para a C1 (aplicando LRU caso a C1 esteja cheia)
        levarParaCache(mmu->c1, mmu->tamC1, &mmu->c2[idx], ram->blocos, mmu->relogioLRU);
        
        if(isEscrita) { 
            mmu->c2[idx].palavras[offset] = valorEscrita; 
            mmu->c2[idx].atualizado = true; }
        return mmu->c2[idx].palavras[offset];
    }
    mmu->missesC2++;
    
    // C3
    idx = buscarNaCache(mmu->c3, mmu->tamC3, endBloco);
    if(idx != -1) {//cache hit na c3
        mmu->hitsC3++; mmu->custoTotal += 100;
        mmu->c3[idx].contadorUso = mmu->relogioLRU;

        // Sobe o bloco da C3 replicando-o tanto na C2 quanto na C1
        levarParaCache(mmu->c2, mmu->tamC2, &mmu->c3[idx], ram->blocos, mmu->relogioLRU);
        levarParaCache(mmu->c1, mmu->tamC1, &mmu->c3[idx], ram->blocos, mmu->relogioLRU);
        
        if(isEscrita) { 
            mmu->c3[idx].palavras[offset] = valorEscrita; 
            mmu->c3[idx].atualizado = true; }
        return mmu->c3[idx].palavras[offset];
    }
    mmu->missesC3++;
    
    // RAM
    mmu->acessosRAM++; mmu->custoTotal += 1000;
    BlocoMemoria* blocoRAM = &ram->blocos[endBloco];
    if(isEscrita) { blocoRAM->palavras[offset] = valorEscrita; blocoRAM->atualizado = true; }
    

    //traz o bloco descoberto da RAM e preenche em cascata 
    levarParaCache(mmu->c3, mmu->tamC3, blocoRAM, ram->blocos, mmu->relogioLRU);
    levarParaCache(mmu->c2, mmu->tamC2, blocoRAM, ram->blocos, mmu->relogioLRU);
    levarParaCache(mmu->c1, mmu->tamC1, blocoRAM, ram->blocos, mmu->relogioLRU);
    
    return blocoRAM->palavras[offset];//devolve o dado para os registradores da CPU
}

void imprimirEstatisticas(MMU* mmu) {
    // Soma quantos acessos passaram por cada nivel para servir de divisor 
    int t1 = mmu->hitsC1 + mmu->missesC1;
    int t2 = mmu->hitsC2 + mmu->missesC2;
    int t3 = mmu->hitsC3 + mmu->missesC3;
    
    double porcC1 = 0.0;
    double porcC2 = 0.0;
    double porcC3 = 0.0;
    
    if (t1 > 0) {
        porcC1 = ((double)mmu->hitsC1 / t1) * 100;
    }
    
    if (t2 > 0) {
        porcC2 = ((double)mmu->hitsC2 / t2) * 100;
    }
    
    if (t3 > 0) {
        porcC3 = ((double)mmu->hitsC3 / t3) * 100;
    }
    
    printf("| C1 Hit: %6.2f%% | C2 Hit: %6.2f%% | C3 Hit: %6.2f%% | RAM Acc: %4d | Custo: %7d |\n",
           porcC1, porcC2, porcC3, mmu->acessosRAM, mmu->custoTotal);
}