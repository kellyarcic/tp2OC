#ifndef CONFIG_H
#define CONFIG_H

#define DEBUG_CPU 0

//config do simulador e do gerador
#define QTD_INSTRUCOES        10000 
#define ARQUIVO_INSTRUCOES    "instructions2.txt"
#define TAMANHO_RAM           1024

//custos  de acesso
#define CUSTO_C1              1
#define CUSTO_C2              10
#define CUSTO_C3              100
#define CUSTO_RAM             1000

//config das maquinas(c1,c2,c3)
#define M1_C1 8
#define M1_C2 16
#define M1_C3 32

#define M2_C1 32
#define M2_C2 64
#define M2_C3 128

#define M3_C1 16
#define M3_C2 64
#define M3_C3 256

#define M4_C1 8
#define M4_C2 32
#define M4_C3 128

#define M5_C1 64
#define M5_C2 128
#define M5_C3 256

#endif