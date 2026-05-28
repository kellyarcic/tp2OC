# Sistema de Memória Cache em 3 Níveis - TP 2

[cite_start]Este repositório contém o segundo Trabalho Prático da disciplina **BCC266 - Arquitetura de Computadores I**, do curso de Ciência da Computação da **Universidade Federal de Ouro Preto (UFOP)**[cite: 1, 2].

## 🎯 Objetivo

[cite_start]O projeto consiste na extensão do simulador de CPU desenvolvido no TP1, adicionando uma hierarquia de memória cache em 3 níveis (C1, C2 e C3) intermediando o fluxo de dados entre o processador e a Memória RAM[cite: 3]. [cite_start]Todo o acesso ao sistema de memória é centralizado e gerenciado de forma transparente por uma **UCM (Unidade de Controle de Memória)**[cite: 73].

## 🛠️ Especificações do Hardware

* [cite_start]**Mapeamento de Cache:** Mapeamento Associativo Puro[cite: 28].
* [cite_start]**Política de Substituição:** LFU (*Least Frequently Used* - Menos Frequentemente Utilizado)[cite: 31].
* **Política de Escrita:** Write-Through (Atualização síncrona entre cache e memória principal).
* [cite_start]**Organização da Memória:** Memórias estruturadas em blocos/linhas contendo 4 palavras (inteiros) cada[cite: 42, 43].

## 📂 Estrutura do Projeto

O projeto adota uma estrutura modular separando cabeçalhos e códigos-fonte:

* `include/`: Arquivos de cabeçalho (`.h`) contendo as definições das estruturas da CPU, RAM, Blocos de Memória e UCM.
* [cite_start]`src/`: Implementação dos códigos-fonte (`.c`), incluindo o pipeline da máquina interpretada e a lógica de busca/substituição de blocos[cite: 72].

## 🚀 Como Compilar e Executar

Como o projeto está dividido em pastas, utilize o comando abaixo dentro do diretório raiz para compilar todos os arquivos corretamente:

```bash
gcc src/*.c -I include -o tp2
./tp2
