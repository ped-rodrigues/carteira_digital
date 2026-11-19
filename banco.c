#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "meu_banco.h"

/* ---------- entrada ---------- */

void limparBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }
}

int lerInteiro(int *v) {
    if (scanf("%d", v) != 1) {
        limparBuffer();
        return 0;
    }
    return 1;
}

int lerReal(float *v) {
    if (scanf("%f", v) != 1) {
        limparBuffer();
        return 0;
    }
    return 1;
}

/* ---------- banco: memoria dinamica e arquivo ---------- */

int iniciarBanco(Banco *b) {
    b->capacidade = 2;
    b->clientes = malloc(b->capacidade * sizeof(Cliente));
    b->qtdClientes = 0;
    b->aberto = 0;
    b->ganhador = -1;
    b->premio = 0;
    return b->clientes != NULL;
}

int adicionarCliente(Banco *b, Cliente *c) {
    if (b->qtdClientes == b->capacidade) {
        Cliente *novo = realloc(b->clientes, 2 * b->capacidade * sizeof(Cliente));
        if (novo == NULL) {
            return 0;
        }
        b->clientes = novo;
        b->capacidade = 2 * b->capacidade;
    }
    b->clientes[b->qtdClientes] = *c;
    b->qtdClientes = b->qtdClientes + 1;
    return 1;
}

void liberarBanco(Banco *b) {
    free(b->clientes);
    b->clientes = NULL;
    b->qtdClientes = 0;
    b->capacidade = 0;
}

int salvarBanco(Banco *b) {
    FILE *f = fopen(ARQUIVO, "wb");

    if (f == NULL) {
        return 0;
    }
    fwrite(&b->qtdClientes, sizeof(int), 1, f);
    fwrite(&b->ganhador, sizeof(int), 1, f);
    fwrite(&b->premio, sizeof(float), 1, f);
    fwrite(b->clientes, sizeof(Cliente), b->qtdClientes, f);
    fclose(f);
    return 1;
}

int carregarBanco(Banco *b) {
    FILE *f = fopen(ARQUIVO, "rb");
    int n, ganhador, i;
    float premio;
    Cliente *novo;

    if (f == NULL) {
        return 0;
    }
    if (fread(&n, sizeof(int), 1, f) != 1 || fread(&ganhador, sizeof(int), 1, f) != 1 ||
        fread(&premio, sizeof(float), 1, f) != 1 || n < 1 || n > 100000) {
        fclose(f);
        return 0;
    }
    novo = realloc(b->clientes, n * sizeof(Cliente));
    if (novo == NULL) {
        fclose(f);
        return 0;
    }
    b->clientes = novo;
    b->capacidade = n;
    if (fread(novo, sizeof(Cliente), n, f) != (size_t)n) {
        fclose(f);
        return 0;
    }
    fclose(f);

    for (i = 0; i < n; i++) {
        novo[i].nome[sizeof(novo[i].nome) - 1] = '\0';
        if (novo[i].qtdContas < 0 || novo[i].qtdContas > MAX_CONTAS || novo[i].qtdExtrato < 0 ||
            novo[i].qtdExtrato > EXTRATO_QTD) {
            return 0;
        }
    }
    b->qtdClientes = n;
    b->ganhador = (ganhador >= 0 && ganhador < n) ? ganhador : -1;
    b->premio = premio;
    return 1;
}

/* ---------- contas e clientes ---------- */

Cliente *buscarCliente(Banco *b, int numero) {
    int i;
    for (i = 0; i < b->qtdClientes; i++) {
        if (b->clientes[i].numero == numero) {
            return &b->clientes[i];
        }
    }
    return NULL;
}

Conta *buscarConta(Cliente *c, int tipo) {
    int i;
    for (i = 0; i < c->qtdContas; i++) {
        if (c->contas[i].tipo == tipo) {
            return &c->contas[i];
        }
    }
    return NULL;
}

float saldoCliente(Cliente *c) {
    int i;
    float total = 0;
    for (i = 0; i < c->qtdContas; i++) {
        total = total + c->contas[i].saldo;
    }
    return total;
}

