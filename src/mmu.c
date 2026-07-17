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
static BlocoMemoria levarParaCache(BlocoMemoria *cache,int tamanho,BlocoMemoria *bloco,BlocoMemoria *ram,bool permitirWriteBack,unsigned long long relogio){
    BlocoMemoria vazio = {0};
    vazio.endBloco = -1;

    // Procura posição livre
    for(int i = 0; i < tamanho; i++){
        if(cache[i].endBloco == -1){
            copiarBloco(bloco, &cache[i]);
            cache[i].contadorUso = relogio;
            return vazio;
        }
    }

    // Procura o LRU
    int lru = 0;

    for(int i = 1; i < tamanho; i++){
        if(cache[i].contadorUso < cache[lru].contadorUso)
            lru = i;
    }

    BlocoMemoria expulso;
    copiarBloco(&cache[lru], &expulso);

    if(permitirWriteBack &&expulso.endBloco != -1 &&expulso.atualizado){
        copiarBloco(&expulso, &ram[expulso.endBloco]);
        ram[expulso.endBloco].atualizado = false;
    }

    copiarBloco(bloco, &cache[lru]);
    cache[lru].contadorUso = relogio;

    return expulso;
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
    
    //C1 
    idx = buscarNaCache(mmu->c1, mmu->tamC1, endBloco);
    if (idx != -1) {
        return tratarHitCache(mmu->c1, idx, offset, isEscrita, valorEscrita, &mmu->hitsC1, CUSTO_C1, mmu);
    }
    mmu->missesC1++;
    
   // C2
    idx = buscarNaCache(mmu->c2, mmu->tamC2, endBloco);

    if (idx != -1){
        int valor = tratarHitCache(mmu->c2,idx,offset,isEscrita,valorEscrita,&mmu->hitsC2,CUSTO_C2,mmu);

        // Guarda o bloco que será promovido
        BlocoMemoria bloco = mmu->c2[idx];

        // Remove da C2
        mmu->c2[idx].endBloco = -1;
        mmu->c2[idx].atualizado = false;

        // Coloca na C1 e recebe quem foi expulso
        BlocoMemoria expulsoC1 = levarParaCache(mmu->c1,mmu->tamC1,&bloco,ram->blocos,false,mmu->relogioLRU);

        // Se a C1 expulsou alguém, ele desce para a C2
        if (expulsoC1.endBloco != -1) {
            BlocoMemoria expulsoC2 = levarParaCache(mmu->c2, mmu->tamC2,&expulsoC1,ram->blocos,false,mmu->relogioLRU);
        
        // Se C2 expulsou um bloco, ele desce para C3
        if (expulsoC2.endBloco != -1){
            BlocoMemoria expulsoC3 = levarParaCache(mmu->c3,mmu->tamC3,&expulsoC2,ram->blocos,false,mmu->relogioLRU);

            // Se C3 também expulsou um bloco, escreve na RAM
            if (expulsoC3.endBloco != -1 && expulsoC3.atualizado){
                copiarBloco(&expulsoC3,&ram->blocos[expulsoC3.endBloco]);
                ram->blocos[expulsoC3.endBloco].atualizado = false;
            }
        }
    }
    return valor;
}

mmu->missesC2++;
    
    // C3
    idx = buscarNaCache(mmu->c3, mmu->tamC3, endBloco);

    if (idx != -1){
    int valor = tratarHitCache(mmu->c3,idx,offset,isEscrita,valorEscrita,&mmu->hitsC3,CUSTO_C3,mmu);

    // Guarda o bloco encontrado
    BlocoMemoria bloco = mmu->c3[idx];

    // Remove da C3 (cache exclusiva)
    mmu->c3[idx].endBloco = -1;
    mmu->c3[idx].atualizado = false;

    // Promove para C2
    BlocoMemoria expulsoC2 = levarParaCache(mmu->c2,mmu->tamC2,&bloco,ram->blocos,false,mmu->relogioLRU);

    // Se a C2 expulsou alguém, ele desce para a C3
    if (expulsoC2.endBloco != -1){
        BlocoMemoria expulsoC3 = levarParaCache(mmu->c3,mmu->tamC3,&expulsoC2,ram->blocos,false,mmu->relogioLRU);

        // Se a C3 expulsou alguém, escreve na RAM
        if (expulsoC3.endBloco != -1 && expulsoC3.atualizado){
            copiarBloco(&expulsoC3,&ram->blocos[expulsoC3.endBloco]);
            ram->blocos[expulsoC3.endBloco].atualizado = false;
        }
    }

    // Procura o bloco que acabou de subir para a C2
    int idxC2 = buscarNaCache(mmu->c2, mmu->tamC2, endBloco);

    bloco = mmu->c2[idxC2];

    // Remove da C2
    mmu->c2[idxC2].endBloco = -1;
    mmu->c2[idxC2].atualizado = false;

    // Promove para C1
    BlocoMemoria expulsoC1 = levarParaCache(mmu->c1,mmu->tamC1,&bloco,ram->blocos,false,mmu->relogioLRU);

    // Se a C1 expulsou alguém, ele desce para a C2
    if (expulsoC1.endBloco != -1){
        BlocoMemoria expulsoC2b = levarParaCache(mmu->c2,mmu->tamC2,&expulsoC1,ram->blocos,false,mmu->relogioLRU);

        // Se a C2 expulsou alguém, ele desce para a C3
        if (expulsoC2b.endBloco != -1)
        {
            BlocoMemoria expulsoC3b = levarParaCache(mmu->c3,mmu->tamC3,&expulsoC2b,ram->blocos,false,mmu->relogioLRU);

            // Se a C3 expulsou alguém, escreve na RAM
            if (expulsoC3b.endBloco != -1 && expulsoC3b.atualizado){
                copiarBloco(&expulsoC3b,&ram->blocos[expulsoC3b.endBloco]);
                ram->blocos[expulsoC3b.endBloco].atualizado = false;
            }
        }
    }

    return valor;
}

mmu->missesC3++;

// RAM
mmu->acessosRAM++;
mmu->custoTotal += CUSTO_RAM;

BlocoMemoria *blocoRAM = &ram->blocos[endBloco];
    if (isEscrita){
        blocoRAM->palavras[offset] = valorEscrita;
        blocoRAM->atualizado = true;
    }

    // RAM -> C3
    levarParaCache(mmu->c3,mmu->tamC3,blocoRAM,ram->blocos,true,mmu->relogioLRU);

    // O bloco recém inserido está na C3
    int idxC3 = buscarNaCache(mmu->c3, mmu->tamC3, endBloco);

    BlocoMemoria bloco = mmu->c3[idxC3];

    // Remove da C3
    mmu->c3[idxC3].endBloco = -1;
    mmu->c3[idxC3].atualizado = false;

  // C3 -> C2
    BlocoMemoria expulsoC2 = levarParaCache(mmu->c2,mmu->tamC2,&bloco,ram->blocos,false,mmu->relogioLRU);

    // O expulso da C2 desce para a C3
    if (expulsoC2.endBloco != -1) {
        BlocoMemoria expulsoC3 = levarParaCache(mmu->c3,mmu->tamC3,&expulsoC2,ram->blocos,false,mmu->relogioLRU);

        if (expulsoC3.endBloco != -1 && expulsoC3.atualizado){
            copiarBloco(&expulsoC3, &ram->blocos[expulsoC3.endBloco]);
            ram->blocos[expulsoC3.endBloco].atualizado = false;
        }
    }
    // Recupera o bloco que subiu para C2
    int idxC2 = buscarNaCache(mmu->c2, mmu->tamC2, endBloco);

    bloco = mmu->c2[idxC2];

    // Remove da C2
    mmu->c2[idxC2].endBloco = -1;
    mmu->c2[idxC2].atualizado = false;

    // C2 -> C1
    BlocoMemoria expulsoC1 = levarParaCache(mmu->c1,mmu->tamC1,&bloco,ram->blocos,false,mmu->relogioLRU);


    // O expulso da C1 desce para a C2
    if (expulsoC1.endBloco != -1){
        BlocoMemoria expulsoC2 = levarParaCache(mmu->c2,mmu->tamC2,&expulsoC1,ram->blocos,false,mmu->relogioLRU);

        if (expulsoC2.endBloco != -1){
            BlocoMemoria expulsoC3 = levarParaCache(mmu->c3,mmu->tamC3,&expulsoC2,ram->blocos,false,mmu->relogioLRU);

            if (expulsoC3.endBloco != -1 && expulsoC3.atualizado){
                copiarBloco(&expulsoC3, &ram->blocos[expulsoC3.endBloco]);
                ram->blocos[expulsoC3.endBloco].atualizado = false;
            }
        }
    }
    return blocoRAM->palavras[offset];
}
/*static void verificarExclusividade(MMU *mmu)
{
    // C1 x C2
    for (int i = 0; i < mmu->tamC1; i++)
    {
        if (mmu->c1[i].endBloco == -1)
            continue;

        for (int j = 0; j < mmu->tamC2; j++)
        {
            if (mmu->c1[i].endBloco == mmu->c2[j].endBloco)
            {
                printf("ERRO: bloco %d está em C1 e C2\n",
                       mmu->c1[i].endBloco);
            }
        }

        for (int j = 0; j < mmu->tamC3; j++)
        {
            if (mmu->c1[i].endBloco == mmu->c3[j].endBloco)
            {
                printf("ERRO: bloco %d está em C1 e C3\n",
                       mmu->c1[i].endBloco);
            }
        }
    }

    // C2 x C3
    for (int i = 0; i < mmu->tamC2; i++)
    {
        if (mmu->c2[i].endBloco == -1)
            continue;

        for (int j = 0; j < mmu->tamC3; j++)
        {
            if (mmu->c2[i].endBloco == mmu->c3[j].endBloco)
            {
                printf("ERRO: bloco %d está em C2 e C3\n",
                       mmu->c2[i].endBloco);
            }
        }
    }
}
*/
void imprimirEstatisticas(MMU* mmu, int maquina,double tempoExecucao) {
    //verificarExclusividade(mmu);
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
    printf(" C1  C2  C3 |  Hit C1 | Miss C1  |  Hit C2 |  Miss C2 |  Hit C3 |  Miss C3 | RAM Acc |  Custo\n");
    printf("-----------------------------------------------------------------------------------------------\n");

    printf("%3d %3d %3d | %6.2f%% | %7.2f%% | %6.2f%% | %7.2f%% | %6.2f%% | %7.2f%% |   %4d   | %7d\n",
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

    printf("\nResumo de Hits e Misses\n");
    printf("-------------------------------------------------------------------------------\n");
    printf("%-10s %8s %8s %8s %12s %12s\n",
       "Memoria", "Hits", "Misses", "Total", "Hit (%)", "Miss (%)");
    printf("-------------------------------------------------------------------------------\n");

    printf("%-10s %8d %8d %8d %11.2f%% %11.2f%%\n",
       "Cache C1",
       mmu->hitsC1,
       mmu->missesC1,
       t1,
       porcC1,
       missC1);

    printf("%-10s %8d %8d %8d %11.2f%% %11.2f%%\n",
       "Cache C2",
       mmu->hitsC2,
       mmu->missesC2,
       t2,
       porcC2,
       missC2);

    printf("%-10s %8d %8d %8d %11.2f%% %11.2f%%\n",
       "Cache C3",
       mmu->hitsC3,
       mmu->missesC3,
       t3,
       porcC3,
       missC3);

    printf("-------------------------------------------------------------------------------\n");

    printf("\n[RESUMO]\n");
    printf("Acessos a RAM : %d\n", mmu->acessosRAM);
    printf("Custo Total   : %d\n", mmu->custoTotal);
    printf("Tempo de Execucao  : %.6f segundos\n", tempoExecucao);

    printf("===============================================================================\n");
}