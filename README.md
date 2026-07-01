# Simulador de Hierarquia de Memória Cache e RAM (TP2)

Este projeto consiste em um simulador de arquitetura de computadores focado no gerenciamento da hierarquia de memória. O sistema simula o comportamento de uma CPU integrada a uma Unidade de Gerenciamento de Memória (MMU) operando em cascata através de três níveis de memória cache ($C1$, $C2$, $C3$) e uma Memória RAM física principal.

## 🚀 Funcionalidades e Características

* **Hierarquia de Memória em Cascata**: Busca de dados sequencial e dinâmica ($C1 \rightarrow C2 \rightarrow C3 \rightarrow \text{RAM}$).
* **Algoritmo de Substituição LRU (Least Recently Used)**: Substituição baseada no tempo de uso recente controlado por um relógio lógico interno para determinar qual bloco deve ser mapeado para as camadas superiores.
* **Mapeamento Totalmente Associativo**: Flexibilidade total no posicionamento de blocos de palavras dentro das linhas de cache, maximizando o aproveitamento do espaço.

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

| Configuração | Hit C1 | Hit C2 | Hit C3 | Acessos RAM (RAM Acc) | Custo Total (Ciclos) |
| :---: | :---: | :---: | :---: | :---: | :---: |
| **M1** | 0.90% | 76.97% | 30.78% | 3.420 | 3.737.474 |
| **M2** | 86.71% | 19.01% | 34.59% | 1.524 | 1.628.846 |
| **M3** | 77.26% | 49.72% | 89.90% | 250 | **513.809** |
| **M4** | 0.90% | 86.59% | 47.27% | 1.517 | 1.839.014 |
| **M5** | 77.26% | 30.63% | 21.69% | 2.675 | 2.780.909 |

> 💡 **Análise dos Resultados**: 
> * A **Máquina M3** consagrou-se como a mais eficiente do pipeline, atingindo uma espetacular taxa de **89.90% de acertos na Cache C3**, o que minimizou os acessos à memória RAM principal para apenas 250 repetições e derrubou o custo operacional para apenas 513.809 ciclos.
> * A **Máquina M2** obteve o maior índice de filtragem imediata no primeiro nível ($C1$), absorvendo **86.71%** dos pedidos de dados direto na camada de maior velocidade de barramento.

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
