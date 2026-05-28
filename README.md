# Trabalho Prático 2 da disciplina BCC266.

## 🎯 Objetivo

O projeto consiste na extensão do simulador de CPU desenvolvido no TP1, adicionando uma hierarquia de memória cache em 3 níveis (C1, C2 e C3) intermediando o fluxo de dados entre o processador e a Memória RAM.Todo o acesso ao sistema de memória é centralizado e gerenciado de forma transparente por uma **UCM (Unidade de Controle de Memória)**.

## 🛠️ Especificações do Hardware

* **Mapeamento de Cache:** Mapeamento Associativo Puro.
* **Política de Substituição:** LFU (*Least Frequently Used* - Menos Frequentemente Utilizado).
* **Política de Escrita:** Write-Through (Atualização síncrona entre cache e memória principal).
* **Organização da Memória:** Memórias estruturadas em blocos/linhas contendo 4 palavras (inteiros) cada.

## 📂 Estrutura do Projeto

O projeto adota uma estrutura modular separando cabeçalhos e códigos-fonte:

* `include/`: Arquivos de cabeçalho (`.h`) contendo as definições das estruturas da CPU, RAM, Blocos de Memória e UCM.
* `src/`: Implementação dos códigos-fonte (`.c`), incluindo o pipeline da máquina interpretada e a lógica de busca/substituição de blocos.

## 🚀 Como Compilar e Executar

Como o projeto está dividido em pastas, utilize o comando abaixo dentro do diretório raiz para compilar todos os arquivos corretamente:

```bash
gcc src/*.c -I include -o tp2
./tp2
