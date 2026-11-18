# Carteira Digital (banco) em C

Trabalho de **Algoritmos e Pensamento Computacional** (UNIPÊ). Um programa em C, no terminal, que simula um banco: cadastra clientes, cada um com até 2 contas (corrente, salário ou poupança), controla saldo e empréstimo e, no final, mostra quem pode fazer empréstimo.

O projeto está sendo construído **passo a passo**. Este arquivo mostra o que já funciona e o que ainda falta.

## Estrutura dos arquivos

| Arquivo | Papel |
| --- | --- |
| `meu_banco.h` | A "biblioteca": constantes, `typedef struct` (`Conta`, `Cliente`, `Banco`) e os protótipos das funções. Tem *include guard* (`#ifndef MEU_BANCO_H`). |
| `banco.c` | As funções do banco (o corpo de cada uma). |
| `main.c` | A "cara" do programa: o menu e o laço principal. |

Regra de ouro: o `.h` **declara** (tipos, constantes, protótipos); o `.c` **faz** (corpo das funções).

## Como compilar e rodar

Dentro da pasta do projeto (só os arquivos `.c` vão no comando):

```
gcc -Wall -Wextra main.c banco.c -o banco
.\banco.exe        (PowerShell)
./banco            (Linux / Mac)
```

`-Wall -Wextra` liga os avisos do compilador. O projeto atual compila **sem nenhum aviso**.

## Modelo de dados (`meu_banco.h`)

- **Tipos de conta** (constantes com nome): `CORRENTE` = 1, `SALARIO` = 2, `POUPANCA` = 3.
- **Limites:** `MAX_CONTAS` = 2 por cliente, `MAX_CLIENTES` = 50, `MAX_HISTORICO` = 20 linhas de `TAM_TEXTO` = 80 caracteres.
- **`Conta`:** `numero`, `tipo`, `saldo` (float).
- **`Cliente`:** `nome` (50), `contas[MAX_CONTAS]`, `qtdContas`, `emprestimo` (0 = ainda não pegou, 1 = já pegou), `historico`, `qtdHistorico`.
- **`Banco`:** `clientes[MAX_CLIENTES]`, `qtdClientes`, `aberto`.

## Regras do enunciado (como entendi até agora)

- O cliente tem **até 2 contas**, e **não pode repetir o tipo**.
- Combinações permitidas: corrente + poupança, salário + poupança, ou uma conta só. **Corrente + salário não pode.**
- Cadastrar 3 clientes e, na saída, mostrar os clientes que podem fazer empréstimo.
- Empréstimo (do quadro, **a confirmar com a professora**): exige conta corrente + poupança com saldo, só um empréstimo por cliente, limite de 25% de 10 vezes o saldo.
- Saídas finais (do quadro, **a confirmar**): saldo total, clientes aptos a empréstimo, clientes com saldo negativo e uma lista de contas salário com saldo entre 1000 e 1500.

## O que já está feito

- [x] Divisão em três arquivos, com include guard e constantes com nome.
- [x] Structs `Conta`, `Cliente` e `Banco` com `typedef`.
- [x] Menu em laço (`do ... while` + `switch`) com as opções `1` (abrir banco), `2` (ver histórico) e `s` (sair). A opção 2 só responde se o banco estiver aberto (`aberto` por enquanto é uma variável do `main`).
- [x] `mostrarMenu()`: mostra o menu.
- [x] `lerCliente()`: lê o nome (`" %49[^\n]"`) e devolve o cliente com `qtdContas`, `emprestimo` e `qtdHistorico` zerados.
- [x] `combinacaoValida(tipo1, tipo2)`: devolve 1 se dois tipos podem conviver no mesmo cliente e 0 se não. Testada.
- [x] `podeAdicionarConta(cliente, tipoNovo)`: devolve 1 se o cliente ainda cabe uma conta desse tipo (máximo de contas, tipo repetido, corrente + salário). Testada com `1 0 1 0 0`.

## O que falta fazer

Na ordem em que vamos fazer:

- [ ] **`adicionarConta(cliente, numero, tipo, saldoInicial)`**: coloca a conta na primeira vaga livre (`contas[qtdContas]`), soma 1 em `qtdContas` e devolve o cliente atualizado. (Próximo passo.)
- [ ] **Cadastro de 3 clientes:** ler o nome, o tipo e o saldo de cada conta, usando `podeAdicionarConta` antes de adicionar. Validar o tipo (1 a 3).
- [ ] **Guardar os clientes no `Banco`** (`clientes[]` e `qtdClientes`), dentro do menu do `main`. Aqui entra o ponteiro para struct (`Banco *`), quando a professora passar o conteúdo.
- [ ] **Regra de empréstimo:** função que diz se o cliente pode pegar empréstimo e qual o limite.
- [ ] **Saídas finais:** saldo total, clientes aptos, clientes com saldo negativo e a lista de contas salário entre 1000 e 1500.
- [ ] **Histórico:** registrar as operações em `historico` e mostrar na opção 2 do menu.
- [ ] **Testes:** rodar um roteiro com os 3 clientes cobrindo cada regra.
- [ ] **Limpeza:** escrever `(void)` nas funções sem parâmetros em `banco.c` (`mostrarMenu(void)` e `lerCliente(void)`), para casar com os protótipos; aceitar `S` maiúsculo no menu; não versionar o `banco.exe` no Git.

## Dúvidas para confirmar com a professora

1. O enunciado pede `emprestimo` como `char` `'S'` ou `'N'`, mas o projeto usa `int` (0 ou 1). Qual ela quer?
2. Os tipos de conta eram `0`, `1` e `2` no enunciado e aqui estão `1`, `2` e `3`. Isso é aceito, desde que sejam constantes com nome?
3. A quantidade de clientes é **fixa em 3** ou pode variar (alocação dinâmica)? Hoje o limite é `MAX_CLIENTES` = 50.
4. O limite do empréstimo usa o saldo da **poupança** ou a **soma** das contas?
5. O histórico é **por cliente** (como está no `.h`) ou **do banco inteiro**?
6. O que exatamente conta o item "R$ valor : quantidade" das saídas, e qual é o título da quarta lista?

## Decisões que tomamos (e por quê)

- **Passagem por valor por enquanto.** As funções recebem uma cópia do cliente e devolvem o cliente atualizado (`t = adicionarConta(t, ...)`), porque ponteiro para struct ainda não foi dado em aula. Quando for, trocamos por `Cliente *`.
- **Funções pequenas que respondem 1 ou 0** (`combinacaoValida`, `podeAdicionarConta`): cada regra do enunciado vira uma função testável separada.
- **Constantes com nome** (`CORRENTE`, `MAX_CONTAS`) no lugar de números soltos.
