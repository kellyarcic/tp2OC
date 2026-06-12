
#ifndef BLOCO_H
#define BLOCO_H

#include <stdbool.h>

// Estrutura do Bloco de Memória / Linha de Cache
typedef struct {
    int palavras[4];            
    int endBloco;                   
    bool atualizado;               
    unsigned long long contadorUso; 
} BlocoMemoria;

#endif