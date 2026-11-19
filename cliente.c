#include <stdio.h>
#include <string.h>
#include "meu_banco.h"

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
