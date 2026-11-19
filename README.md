# Carteira Digital (banco) em C

Trabalho de **Algoritmos e Pensamento Computacional** (UNIPÊ): simulação de uma carteira digital no terminal, seguindo o enunciado "Visão do Banco / Visão do Cliente". Usa **funções, registros (`struct`), alocação dinâmica (`malloc`/`realloc`/`free`), ponteiros, modularização e persistência em arquivo**.

## Estrutura (módulos)

| Arquivo | Responsabilidade |
| --- | --- |
| `main.c` | O menu principal. |
| `meu_banco.h` / `banco.c` | O **banco**: vetor dinâmico de clientes, abrir/cadastrar, arquivo (`salvarBanco`/`carregarBanco`) e o histórico do banco. |
| `cliente.h` / `cliente.c` | Tudo do **cliente**: `Transacao`, `Conta`, `Cliente`, regras de contas, depósito, saque, extrato, empréstimo, títulos e o menu do cliente (`sessaoCliente`). |
| `sorteio.h` / `sorteio.c` | O **sorteio** (Fibonacci). |
| `entrada.h` / `entrada.c` | Leitura segura do teclado (`lerInteiro`, `lerReal`, `limparBuffer`). |
| `banco.dat` | Criado sozinho: os dados salvos. Não vai pro Git. |

Quem inclui quem: `meu_banco.h` inclui `cliente.h` (o `Banco` tem `Cliente`); `sorteio.h` inclui `meu_banco.h`. O `cliente.c` não conhece o `Banco`: o `banco.c` chama `escreverCliente` e `lerClienteArquivo` para gravar e ler cada cliente.

## O que o enunciado pede e onde está

| Enunciado | Onde está |
| --- | --- |
| Campos: cliente, saldo, tipo de conta (01/02/03), histórico, número da conta | `cliente.h`: `Cliente`, `Conta`, `Transacao` |
| Número de conta de 4 dígitos a partir de 1000 + código do tipo (`1000-01`) | `Cliente.numero` (1000, 1001, ...) + `tipo`; impresso por `textoConta` (`cliente.c`) |
| O banco pode ter *n* clientes | `Banco.clientes` com `malloc`/`realloc` (`iniciarBanco`, `adicionarCliente`) |
| Clientes cadastrados com saldo zerado | `abrirBanco` → `cadastrarClientes` → `lerContas` |
| Menu: abrir banco, consultar histórico, sorteio, sair (histórico e sorteio só com o banco aberto) | `main.c` |
| Sem conta salário + corrente juntas | `combinacaoValida` / `podeAdicionarConta` |
| Empréstimo: tem poupança com saldo e conta corrente, um por cliente, limite de 25% de 10× o saldo | `podeEmprestimo`, `limiteEmprestimo`, `pegarEmprestimo` |
| Títulos de capitalização com salário entre R$ 1.000 e R$ 1.500 | `podeComprarTitulos`, `comprarTitulos` |
| Alerta de saldo negativo na conta corrente a cada acesso | `sessaoCliente` |
| Sorteio entre os 5 maiores saldos (todos com títulos), baseado em Fibonacci, prêmio de 10× o saldo | `sorteio.c`: `efetuarSorteio`, `fibonacci`, `sortearPosicao` |
| Saída do histórico do banco (5 seções do exemplo) | `mostrarHistoricoBanco` |
| Menu do cliente: depositar, sacar, empréstimo, títulos, extrato (últimas 5, `<valor> :<op>`) | `cliente.c`: `sessaoCliente`, `mostrarExtrato` |
| Persistência em arquivo | `salvarBanco`, `carregarBanco` |

## Como usar

**Menu principal**

| Opção | O que faz |
| --- | --- |
| `1` | Abre o banco. Se existir `banco.dat`, carrega os dados. Se não, pede quantos clientes cadastrar (contas com saldo zero). |
| `2` | Consulta o histórico do banco (saldo total, aptos a empréstimo, saldo negativo, aptos a títulos, contemplado no sorteio) |
| `3` | Efetua o sorteio |
| `4` | Acessa uma conta pelo número, ex.: `1000-01` (abre o menu do cliente) |
| `5` | Cadastra mais clientes |
| `s` / `S` | Sai |

**Menu do cliente:** `1` depositar, `2` sacar, `3` solicitar empréstimo, `4` adquirir títulos, `5` extrato (op: `S` saque, `D` depósito, `E` empréstimo), `0` voltar.

Os dados são salvos em `banco.dat` a cada volta do menu e ao sair. Pra começar do zero, apague o `banco.dat`.

## Decisões onde o enunciado não detalha (a confirmar com a professora)