/* sem repetir o tipo e sem corrente + salario juntas */
int podeAdicionarConta(Cliente *c, int tipo) {
    int outro;

    if (c->qtdContas == 0) {
        return 1;
    }
    if (c->qtdContas >= MAX_CONTAS) {
        return 0;
    }
    outro = c->contas[0].tipo;
    if (outro == tipo) {
        return 0;
    }
    return !((outro == CORRENTE && tipo == SALARIO) || (outro == SALARIO && tipo == CORRENTE));
}

void lerCliente(Cliente *c, int numero) {
    int tipo;
    int mais = 1;

    memset(c, 0, sizeof(Cliente));
    c->numero = numero;
    printf("Nome do cliente: ");
    if (scanf(" %49[^\n]", c->nome) != 1) {
        limparBuffer();
        return;
    }

    while (c->qtdContas < MAX_CONTAS && (mais || c->qtdContas == 0) && !feof(stdin)) {
        printf("Tipo de conta (1-corrente, 2-salario, 3-poupanca): ");
        if (!lerInteiro(&tipo)) {
            tipo = 0;
        }

        if (tipo < CORRENTE || tipo > POUPANCA) {
            printf("Tipo invalido.\n");
        } else if (!podeAdicionarConta(c, tipo)) {
            printf("Esse cliente nao pode ter essa conta.\n");
        } else {
            c->contas[c->qtdContas].tipo = tipo;
            c->contas[c->qtdContas].saldo = 0;
            c->qtdContas = c->qtdContas + 1;
        }

        if (c->qtdContas > 0 && c->qtdContas < MAX_CONTAS) {
            printf("Adicionar outra conta? (1-sim, 0-nao): ");
            if (!lerInteiro(&mais)) {
                mais = 0;
            }
        }
    }
}

int cadastrarClientes(Banco *b) {
    int n, i;

    printf("Quantos clientes deseja cadastrar? ");
    if (!lerInteiro(&n) || n < 1) {
        printf("Quantidade invalida.\n");
        return 0;
    }
    for (i = 0; i < n; i++) {
        Cliente novo;

        printf("\nCliente %d de %d\n", i + 1, n);
        lerCliente(&novo, NUMERO_INICIAL + b->qtdClientes);
        if (novo.qtdContas == 0 || !adicionarCliente(b, &novo)) {
            printf("Cadastro interrompido.\n");
            break;
        }
        printf("Cliente cadastrado. Numero: %d\n", novo.numero);
    }
    return b->qtdClientes;
}

void abrirBanco(Banco *b) {
    if (b->aberto) {
        printf("O banco ja esta aberto.\n");
        return;
    }
    if (carregarBanco(b)) {
        printf("Banco aberto. %d cliente(s) carregado(s) de %s.\n", b->qtdClientes, ARQUIVO);
    } else {
        printf("Nenhum dado salvo. Cadastre os clientes (contas com saldo zero).\n");
        if (cadastrarClientes(b) == 0) {
            printf("Nenhum cliente cadastrado. O banco continua fechado.\n");
            return;
        }
        printf("Banco aberto.\n");
    }
    b->aberto = 1;
}

/* ---------- visao do cliente ---------- */

void registrar(Cliente *c, float valor, char op) {
    int i;

    if (c->qtdExtrato == EXTRATO_QTD) {
        for (i = 1; i < EXTRATO_QTD; i++) {
            c->extrato[i - 1] = c->extrato[i];
        }
        c->qtdExtrato = c->qtdExtrato - 1;
    }
    c->extrato[c->qtdExtrato].valor = valor;
    c->extrato[c->qtdExtrato].op = op;
    c->qtdExtrato = c->qtdExtrato + 1;
}

/* 0 = nao pode; senao, 25% de 10 vezes o saldo da poupanca */
float limiteEmprestimo(Cliente *c) {
    Conta *poupanca = buscarConta(c, POUPANCA);

    if (c->emprestimo || buscarConta(c, CORRENTE) == NULL || poupanca == NULL || poupanca->saldo <= 0) {
        return 0;
    }
    return poupanca->saldo * MULT_EMPRESTIMO * PERC_EMPRESTIMO;
}

