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

## Como explicar o código

### Roteiro (uns 5 minutos)

1. **A ideia (30 s).** "É um banco no terminal. Cada cliente tem até 2 contas, pode pegar um empréstimo e o banco mostra relatórios."
2. **Os 3 arquivos (1 min).** `meu_banco.h` *declara* (constantes, tipos, protótipos), `banco.c` *faz* (o corpo das funções), `main.c` só cuida do menu. Assim cada regra do banco fica numa função pequena e o `main` não vira uma bagunça.
3. **Os dados (1 min).** Três `struct` encaixadas: o `Banco` tem vários `Cliente`, e cada `Cliente` tem até 2 `Conta`. Mostre o `meu_banco.h` de cima pra baixo.
4. **Uma execução de ponta a ponta (2 min).** Abra o banco (1), cadastre um cliente com corrente + poupança (3), peça um empréstimo (5) e mostre os relatórios (6). Conforme faz, aponte a função que está rodando.
5. **Fechamento (30 s).** "As regras estão em funções separadas e as constantes têm nome. Se a regra mudar, troco um número no `.h`."

### De onde vem cada coisa que aparece na tela

| Você faz | Função que roda | O que ela decide |
| --- | --- | --- |
| Cadastra um cliente | `lerCliente` e `lerContas` | pergunta nome, tipo e saldo |
| Digita um tipo de conta | `podeAdicionarConta` → `combinacaoValida` | no máximo 2 contas, sem repetir, sem corrente + salário |
| A conta entra | `adicionarConta` | grava na próxima vaga livre (`contas[qtdContas]`) |
| Pede empréstimo | `podeEmprestimo` → `limiteEmprestimo` → `pegarEmprestimo` | confere as regras, calcula o limite e credita na corrente |
| Abre os relatórios | `mostrarRelatorios` | percorre todos os clientes e contas |

### Perguntas que costumam fazer (e respostas curtas)

- **Por que usar `struct`?** Porque um cliente tem nome, contas, histórico e outras informações que andam juntas. Uma `struct` agrupa tudo num tipo só, em vez de vários vetores soltos.
- **Por que dividir em `.h` e `.c`?** O `.h` diz *o que existe* e o `.c` diz *como funciona*. O `main` só precisa do `.h` pra chamar as funções.
- **O que é o `#ifndef MEU_BANCO_H`?** É o *include guard*: impede que o `.h` seja incluído duas vezes e dê erro de tipo repetido.
- **Por que `#define` nos números?** `CORRENTE` é mais claro que `1`, e se um valor mudar, troca em um lugar só.
- **Por que `c = adicionarConta(c, ...)`?** Em C, a função recebe uma **cópia** do cliente. Alterar a cópia não muda o original, então ela devolve o cliente atualizado e a gente guarda de volta.
- **Por que `const Banco *b` nos relatórios?** Elas só *leem* o banco. Passar o ponteiro evita copiar o banco inteiro (são 50 clientes), e o `const` garante que a função não altera nada. Com ponteiro, o acesso é `b->campo` em vez de `b.campo`.
- **Por que `100 * banco.qtdClientes + 101` no número da conta?** Dá 101 e 102 pro 1º cliente, 201 e 202 pro 2º, e assim por diante. Nenhum número se repete.
- **O que faz `scanf(" %49[^\n]", nome)`?** Lê o nome **com espaços** até o Enter. O espaço no começo descarta o Enter que sobrou da leitura anterior, e o `49` evita estourar o vetor `nome[50]` (sobra 1 posição pro `'\0'`).
- **Por que as funções devolvem 1 ou 0?** É a convenção do C: 1 = sim, 0 = não. Dá pra usar direto no `if`, como em `if (podeEmprestimo(c))`.
- **E se o usuário digitar algo errado?** Os `scanf` conferem o retorno; se não leu um número, o programa limpa a entrada (`limparBuffer`) e trata como inválido, sem travar.

### Pontos fortes pra destacar

- Cada regra do enunciado virou **uma função pequena e testável**.
- Constantes com nome em vez de números soltos.
- Compila com `-Wall -Wextra` sem nenhum aviso.
- Foi testado com um roteiro de 3 clientes, incluindo os casos de erro (conta barrada, empréstimo acima do limite, segundo empréstimo).

## A confirmar com a professora

1. `emprestimo` como `char` (`'S'`/`'N'`) ou `int` (0/1, como está).
2. Tipos de conta numerados de 1 a 3 (aqui) ou de 0 a 2.
3. Quantidade de clientes fixa em 3 ou variável (hoje: até `MAX_CLIENTES` = 50).
4. A base do limite do empréstimo (saldo da poupança, como está, ou soma das contas) e se os 25% estão certos.
5. Histórico por cliente (como está) ou do banco inteiro.
6. O título e o conteúdo exato da 4ª lista e do item "R$ valor : quantidade".

Se alguma regra mudar, quase tudo se resolve alterando uma função ou uma constante no `meu_banco.h`.
