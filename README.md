# Simulador de CPU com Hierarquia de Memória Cache (TP2)

Este projeto foi desenvolvido como o Trabalho Prático 2 da disciplina **BCC266 - Organização de Computadores** da Universidade Federal de Ouro Preto (UFOP). O objetivo é estender a CPU desenvolvida no TP1, adicionando uma hierarquia de memória cache gerenciada por uma Unidade de Controle de Memória (UCM/MMU).

## 🎯 Objetivo
O simulador intercepta as instruções geradas pelo pipeline da CPU e gerencia o fluxo de dados através de 3 níveis de memória cache (C1, C2 e C3) antes de acessar a Memória RAM, visando analisar o impacto do custo de acesso e as taxas de acerto (*hits*).

## 🛠️ Especificações do Hardware
* **Mapeamento de Cache:** Mapeamento Associativo Puro.
* **Política de Substituição:** LRU (*Least Recently Used* - Menos Recentemente Utilizado).
* **Política de Escrita:** Write-Back (Escrita postergada com uso de bit de modificação/Dirty Bit).
* **Organização da Memória:** Estruturada em blocos/linhas contendo 4 palavras (inteiros) cada.

## 📂 Estrutura do Projeto
```text
├── include/       # Arquivos de cabeçalho (.h)
│   ├── bloco.h
│   ├── cpu.h
│   ├── instrucao.h
│   ├── mmu.h
│   └── ram.h
├── src/           # Implementação dos códigos-fonte (.c)
│   ├── cpu.c
│   ├── main.c
│   ├── mmu.c
│   └── ram.c
└── README.md      # Documentação do projeto

### 📤 Como subir as atualizações pro GitHub

Depois de salvar as alterações nesses arquivos, roda essa sequência clássica no seu terminal para deixar o Git atualizadinho:

```bash
git add .
git commit -m "Ajuste: Altera politicas de cache para LRU e Write-Back e atualiza README"
git push origin main

## 🚀 Como Compilar e Executar

gcc src/*.c -I include -o tp2
./tp2