int podeTitulos(Cliente *c) {
    Conta *salario = buscarConta(c, SALARIO);
    return salario != NULL && salario->saldo >= TITULO_MIN && salario->saldo <= TITULO_MAX;
}

void sessaoCliente(Cliente *c, Conta *conta) {
    int op = -1, i;
    float valor, limite;
    Conta *corrente = buscarConta(c, CORRENTE);

    printf("\nBem-vindo(a), %s | conta %d-%02d | saldo R$ %.2f\n", c->nome, c->numero, conta->tipo, conta->saldo);
    if (corrente != NULL && corrente->saldo < 0) {
        printf("ALERTA: sua conta corrente esta com saldo negativo (R$ %.2f).\n", corrente->saldo);
    }

    while (op != 0 && !feof(stdin)) {
        printf("\n===Cliente===\n1 - Depositar\n2 - Sacar\n3 - Solicitar emprestimo\n");
        printf("4 - Adquirir titulos de capitalizacao\n5 - Solicitar extrato\n0 - Voltar\nopcao: ");
        if (!lerInteiro(&op)) {
            op = -1;
            printf("opcao invalida.\n");
            continue;
        }

        switch (op) {
        case 1:
        case 2:
            printf("Valor: ");
            if (!lerReal(&valor) || valor <= 0) {
                printf("Valor invalido.\n");
            } else if (op == 1) {
                conta->saldo = conta->saldo + valor;
                registrar(c, valor, 'D');
                printf("Deposito realizado. Saldo: R$ %.2f\n", conta->saldo);
            } else if (conta->tipo != CORRENTE && valor > conta->saldo) {
                printf("Saldo insuficiente.\n");
            } else {
                conta->saldo = conta->saldo - valor;
                registrar(c, valor, 'S');
                printf("Saque realizado. Saldo: R$ %.2f\n", conta->saldo);
            }
            break;

        case 3:
            limite = limiteEmprestimo(c);
            if (limite <= 0) {
                printf("Emprestimo negado: precisa de conta corrente e poupanca com saldo, e so e permitido um por cliente.\n");
                break;
            }
            printf("Limite do emprestimo: R$ %.2f\nValor desejado: ", limite);
            if (!lerReal(&valor) || valor <= 0 || valor > limite) {
                printf("Valor invalido (maior que 0 e ate o limite).\n");
            } else {
                corrente->saldo = corrente->saldo + valor;
                c->emprestimo = 1;
                registrar(c, valor, 'E');
                printf("Emprestimo aprovado e creditado na conta corrente.\n");
            }
            break;

        case 4:
            if (c->titulos) {
                printf("Voce ja possui titulos de capitalizacao.\n");
            } else if (podeTitulos(c)) {
                c->titulos = 1;
                printf("Titulos de capitalizacao adquiridos.\n");
            } else {
                printf("Para adquirir titulos e preciso ter conta salario com saldo entre R$ %.2f e R$ %.2f.\n", TITULO_MIN, TITULO_MAX);
            }
            break;

        case 5:
            if (c->qtdExtrato == 0) {
                printf("Nenhuma transacao.\n");
            }
            for (i = 0; i < c->qtdExtrato; i++) {
                printf("%.2f :%c\n", c->extrato[i].valor, c->extrato[i].op);
            }
            break;

        case 0:
            break;

        default:
            printf("opcao invalida.\n");
            break;
        }
    }
}

void acessarConta(Banco *b) {
    int numero, tipo;
    Cliente *cliente;
    Conta *conta = NULL;

    printf("Numero da conta (ex.: 1000-01): ");
    if (scanf("%d-%d", &numero, &tipo) != 2) {
        limparBuffer();
        printf("Numero de conta invalido.\n");
        return;
    }
    cliente = buscarCliente(b, numero);
    if (cliente != NULL) {
        conta = buscarConta(cliente, tipo);
    }
    if (conta == NULL) {
        printf("Conta nao encontrada.\n");
    } else {
        sessaoCliente(cliente, conta);
    }
}

/* ---------- sorteio ---------- */

