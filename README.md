# Carteira Digital (banco) em C

Trabalho de **Algoritmos e Pensamento Computacional** (UNIPÊ): carteira digital no terminal, seguindo o enunciado "Visão do Banco / Visão do Cliente". Usa funções, `struct`, `malloc`/`realloc`/`free`, ponteiros, modularização e arquivo.

## Arquivos

| Arquivo | O que tem |
| --- | --- |
| `meu_banco.h` | Constantes, `struct`s (`Transacao`, `Conta`, `Cliente`, `Banco`) e protótipos |
| `banco.c` | Todas as funções: banco, arquivo, cliente, sorteio e histórico |
| `main.c` | O menu principal |
| `banco.dat` | Criado sozinho com os dados salvos (não vai pro Git) |

## Compilar e rodar

```
gcc -Wall -Wextra main.c banco.c -o banco
.\banco.exe        (PowerShell)
./banco            (Linux / Mac)
```

## Como usar

**Menu principal:** `1` abrir banco (carrega o `banco.dat` ou pede quantos clientes cadastrar, com contas zeradas) · `2` consultar histórico · `3` efetuar sorteio · `4` acessar conta (ex.: `1000-01`) · `s` sair. As opções 2, 3 e 4 só funcionam com o banco aberto.

**Menu do cliente:** `1` depositar · `2` sacar · `3` empréstimo · `4` títulos de capitalização · `5` extrato (últimas 5: `<valor> :<op>`, com `S` saque, `D` depósito, `E` empréstimo) · `0` voltar.

Os dados são salvos a cada volta do menu. Pra começar do zero, apague o `banco.dat`.

## Regras do enunciado e onde estão

| Regra | Função |
| --- | --- |
| Conta `1000-01` (número do cliente a partir de 1000 + tipo 01/02/03) | `Cliente.numero` + `Conta.tipo` |
| *n* clientes | `iniciarBanco`, `adicionarCliente` (`malloc` / `realloc`), `liberarBanco` (`free`) |
| Sem salário + corrente juntas (e sem repetir tipo) | `podeAdicionarConta` |
| Empréstimo: corrente + poupança com saldo, um por cliente, limite 25% de 10× a poupança | `limiteEmprestimo` (devolve 0 se não pode) |
| Títulos: salário entre R$ 1.000 e R$ 1.500 | `podeTitulos` |
| Alerta de saldo negativo na corrente a cada acesso | `sessaoCliente` |
| Sorteio entre os 5 maiores saldos (todos com títulos), Fibonacci, prêmio 10× | `efetuarSorteio`, `fibonacci` |
| Histórico do banco (5 seções do exemplo) | `mostrarHistoricoBanco` |
| Persistência em arquivo | `salvarBanco` (`fwrite`), `carregarBanco` (`fread`) |

## Decisões onde o enunciado não detalha (confirmar com a professora)

1. O número de 4 dígitos é do **cliente** (1000, 1001, ...). A conta é `número-tipo`.
2. Só a conta **corrente** pode ficar negativa no saque.
3. O limite do empréstimo usa o saldo da **poupança**, e o valor entra na **corrente**.
4. Comprar títulos não debita nada (o enunciado não diz o preço); só marca que o cliente tem.
5. Sorteio: participam os 5 de maior saldo total; todos precisam ter títulos, senão cancela. O prêmio (10× o saldo salário) é creditado na conta **salário** do sorteado.
6. **Fibonacci:** usei o Fibonacci **recursivo** da aula. Aqui, `n` vem da hora (20 a 29) e o sorteado é `fibonacci(n) % participantes`. Se o algoritmo da sala for outro, só `fibonacci` e a linha do sorteio em `efetuarSorteio` mudam.
7. O exemplo do PDF intitula uma lista "Saldo Negativo na Conta **Salário**", mas o texto fala da conta **corrente**. A lista mostra contas corrente e salário negativas, com o título do exemplo.
8. "Aptos a compra de títulos" lista quem tem salário entre 1.000 e 1.500, tendo comprado ou não.
9. O menu tem `4 - Acessar conta` além das opções do enunciado, para entrar na visão do cliente.

## Como explicar o código

- **Dados:** `Banco` aponta para um vetor de `Cliente` (alocado com `malloc` e dobrado com `realloc` quando enche); cada `Cliente` tem até 2 `Conta` e as últimas 5 `Transacao`.
- **Ponteiro:** variável que guarda o **endereço** de algo. `Cliente *c` altera o cliente original, sem copiar a `struct`. `c->saldo` é "o saldo do cliente para o qual `c` aponta", e `&x` é "o endereço de `x`".
- **`malloc` / `realloc` / `free`:** pedir memória, trocar por um bloco maior (copiando o conteúdo) e devolver. O `realloc` usa um ponteiro auxiliar pra não perder o vetor antigo se falhar.
- **Arquivo:** `fwrite` grava o vetor de clientes de uma vez no `banco.dat`; `fread` lê de volta. O programa confere o arquivo e, se estiver corrompido, pede o cadastro de novo.
- **Funções que respondem 1 ou 0:** convenção do C (1 = sim, 0 = não), usadas direto no `if`.
- **Sorteio:** pega os 5 maiores saldos, confere os títulos, calcula `fibonacci(n) % participantes` e credita 10× o saldo salário.
