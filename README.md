# TP2 - Organização de Computadores

Trabalho Prático 2 da disciplina BCC266.

## Objetivo
Implementar um sistema de memória cache em 3 níveis:

- Cache L1
- Cache L2
- Cache L3
- Memória RAM

## Funcionalidades
- Mapeamento associativo
- Política de substituição LRU ou LFU
- Cache hit e cache miss
- Estatísticas de execução
- Unidade de Controle de Memória (UCM)

## Estrutura
O projeto é baseado no TP1 e adiciona o sistema de caches ao acesso de memória.

## Compilação

```bash
gcc *.c -o tp2
./tp2
