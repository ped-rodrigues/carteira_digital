# Carteira Digital (banco) em C

Trabalho de **Algoritmos e Pensamento Computacional** (UNIPÊ). Programa em C, no terminal, que simula um banco: cadastra clientes com até 2 contas (corrente, salário ou poupança), controla empréstimo, guarda um histórico e mostra relatórios.

## Estrutura

| Arquivo | Papel |
| --- | --- |
| `meu_banco.h` | Constantes, `typedef struct` (`Conta`, `Cliente`, `Banco`) e protótipos. Tem *include guard*. |
| `banco.c` | O corpo de todas as funções do banco. |
| `main.c` | O menu e o laço principal. |

## Como compilar e rodar

```
gcc -Wall -Wextra main.c banco.c -o banco
.\banco.exe        (PowerShell)
./banco            (Linux / Mac)
```

O projeto compila **sem nenhum aviso**.

## Menu

| Opção | O que faz |
| --- | --- |
| `1` | Abre o banco (as outras opções só funcionam depois disso) |
| `2` | Mostra o histórico de cada cliente |
| `3` | Cadastra um cliente e suas contas |
| `4` | Lista os clientes, com as contas e se já pegaram empréstimo |
| `5` | Pede empréstimo (escolhe o cliente e o valor) |
| `6` | Relatórios finais |
| `s` ou `S` | Sai |

## Regras implementadas

- Cada cliente tem de **1 a 2 contas**, sem repetir o tipo.
- **Corrente + salário não pode.** Combinações aceitas: corrente + poupança, salário + poupança, ou uma conta só.
- Tipos de conta e limites são **constantes com nome** (`CORRENTE`, `SALARIO`, `POUPANCA`, `MAX_CONTAS`, ...).
- **Empréstimo:** o cliente precisa ter conta corrente **e** poupança, a poupança com saldo maior que zero, e **só pode pegar um** empréstimo. O limite é **25% de 10 vezes o saldo da poupança** (`MULT_EMPRESTIMO` e `PERC_EMPRESTIMO`). O valor entra na conta corrente.
- **Histórico** por cliente: abertura de contas e empréstimo. Guarda até 20 linhas e descarta as mais antigas.
- **Relatórios (opção 6):** saldo total do banco; clientes aptos a empréstimo; clientes com saldo negativo; contas salário com saldo entre 1000 e 1500.

## Funções principais (`banco.c`)

- Contas: `combinacaoValida`, `podeAdicionarConta`, `adicionarConta`, `temConta`, `saldoDoTipo`, `nomeTipo`
- Cadastro: `lerCliente`, `lerContas`
- Empréstimo: `podeEmprestimo`, `limiteEmprestimo`, `pegarEmprestimo`
- Histórico: `registrarHistorico`, `mostrarHistorico`
- Relatórios: `listarClientes`, `saldoTotal`, `mostrarRelatorios`
- Entrada: `limparBuffer` (descarta o que sobrou digitado até o Enter)

As funções que só **leem** o banco (`mostrarRelatorios`, `listarClientes`, ...) recebem `const Banco *` (um ponteiro, para não copiar o banco inteiro) e usam `b->campo`. As que **alteram** um cliente recebem uma cópia e devolvem o cliente atualizado (`c = adicionarConta(c, ...)`).

## Testado

Roteiro com 3 clientes: conta barrada (salário + corrente), empréstimo acima do limite recusado, segundo empréstimo recusado, cliente inexistente, saldo negativo, relatórios e histórico conferidos com os valores esperados.

## A confirmar com a professora

1. `emprestimo` como `char` (`'S'`/`'N'`) ou `int` (0/1, como está).
2. Tipos de conta numerados de 1 a 3 (aqui) ou de 0 a 2.
3. Quantidade de clientes fixa em 3 ou variável (hoje: até `MAX_CLIENTES` = 50).
4. A base do limite do empréstimo (saldo da poupança, como está, ou soma das contas) e se os 25% estão certos.
5. Histórico por cliente (como está) ou do banco inteiro.
6. O título e o conteúdo exato da 4ª lista e do item "R$ valor : quantidade".

Se alguma regra mudar, quase tudo se resolve alterando uma função ou uma constante no `meu_banco.h`.