int fibonacci(int n) {
    if (n < 2) {
        return n;
    }
    return fibonacci(n - 1) + fibonacci(n - 2);
}

void efetuarSorteio(Banco *b) {
    int n = b->qtdClientes;
    int k = (n < PARTICIPANTES) ? n : PARTICIPANTES;
    int i, j, t, ok = 1;
    int *ordem;
    Cliente *v;
    Conta *salario;
    float premio;

    if (n == 0) {
        printf("Nenhum cliente cadastrado.\n");
        return;
    }
    ordem = malloc(n * sizeof(int));
    if (ordem == NULL) {
        printf("Sem memoria para o sorteio.\n");
        return;
    }
    for (i = 0; i < n; i++) {
        ordem[i] = i;
    }
    for (i = 0; i < k; i++) {
        for (j = i + 1; j < n; j++) {
            if (saldoCliente(&b->clientes[ordem[j]]) > saldoCliente(&b->clientes[ordem[i]])) {
                t = ordem[i];
                ordem[i] = ordem[j];
                ordem[j] = t;
            }
        }
    }

    printf("\nParticipantes (maiores saldos):\n");
    for (i = 0; i < k; i++) {
        v = &b->clientes[ordem[i]];
        printf("%s | R$ %.2f | titulos: %s\n", v->nome, saldoCliente(v), v->titulos ? "sim" : "nao");
        if (!v->titulos) {
            ok = 0;
        }
    }
    if (!ok) {
        printf("Sorteio cancelado: todos os participantes precisam ter adquirido titulos.\n");
        free(ordem);
        return;
    }

    i = ordem[fibonacci(20 + (int)(time(NULL) % 10)) % k];
    free(ordem);
    v = &b->clientes[i];
    salario = buscarConta(v, SALARIO);
    if (salario == NULL) {
        printf("Sorteio cancelado: o contemplado nao tem conta salario.\n");
        return;
    }
    premio = salario->saldo * MULT_PREMIO;
    if (premio > 0) {
        salario->saldo = salario->saldo + premio;
        registrar(v, premio, 'D');
    }
    b->ganhador = i;
    b->premio = premio;
    printf("\nContemplado: %s | %d-%02d | premio R$ %.2f\n", v->nome, v->numero, SALARIO, premio);
}

/* ---------- historico do banco ---------- */

void mostrarHistoricoBanco(Banco *b) {
    int i, j;
    float total = 0, limite;
    Cliente *c;

    for (i = 0; i < b->qtdClientes; i++) {
        total = total + saldoCliente(&b->clientes[i]);
    }
    printf("\n== Saldo Total do banco ==\n%d | R$ %.2f\n", b->qtdClientes, total);

    printf("\n== Clientes Aptos a Emprestimos ==\n");
    for (i = 0; i < b->qtdClientes; i++) {
        c = &b->clientes[i];
        limite = limiteEmprestimo(c);
        if (limite > 0) {
            printf("%s|%d-%02d|%.2f\n", c->nome, c->numero, CORRENTE, limite);
        }
    }

    printf("\n== Clientes com Saldo Negativo na Conta Salario ==\n");
    for (i = 0; i < b->qtdClientes; i++) {
        c = &b->clientes[i];
        for (j = 0; j < c->qtdContas; j++) {
            if (c->contas[j].tipo != POUPANCA && c->contas[j].saldo < 0) {
                printf("%s|%d-%02d|%.2f\n", c->nome, c->numero, c->contas[j].tipo, c->contas[j].saldo);
            }
        }
    }

    printf("\n== Clientes Aptos a Compra de Titulos ==\n");
    for (i = 0; i < b->qtdClientes; i++) {
        c = &b->clientes[i];
        if (podeTitulos(c)) {
            printf("%s|%d-%02d|%.2f\n", c->nome, c->numero, SALARIO, buscarConta(c, SALARIO)->saldo);
        }
    }

    printf("\n== Clientes Contemplado no Sorteio ==\n");
    if (b->ganhador >= 0) {
        c = &b->clientes[b->ganhador];
        printf("%s|%d-%02d|%.2f\n", c->nome, c->numero, SALARIO, b->premio);
    }
}
