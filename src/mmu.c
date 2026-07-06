#include "../include/mmu.h"
#include "../include/config.h"
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
static void levarParaCache(BlocoMemoria* cache, int tamanho, BlocoMemoria* blocoSubindo, BlocoMemoria* ram, unsigned long long relogio, const char *nomeCache) {
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
    printf("[LRU] %s cheia! Removendo bloco %d.\n",nomeCache, cache[lruIdx].endBloco);

    //WRITE-BACK
    //teste antes de jogar no lixo
    if(cache[lruIdx].atualizado && cache[lruIdx].endBloco != -1) {
        copiarBloco(&cache[lruIdx], &ram[cache[lruIdx].endBloco]); //salva de volta na RAM antes de joga-la fora
        ram[cache[lruIdx].endBloco].atualizado = false;
    }

    printf("[WRITE-BACK] Gravando bloco %d da %s na RAM.\n",cache[lruIdx].endBloco, nomeCache);

    copiarBloco(blocoSubindo, &cache[lruIdx]);
    cache[lruIdx].contadorUso = relogio;
}

static int tratarHitCache(BlocoMemoria *cache,int idx,int offset,bool isEscrita,int valorEscrita,int *hits,int custo,MMU *mmu) {
    (*hits)++;
    mmu->custoTotal += custo;
    cache[idx].contadorUso = mmu->relogioLRU;

    if (isEscrita) {
        cache[idx].palavras[offset] = valorEscrita;
        cache[idx].atualizado = true;
    }

    return cache[idx].palavras[offset];
}

//busca em cascatas
int buscarNasMemorias(MMU* mmu, int enderecoGlobal, bool isEscrita, int valorEscrita, RAM* ram) {
    int endBloco = enderecoGlobal / 4;
    int offset = enderecoGlobal % 4;
    int idx;

    mmu->relogioLRU++;
    
    // C1
    idx = buscarNaCache(mmu->c1, mmu->tamC1, endBloco);
    if (idx != -1) {
        return tratarHitCache(mmu->c1,idx,offset,isEscrita,valorEscrita,&mmu->hitsC1,CUSTO_C1,mmu
        );
    }

    mmu->missesC1++;
    
    // C2
    idx = buscarNaCache(mmu->c2, mmu->tamC2, endBloco);
    if (idx != -1) {
        int valor = tratarHitCache(mmu->c2,idx,offset,isEscrita,valorEscrita,&mmu->hitsC2,CUSTO_C2,mmu);

        // Sobe o bloco encontrado da C2 para a C1 (aplicando LRU caso a C1 esteja cheia)
        levarParaCache(mmu->c1, mmu->tamC1, &mmu->c2[idx], ram->blocos, mmu->relogioLRU, "C1");
        return valor;
    }
    mmu->missesC2++;
    
    // C3
    idx = buscarNaCache(mmu->c3, mmu->tamC3, endBloco);
    if(idx != -1) {//cache hit na c3
        int valor = tratarHitCache(mmu->c3,idx,offset,isEscrita,valorEscrita,&mmu->hitsC3,CUSTO_C3,mmu);

        // Sobe o bloco da C3 replicando-o tanto na C2 quanto na C1
        levarParaCache(mmu->c2, mmu->tamC2, &mmu->c3[idx], ram->blocos, mmu->relogioLRU,"C2");
        levarParaCache(mmu->c1, mmu->tamC1, &mmu->c3[idx], ram->blocos, mmu->relogioLRU,"C1");
        
        return valor;
    }
    mmu->missesC3++;
    
    // RAM
    mmu->acessosRAM++; mmu->custoTotal += CUSTO_RAM;
    BlocoMemoria* blocoRAM = &ram->blocos[endBloco];
    if(isEscrita) { blocoRAM->palavras[offset] = valorEscrita; blocoRAM->atualizado = true; }
    

    //traz o bloco descoberto da RAM e preenche em cascata 
    levarParaCache(mmu->c3, mmu->tamC3, blocoRAM, ram->blocos, mmu->relogioLRU,"C3");
    levarParaCache(mmu->c2, mmu->tamC2, blocoRAM, ram->blocos, mmu->relogioLRU,"C2");
    levarParaCache(mmu->c1, mmu->tamC1, blocoRAM, ram->blocos, mmu->relogioLRU,"C1");
    
    return blocoRAM->palavras[offset];//devolve o dado para os registradores da CPU
}

void imprimirEstatisticas(MMU* mmu, int maquina) {
    // Soma quantos acessos passaram por cada nivel para servir de divisor 
    int t1 = mmu->hitsC1 + mmu->missesC1;
    int t2 = mmu->hitsC2 + mmu->missesC2;
    int t3 = mmu->hitsC3 + mmu->missesC3;
    
    double porcC1 = 0.0;
    double porcC2 = 0.0;
    double porcC3 = 0.0;

    double missC1 = 0.0;
    double missC2 = 0.0;
    double missC3 = 0.0;
    
    if (t1 > 0) {
        porcC1 = ((double)mmu->hitsC1 / t1) * 100;
    }
    
    if (t2 > 0) {
        porcC2 = ((double)mmu->hitsC2 / t2) * 100;
    }
    
    if (t3 > 0) {
        porcC3 = ((double)mmu->hitsC3 / t3) * 100;
    }
    
    if (t1 > 0) {
    missC1 = ((double)mmu->missesC1 / t1) * 100;
    }

    if (t2 > 0) {
        missC2 = ((double)mmu->missesC2 / t2) * 100;
    }

    if (t3 > 0) {
        missC3 = ((double)mmu->missesC3 / t3) * 100;
    }
    printf("\n==================================== Máquina M%d ====================================\n", maquina);
    printf(" C1  C2  C3 | Hit C1 | Miss C1 | Hit C2 | Miss C2 | Hit C3 | Miss C3 | RAM Acc | Custo\n");
    printf("-----------------------------------------------------------------------------------------------\n");

    printf("%3d %3d %3d | %6.2f%% | %7.2f%% | %6.2f%% | %7.2f%% | %6.2f%% | %7.2f%% | %4d | %7d\n",
        mmu->tamC1,
        mmu->tamC2,
        mmu->tamC3,
        porcC1,
        missC1,
        porcC2,
        missC2,
        porcC3,
        missC3,
        mmu->acessosRAM,
        mmu->custoTotal);

    printf("===============================================================================================\n");
}