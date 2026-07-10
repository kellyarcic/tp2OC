# Simulador de Hierarquia de Memória Cache e RAM (TP2)

Este projeto consiste em um simulador de arquitetura de computadores focado no gerenciamento da hierarquia de memória. O sistema simula o comportamento de uma CPU integrada a uma Unidade de Gerenciamento de Memória (MMU) operando em cascata através de três níveis de memória cache ($C1$, $C2$, $C3$) e uma Memória RAM física principal.

## 🚀 Funcionalidades

- Busca em cascata entre C1 → C2 → C3 → RAM.
- Política de substituição LRU (Least Recently Used).
- Política de escrita Write-Back.
- Mapeamento totalmente associativo.
- Geração automática de programas de teste através do gerador de instruções.
- Estatísticas de execução contendo:
  - Hit e Miss das caches;
  - Número de acessos à RAM;
  - Custo total da execução.

---

## 🛠️ Arquitetura do Sistema

O simulador testa o comportamento do ecossistema de memória sob diferentes configurações de hardware (Máquinas $M1$ a $M5$), variando o tamanho de suas respectivas caches:

| Máquina | Tamanho C1 (Blocos) | Tamanho C2 (Blocos) | Tamanho C3 (Blocos) |
| :---: | :---: | :---: | :---: |
| **M1** | 8 | 16 | 32 |
| **M2** | 32 | 64 | 128 |
| **M3** | 16 | 64 | 256 |
| **M4** | 8 | 32 | 128 |
| **M5** | 16 | 32 | 64 |

---

## 📊 Relatório Comparativo de Desempenho

Abaixo estão consolidadas as métricas obtidas na execução do programa de testes em todas as configurações de hardware disponíveis para o ambiente sem armazenamento em disco (HD).

## 📊 Resultados Obtidos

| Máquina | C1 | C2 | C3 | Hit C1 | Miss C1 | Hit C2 | Miss C2 | Hit C3 | Miss C3 | RAM Acc | Custo Total |
|:------:|:--:|:--:|:--:|-------:|--------:|-------:|--------:|-------:|--------:|--------:|------------:|
| **M1** | 8 | 16 | 32 | 0.90% | 99.10% | 76.97% | 23.03% | 30.78% | 69.22% | 3.420 | 3.737.474 |
| **M2** | 32 | 64 | 128 | 86.71% | 13.29% | 19.01% | 80.99% | 34.59% | 65.41% | 1.524 | 1.628.846 |
| **M3** | 16 | 64 | 256 | 77.26% | 22.74% | 49.72% | 50.28% | 89.90% | 10.10% | 250 | **513.809** |
| **M4** | 8 | 32 | 128 | 0.90% | 99.10% | 86.59% | 13.41% | 47.27% | 52.73% | 1.517 | 1.839.014 |
| **M5** | 16 | 32 | 64 | 77.26% | 22.74% | 30.63% | 69.37% | 21.69% | 78.31% | 2.675 | 2.780.909 |

## 📈 Análise dos Resultados

Os testes mostraram diferenças significativas entre as configurações de cache.

- A Máquina M3 apresentou o menor custo total de execução, consequência da alta taxa de acertos nas caches e da redução dos acessos à memória RAM.

- A Máquina M2 apresentou a maior taxa de acertos na cache C1, reduzindo significativamente o tempo médio de acesso aos dados.

- **Máquina M1:** apresentou o **pior desempenho**, pois possui as menores caches (**C1 = 8, C2 = 16 e C3 = 32**), resultando em **mais Misses**, **3.420 acessos à RAM** e o **maior custo total (3.737.474)**.
---
## 🧠 Políticas Implementadas

### LRU (Least Recently Used)

Quando uma cache está cheia e um novo bloco precisa ser inserido, o algoritmo LRU seleciona o bloco menos recentemente utilizado para substituição.

### Write-Back

Caso o bloco escolhido pelo LRU tenha sido modificado, ele é gravado novamente na memória RAM antes da substituição, evitando perda de dados.

---
## 📁 Estrutura do Projeto

```text
├── include/               # Arquivos de cabeçalho (.h)
│   ├── bloco.h
│   ├── cpu.h
│   ├── instrucao.h
│   ├── mmu.h
│   └── ram.h
├── src/                   # Implementação dos códigos-fonte (.c)
│   ├── cpu.c
│   ├── main.c
│   ├── mmu.c
│   └── ram.c
├── instructionsGenerator.c # Código do gerador de instruções oficial
├── instructions2.txt      # Arquivo de texto com as instruções geradas
└── README.md              # Documentação do projeto

### 📤 Como subir as atualizações pro GitHub

Depois de salvar as alterações nesses arquivos, roda essa sequência clássica no seu terminal para deixar o Git atualizadinho:

```bash
git add .
git commit -m "Ajuste: Altera politicas de cache para LRU e Write-Back e atualiza README"
git push origin main

## 🚀 Como Compilar e Executar

gcc src/main.c src/mmu.c src/ram.c src/cpu.c -I include -o tp2
./tp2
