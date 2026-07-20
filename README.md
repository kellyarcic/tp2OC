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

| Máquina | C1 | C2 |  C3 |     Hit C1 | Miss C1 |     Hit C2 | Miss C2 |     Hit C3 | Miss C3 | RAM Acc |      Custo |
| ------: | -: | -: | --: | ---------: | ------: | ---------: | ------: | ---------: | ------: | ------: | ---------: |
|      M1 |  8 | 16 |  32 |      0.90% |  99.10% |     85.70% |  14.30% |     16.78% |  83.22% |    2554 |    2.789.594 |
|      M2 | 32 | 64 | 128 |     86.71% |  13.29% |     28.92% |  71.08% |     76.77% |  23.23% |     475 |     659.096 |
|      M3 | 16 | 64 | 256 | **77.26%** |  22.74% | **53.86%** |  46.14% | **89.00%** |  11.00% | **250** | **495.449** |
|      M4 |  8 | 32 | 128 |      0.90% |  99.10% |     87.00% |  13.00% |     57.60% |  42.40% |    1183 |    1.530.584 |
|      M5 | 16 | 32 |  64 |     77.26% |  22.74% |     45.78% |  54.22% |     30.94% |  69.06% |    1844 |    1.965.869 |


## 📈 Análise dos Resultados

Os experimentos mostram que o desempenho do simulador depende diretamente da configuração das memórias cache. Configurações com maior capacidade nos níveis superiores reduzem a quantidade de acessos à memória principal, diminuindo o custo total de execução.

- Máquina M3: apresentou o melhor desempenho geral, obtendo o menor custo total (495.449) e apenas 250 acessos à RAM, resultado da elevada taxa de acertos nas caches, principalmente na C3.
- Máquina M2: apresentou a maior taxa de acertos na C1 (86,71%), reduzindo significativamente o tempo médio de acesso aos dados e alcançando o segundo melhor desempenho geral.
- Máquina M4: apresentou desempenho intermediário. Apesar da baixa taxa de acertos na C1, obteve excelentes taxas de acerto em C2 e C3, reduzindo os acessos à RAM em relação à M1.
- Máquina M5: com a política de cache exclusiva, apresentou melhora nas taxas de acerto da C2 e da C3 e redução dos acessos à RAM em comparação com a implementação inclusiva, diminuindo também o custo total de execução.
- Máquina M1: apresentou o pior desempenho, pois possui as menores capacidades de cache (C1 = 8, C2 = 16 e C3 = 32), ocasionando maior quantidade de faltas (misses), 2.554 acessos à RAM e o maior custo entre as configurações avaliadas.
-------
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

gcc instructionsGenerator.c -o gerador
./gerador
