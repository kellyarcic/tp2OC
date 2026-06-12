
#ifndef BLOCO_H
#define BLOCO_H

#include <stdbool.h>

// Estrutura do Bloco de Memória / Linha de Cache [cite: 13, 23, 42, 65]
typedef struct {
    int palavras[4];                // Cada bloco ou linha possui 4 palavras [cite: 43]
    int endBloco;                   // Identificador do endereço do bloco [cite: 68]
    bool atualizado;                // Bit de modificação (Dirty Bit) para Write-Back [cite: 32, 69]
    unsigned long long contadorUso; // Contador para a política de substituição LRU [cite: 31]
} BlocoMemoria;

#endif