1. **Número da conta:** o número de 4 dígitos é do **cliente** (1000, 1001, ...), e a conta é `número-tipo` (ex.: `1000-01` corrente e `1000-03` poupança do mesmo cliente).
2. **Saque:** só a **conta corrente** pode ficar negativa (é a que dispara o alerta). Salário e poupança não aceitam saque maior que o saldo.
3. **Empréstimo:** o limite usa o **saldo da poupança** e o valor é creditado na **conta corrente**.
4. **Títulos de capitalização:** exigem saldo de R$ 1.000 a R$ 1.500 na **conta salário**. O enunciado não diz o preço, então a compra **não debita** nada; só registra que o cliente tem títulos.
5. **Sorteio:** participam os 5 clientes de **maior saldo total** (somando as contas). Todos precisam ter títulos, senão o sorteio é cancelado. Com menos de 5 clientes, participam todos. O prêmio é **10× o saldo da conta salário** do contemplado e é creditado nela (registrado como depósito).
6. **Algoritmo de Fibonacci do sorteio:** não tenho o algoritmo exato que foi feito em sala. Aqui: escolhe-se um `n` a partir da hora (entre 20 e 49), calcula-se o `fibonacci(n)` e o contemplado é o resto da divisão pela quantidade de participantes. Se o da sala for diferente, só `sortearPosicao` (e `fibonacci`) precisam mudar.
7. **"Saldo negativo na conta salário":** o exemplo de saída do PDF usa esse título, mas o texto fala de saldo negativo na conta **corrente**. A lista mostra as contas **corrente e salário** com saldo negativo (o número da conta indica qual é). O título é o do exemplo.
8. **"Clientes aptos a compra de títulos":** lista quem tem salário entre 1.000 e 1.500, tendo comprado ou não.
9. O menu principal tem uma opção a mais que o enunciado (`4 - Acessar conta`) para entrar na visão do cliente, e `5 - Cadastrar cliente` para novos clientes depois da abertura.

## Memória dinâmica e ponteiros

| Onde | O que é dinâmico | Como cresce | Quem devolve |
| --- | --- | --- | --- |
| `Banco.clientes` | o vetor de clientes | começa com 2 e **dobra** com `realloc` | `liberarBanco` |
| `Cliente.historico` | o vetor de transações | começa com 4 e dobra | `liberarCliente` |
| `efetuarSorteio` | vetor auxiliar `usado` (`calloc`) | tamanho = nº de clientes | `free` na própria função |

- Funções que **alteram** recebem ponteiro (`Cliente *c`, `Banco *b`, `Conta *conta`) e usam `->`. Funções que só **leem** recebem `const`.
- `buscarCliente` e `buscarConta` devolvem **o endereço** do cliente/conta (ou `NULL` se não existir), e quem chama altera o original.
- Todo `malloc`/`realloc` é conferido. O `realloc` usa um ponteiro auxiliar para não perder o vetor antigo se falhar.

## Arquivo (`banco.dat`)

Texto simples, uma estrutura por linha:

```
<qtdClientes> <ganhador> <premio>
<numero> <emprestimo> <titulos> <qtdContas> <qtdTransacoes>
<nome>
<tipo> <saldo>           (uma linha por conta)
<valor> <op>             (uma linha por transação)
```

`carregarBanco` confere o formato: se o arquivo estiver corrompido, o banco volta ao estado vazio e pede o cadastro.

## Testado

- Cadastro (inclusive combinação proibida), acesso por `1000-01`, depósito, saque, saque que deixa a corrente negativa com alerta no acesso seguinte, empréstimo (acima do limite recusado, no limite aceito, segundo recusado), títulos, extrato, as 5 seções do histórico.
- Sorteio com 6 clientes (o de menor saldo fica de fora), cancelado quando alguém não tem títulos, prêmio de 10× creditado, e com menos de 5 clientes.
- Fechar e reabrir: os dados voltam do `banco.dat`. Arquivo com lixo ou truncado não derruba o programa.
- Compilado com `-fsanitize=address,undefined`: sem vazamento e sem acesso inválido, inclusive com a entrada acabando no meio do cadastro.

## Como explicar o código

1. **A ideia (30 s):** "É uma carteira digital. O banco tem vários clientes, cada um com até 2 contas. O cliente deposita, saca, pega empréstimo e compra títulos; o banco faz sorteio e mostra relatórios. Tudo fica salvo em arquivo."

3. **Os dados (1 min):** o `Banco` aponta pra um vetor de `Cliente`; cada `Cliente` tem até 2 `Conta` e um histórico de `Transacao`. Aponte os dois ponteiros (`Cliente *clientes` e `Transacao *historico`).
4. **A memória (1 min):** "começa com espaço pra 2 clientes e dobra com `realloc` quando enche; no final o `liberarBanco` devolve tudo com `free`."
5. **Uma execução (2 min):** abrir banco (cadastrar), acessar `1000-01`, depositar, sacar, empréstimo, extrato; depois o histórico do banco e o sorteio.

**Perguntas que costumam fazer**

- **Ponteiro?** Variável que guarda o **endereço** de outra coisa. `Cliente *c` aponta para um cliente em vez de ser uma cópia.
- **Por que passar ponteiro?** Em C a função recebe uma cópia. Com ponteiro ela altera o original e não copia a struct inteira.
- **`->` e `&`?** `c->saldo` é "o `saldo` do cliente para o qual `c` aponta". `&novo` é "o endereço de `novo`".
- **`malloc`, `realloc`, `free`?** Pedir memória, trocar por um bloco maior (copiando o conteúdo) e devolver. Todo `malloc` precisa de um `free`.
- **Por que `.h` e `.c`?** O `.h` diz o que existe; o `.c` diz como funciona.
- **Por que as funções devolvem 1 ou 0?** Convenção do C: 1 = sim, 0 = não, e dá pra usar direto no `if`.
- **Como salva?** `fprintf` escreve em texto no `banco.dat`; `fscanf`/`fgets` leem de volta e recriam os clientes com `malloc`.
- **Como funciona o sorteio?** Pega os 5 maiores saldos, confere os títulos, calcula `fibonacci(n) % participantes` e credita 10× o saldo salário do sorteado.
