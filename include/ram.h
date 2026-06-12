
#ifndef RAM_H
#define RAM_H

#include "bloco.h"

// Estrutura atualizada da RAM para conter blocos em vez de inteiros simples
typedef struct {
    BlocoMemoria *blocos;
    int tamanhoEmBlocos;
} RAM;

void criarRAMVazia(RAM *ram, int tamanho);
void mostrarRAM(RAM *ram);
void destruirRAM(RAM *ram);

#endif