# Carteira Digital (banco) em C

Trabalho de **Algoritmos e Pensamento Computacional** (UNIPÊ). Programa em C, no terminal, que simula um banco: cadastra clientes com até 2 contas (corrente, salário ou poupança), controla empréstimo, guarda um histórico e mostra relatórios.

A memória é **dinâmica** (`malloc`, `realloc`, `free`) e as funções trabalham com **ponteiros**.

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
| `s` ou `S` | Sai (e devolve toda a memória) |

## Regras implementadas

- Cada cliente tem de **1 a 2 contas**, sem repetir o tipo.
- **Corrente + salário não pode.** Combinações aceitas: corrente + poupança, salário + poupança, ou uma conta só.
- Tipos de conta e limites são **constantes com nome** (`CORRENTE`, `SALARIO`, `POUPANCA`, `MAX_CONTAS`, ...).
- **Empréstimo:** o cliente precisa ter conta corrente **e** poupança, a poupança com saldo maior que zero, e **só pode pegar um**. O limite é **25% de 10 vezes o saldo da poupança** (`MULT_EMPRESTIMO` e `PERC_EMPRESTIMO`). O valor entra na conta corrente.
- **Histórico** por cliente: abertura de contas e empréstimo.
- **Relatórios (opção 6):** saldo total do banco; clientes aptos a empréstimo; clientes com saldo negativo; contas salário com saldo entre 1000 e 1500.

## Memória dinâmica e ponteiros

| Onde | O que é dinâmico | Como cresce | Quem devolve |
| --- | --- | --- | --- |
| `Banco.clientes` | o vetor de clientes | começa com `CAPACIDADE_INICIAL` (2) e **dobra** com `realloc` quando enche | `liberarBanco` |
| `Cliente.historico` | o vetor de textos do histórico | começa com 4 e dobra | `liberarCliente` |
| cada texto do histórico | uma cópia do texto, com `malloc(strlen(texto) + 1)` | tamanho exato | `liberarCliente` |

- `iniciarBanco` faz o primeiro `malloc`; `adicionarCliente` faz o `realloc`; `liberarBanco` faz os `free` (primeiro o histórico de cada cliente, depois o vetor de clientes).
- As funções que **alteram** recebem um ponteiro (`Cliente *c`, `Banco *b`) e usam a seta: `c->qtdContas`. Chamada: `adicionarConta(&novo, 101, CORRENTE, 500)`.
- As funções que só **leem** recebem `const Cliente *` ou `const Banco *`: o `const` garante que elas não alteram nada.
- Todo `malloc`/`realloc` é conferido (`NULL` = faltou memória), e o `realloc` usa um ponteiro auxiliar para não perder o vetor antigo se falhar.

## Funções principais (`banco.c`)

- Ciclo de vida: `iniciarBanco`, `adicionarCliente`, `liberarCliente`, `liberarBanco`
- Contas: `combinacaoValida`, `podeAdicionarConta`, `adicionarConta`, `temConta`, `saldoDoTipo`, `nomeTipo`
- Cadastro: `lerCliente`, `lerContas`
- Empréstimo: `podeEmprestimo`, `limiteEmprestimo`, `pegarEmprestimo`
- Histórico: `registrarHistorico`, `mostrarHistorico`
- Relatórios: `listarClientes`, `saldoTotal`, `mostrarRelatorios`
- Entrada: `limparBuffer` (descarta o que sobrou digitado até o Enter)

## Testado

- Roteiro com 3 clientes: conta barrada (salário + corrente), empréstimo acima do limite recusado, segundo empréstimo recusado, cliente inexistente, saldo negativo, relatórios e histórico conferidos.
- 9 clientes seguidos (força o `realloc` do vetor de clientes 3 vezes) e 20 registros de histórico em um cliente (força o `realloc` do histórico).
- Compilado com `-fsanitize=address,undefined`: **sem vazamento de memória e sem acesso inválido**.

## Como explicar o código

### Roteiro (uns 5 minutos)

1. **A ideia (30 s).** "É um banco no terminal. Cada cliente tem até 2 contas, pode pegar um empréstimo e o banco mostra relatórios."
2. **Os 3 arquivos (1 min).** `meu_banco.h` *declara* (constantes, tipos, protótipos), `banco.c` *faz* (o corpo das funções), `main.c` só cuida do menu.
3. **Os dados (1 min).** Três `struct` encaixadas: o `Banco` aponta para um vetor de `Cliente`, e cada `Cliente` tem até 2 `Conta` e um histórico. Mostre o `meu_banco.h` de cima pra baixo e aponte os dois `*` (`Cliente *clientes` e `char **historico`).
4. **A memória (1 min).** "O banco não tem tamanho fixo: começa com espaço pra 2 clientes e, quando enche, o `realloc` dobra. No fim, `liberarBanco` devolve tudo com `free`."
5. **Uma execução (1 min 30 s).** Abra o banco (1), cadastre um cliente com corrente + poupança (3), peça um empréstimo (5) e mostre os relatórios (6), apontando a função que roda em cada passo.

### De onde vem cada coisa que aparece na tela

