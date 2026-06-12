#include "../include/ram.h"
#include <stdio.h>
#include <stdlib.h>

void criarRAMVazia(RAM *ram, int tamanho) {
    ram->tamanhoEmBlocos = tamanho / 4; 
    ram->blocos = (BlocoMemoria*) malloc(sizeof(BlocoMemoria) * ram->tamanhoEmBlocos);
    
    for(int i = 0; i < ram->tamanhoEmBlocos; i++) {
        ram->blocos[i].endBloco = i;
        ram->blocos[i].atualizado = false;
        ram->blocos[i].contadorUso = 0;
        for(int j = 0; j < 4; j++) {
            ram->blocos[i].palavras[j] = 0;
        }
    }
}

void mostrarRAM(RAM *ram) {
    printf("\n===== RAM (Por Blocos de 4 Palavras) =====\n");
    for(int i = 0; i < ram->tamanhoEmBlocos; i++) {
        printf("Bloco %d -> [%d, %d, %d, %d]\n", i, 
               ram->blocos[i].palavras[0], ram->blocos[i].palavras[1], 
               ram->blocos[i].palavras[2], ram->blocos[i].palavras[3]);
    }
    printf("==========================================\n");
}

void destruirRAM(RAM *ram) {
    free(ram->blocos);
    ram->blocos = NULL;
    ram->tamanhoEmBlocos = 0;
}