| Você faz | Função que roda | O que ela decide |
| --- | --- | --- |
| Cadastra um cliente | `lerCliente` e `lerContas` | pergunta nome, tipo e saldo |
| Digita um tipo de conta | `podeAdicionarConta` → `combinacaoValida` | no máximo 2 contas, sem repetir, sem corrente + salário |
| A conta entra | `adicionarConta` | grava na próxima vaga livre (`contas[qtdContas]`) e registra no histórico |
| O cliente entra no banco | `adicionarCliente` | copia pro vetor; se encheu, dobra com `realloc` |
| Pede empréstimo | `podeEmprestimo` → `limiteEmprestimo` → `pegarEmprestimo` | confere as regras, calcula o limite e credita na corrente |
| Abre os relatórios | `mostrarRelatorios` | percorre todos os clientes e contas |
| Sai do programa | `liberarBanco` | `free` em tudo que foi pedido com `malloc` |

### Perguntas que costumam fazer (e respostas curtas)

- **O que é um ponteiro?** Uma variável que guarda **o endereço** de outra coisa na memória. `Cliente *c` guarda onde o cliente está, em vez de uma cópia dele.
- **Por que passar ponteiro e não o cliente inteiro?** Em C a função recebe uma **cópia**. Com ponteiro, ela altera o cliente **original** (não precisa devolver) e não gasta tempo copiando a struct inteira.
- **O que é `->`?** É o ponto para ponteiros: `c->qtdContas` significa "o `qtdContas` do cliente **para o qual** `c` aponta". É o mesmo que `(*c).qtdContas`.
- **O que é o `&` em `adicionarConta(&novo, ...)`?** "O endereço de `novo`". É assim que se entrega o ponteiro para a função.
- **O que faz o `malloc`?** Pede ao sistema um bloco de memória do tamanho que você diz (`n * sizeof(Cliente)`) e devolve o endereço dele, ou `NULL` se não houver.
- **E o `realloc`?** Troca o bloco por um maior, copiando o conteúdo. Pode devolver outro endereço, por isso a gente guarda o resultado num ponteiro auxiliar e só troca se não for `NULL`.
- **Por que precisa de `free`?** Memória pedida com `malloc` não some sozinha. Sem `free` ela fica ocupada até o programa acabar (*vazamento de memória*).
- **Por que o `liberarBanco` faz `free` em tantas coisas?** Cada `malloc` precisa do seu `free`. Cada texto do histórico foi um `malloc`, o vetor de textos foi outro e o vetor de clientes outro. Libera de dentro pra fora.
- **Por que `const Banco *b` nos relatórios?** Elas só *leem*. O `const` impede que alterem o banco sem querer, e o ponteiro evita copiar o banco inteiro.
- **Por que `.h` e `.c` separados?** O `.h` diz *o que existe* e o `.c` diz *como funciona*. O `main` só precisa do `.h`.
- **O que é o `#ifndef MEU_BANCO_H`?** O *include guard*: impede que o `.h` seja incluído duas vezes e dê erro de tipo repetido.
- **Por que `#define` nos números?** `CORRENTE` é mais claro que `1`, e se um valor mudar, troca em um lugar só.
- **Por que `100 * banco.qtdClientes + 101` no número da conta?** Dá 101 e 102 pro 1º cliente, 201 e 202 pro 2º, e assim por diante. Nenhum número se repete.
- **O que faz `scanf(" %49[^\n]", nome)`?** Lê o nome **com espaços** até o Enter. O espaço no começo descarta o Enter que sobrou da leitura anterior, e o `49` evita estourar o vetor `nome[50]` (sobra 1 posição pro `'\0'`).
- **Por que as funções devolvem 1 ou 0?** É a convenção do C: 1 = sim, 0 = não. Dá pra usar direto no `if`, como em `if (podeEmprestimo(c))`.
- **E se o usuário digitar algo errado?** Os `scanf` conferem o retorno; se não leu um número, o programa limpa a entrada (`limparBuffer`) e trata como inválido, sem travar.

### Pontos fortes pra destacar

- Cada regra do enunciado virou **uma função pequena e testável**.
- Memória dinâmica de verdade: o banco **cresce sozinho** e **devolve tudo** no final (conferido com detector de vazamento).
- Constantes com nome em vez de números soltos.
- Compila com `-Wall -Wextra` sem nenhum aviso.

## A confirmar com a professora

1. `emprestimo` como `char` (`'S'`/`'N'`) ou `int` (0/1, como está).
2. Tipos de conta numerados de 1 a 3 (aqui) ou de 0 a 2.
3. Onde ela espera o `malloc`: hoje está nos clientes e no histórico (as contas são um vetor fixo de 2, porque a regra limita a 2).
4. A base do limite do empréstimo (saldo da poupança, como está, ou soma das contas) e se os 25% estão certos.
5. Histórico por cliente (como está) ou do banco inteiro.
6. O título e o conteúdo exato da 4ª lista e do item "R$ valor : quantidade".

Se alguma regra mudar, quase tudo se resolve alterando uma função ou uma constante no `meu_banco.h`.
