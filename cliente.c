#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cliente.h"
#include "entrada.h"

void textoConta(char *dest, int numero, int tipo) {
    snprintf(dest, 16, "%d-%02d", numero, tipo);
}

void mostrarMenuCliente(void) {
    printf("\n===Cliente===\n");
    printf("1 - Depositar\n");
    printf("2 - Sacar\n");
    printf("3 - Solicitar emprestimo\n");
    printf("4 - Adquirir titulos de capitalizacao\n");
    printf("5 - Solicitar extrato\n");
    printf("0 - Voltar\n");
}

void liberarCliente(Cliente *c) {
    free(c->historico);
    c->historico = NULL;
    c->qtdHistorico = 0;
    c->capHistorico = 0;
}

int combinacaoValida(int tipo1, int tipo2) {
    if (tipo1 == tipo2) {
        return 0;
    }

    if ((tipo1 == CORRENTE && tipo2 == SALARIO) || (tipo1 == SALARIO && tipo2 == CORRENTE)) {
        return 0;
    }

    return 1;
}

int podeAdicionarConta(const Cliente *c, int tipoNovo) {
    if (c->qtdContas >= MAX_CONTAS) {
        return 0;
    }
    if (c->qtdContas == 1) {
        return combinacaoValida(c->contas[0].tipo, tipoNovo);
    }
    return 1;
}

void adicionarConta(Cliente *c, int tipoNovo, float saldoInicial) {
    c->contas[c->qtdContas].tipo = tipoNovo;
    c->contas[c->qtdContas].saldo = saldoInicial;
    c->qtdContas = c->qtdContas + 1;
}

int temConta(const Cliente *c, int tipo) {
    int i;
    for (i = 0; i < c->qtdContas; i++) {
        if (c->contas[i].tipo == tipo) {
            return 1;
        }
    }
    return 0;
}

float saldoDoTipo(const Cliente *c, int tipo) {
    int i;
    for (i = 0; i < c->qtdContas; i++) {
        if (c->contas[i].tipo == tipo) {
            return c->contas[i].saldo;
        }
    }
    return 0;
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

float saldoCliente(const Cliente *c) {
    int i;
    float total = 0;
    for (i = 0; i < c->qtdContas; i++) {
        total = total + c->contas[i].saldo;
    }
    return total;
}

void lerCliente(Cliente *c, int numero) {
    c->nome[0] = '\0';
    printf("Nome do cliente: ");
    if (scanf(" %49[^\n]", c->nome) != 1) {
        limparBuffer();
    }

    c->numero = numero;
    c->qtdContas = 0;
    c->emprestimo = 0;
    c->titulos = 0;
    c->historico = NULL;
    c->qtdHistorico = 0;
    c->capHistorico = 0;
}

void lerContas(Cliente *c) {
    int tipo;
    int mais = 1;

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
            adicionarConta(c, tipo, 0);
        }

        if (c->qtdContas < MAX_CONTAS && c->qtdContas > 0) {
            printf("Adicionar outra conta? (1-sim, 0-nao): ");
            if (!lerInteiro(&mais)) {
                mais = 0;
            }
        }
    }
}

int registrarTransacao(Cliente *c, float valor, char op) {
    if (c->qtdHistorico == c->capHistorico) {
        int novaCapacidade = (c->capHistorico == 0) ? 4 : c->capHistorico * 2;
        Transacao *novo = realloc(c->historico, novaCapacidade * sizeof(Transacao));
        if (novo == NULL) {
            return 0;
        }
        c->historico = novo;
        c->capHistorico = novaCapacidade;
    }

    c->historico[c->qtdHistorico].valor = valor;
    c->historico[c->qtdHistorico].op = op;
    c->qtdHistorico = c->qtdHistorico + 1;
    return 1;
}

int depositar(Cliente *c, Conta *conta, float valor) {
    if (valor <= 0) {
        return 0;
    }
    if (!registrarTransacao(c, valor, 'D')) {
        return 0;
    }
    conta->saldo = conta->saldo + valor;
    return 1;
}

int sacar(Cliente *c, Conta *conta, float valor) {
    if (valor <= 0) {
        return 0;
    }
    if (conta->tipo != CORRENTE && valor > conta->saldo) {
        return 0;
    }
    if (!registrarTransacao(c, valor, 'S')) {
        return 0;
    }
    conta->saldo = conta->saldo - valor;
    return 1;
}

void mostrarExtrato(const Cliente *c) {
    int i;
    int inicio = c->qtdHistorico - EXTRATO_QTD;

    if (inicio < 0) {
        inicio = 0;
    }
    if (c->qtdHistorico == 0) {
        printf("Nenhuma transacao.\n");
    }
    for (i = inicio; i < c->qtdHistorico; i++) {
        printf("%.2f :%c\n", c->historico[i].valor, c->historico[i].op);
    }
}

int podeEmprestimo(const Cliente *c) {
    if (temConta(c, CORRENTE) && temConta(c, POUPANCA) && c->emprestimo == 0 && saldoDoTipo(c, POUPANCA) > 0) {
        return 1;
    }
    return 0;
}

float limiteEmprestimo(const Cliente *c) {
    return saldoDoTipo(c, POUPANCA) * MULT_EMPRESTIMO * PERC_EMPRESTIMO;
}

int pegarEmprestimo(Cliente *c, float valor) {
    Conta *corrente = buscarConta(c, CORRENTE);

    if (!podeEmprestimo(c) || corrente == NULL || valor <= 0 || valor > limiteEmprestimo(c)) {
        return 0;
    }
    if (!registrarTransacao(c, valor, 'E')) {
        return 0;
    }
    corrente->saldo = corrente->saldo + valor;
    c->emprestimo = 1;
    return 1;
}

int podeComprarTitulos(const Cliente *c) {
    float saldo = saldoDoTipo(c, SALARIO);
    return temConta(c, SALARIO) && saldo >= TITULO_MIN && saldo <= TITULO_MAX;
}

int comprarTitulos(Cliente *c) {
    if (!podeComprarTitulos(c)) {
        return 0;
    }
    c->titulos = 1;
    return 1;
}

void sessaoCliente(Cliente *c, Conta *conta) {
    int op = -1;
    float valor;
    char texto[16];
    Conta *corrente = buscarConta(c, CORRENTE);

    textoConta(texto, c->numero, conta->tipo);
    printf("\nBem-vindo(a), %s | conta %s | saldo R$ %.2f\n", c->nome, texto, conta->saldo);
    if (corrente != NULL && corrente->saldo < 0) {
        printf("ALERTA: sua conta corrente esta com saldo negativo (R$ %.2f).\n", corrente->saldo);
    }

    while (op != 0 && !feof(stdin)) {
        mostrarMenuCliente();
        printf("opcao: ");
        if (!lerInteiro(&op)) {
            op = -1;
            printf("opcao invalida.\n");
            continue;
        }

        switch (op) {
        case 1:
            printf("Valor do deposito: ");
            if (lerReal(&valor) && depositar(c, conta, valor)) {
                printf("Deposito realizado. Saldo: R$ %.2f\n", conta->saldo);
            } else {
                printf("Valor invalido.\n");
            }
            break;

        case 2:
            printf("Valor do saque: ");
            if (lerReal(&valor) && sacar(c, conta, valor)) {
                printf("Saque realizado. Saldo: R$ %.2f\n", conta->saldo);
            } else {
                printf("Saque nao permitido (valor invalido ou saldo insuficiente).\n");
            }
            break;

        case 3:
            if (!podeEmprestimo(c)) {
                printf("Emprestimo negado: precisa de conta corrente e poupanca com saldo, e so e permitido um emprestimo por cliente.\n");
            } else {
                printf("Limite do emprestimo: R$ %.2f\n", limiteEmprestimo(c));
                printf("Valor desejado: ");
                if (lerReal(&valor) && pegarEmprestimo(c, valor)) {
                    printf("Emprestimo aprovado. Valor creditado na conta corrente.\n");
                } else {
                    printf("Valor invalido (precisa ser maior que 0 e ate o limite).\n");
                }
            }
            break;

        case 4:
            if (c->titulos) {
                printf("Voce ja possui titulos de capitalizacao.\n");
            } else if (comprarTitulos(c)) {
                printf("Titulos de capitalizacao adquiridos.\n");
            } else {
                printf("Para adquirir titulos e preciso ter conta salario com saldo entre R$ %.2f e R$ %.2f.\n", TITULO_MIN, TITULO_MAX);
            }
            break;

        case 5:
            mostrarExtrato(c);
            break;

        case 0:
            break;

        default:
            printf("opcao invalida.\n");
            break;
        }
    }
}

void escreverCliente(FILE *f, const Cliente *c) {
    int j;

    fprintf(f, "%d %d %d %d %d\n", c->numero, c->emprestimo, c->titulos, c->qtdContas, c->qtdHistorico);
    fprintf(f, "%s\n", c->nome);
    for (j = 0; j < c->qtdContas; j++) {
        fprintf(f, "%d %.2f\n", c->contas[j].tipo, c->contas[j].saldo);
    }
    for (j = 0; j < c->qtdHistorico; j++) {
        fprintf(f, "%.2f %c\n", c->historico[j].valor, c->historico[j].op);
    }
}

int lerClienteArquivo(FILE *f, Cliente *c) {
    int qtdHist, j, tipo;
    float saldo, valor;
    char op;

    c->historico = NULL;
    c->qtdHistorico = 0;
    c->capHistorico = 0;

    if (fscanf(f, "%d %d %d %d %d ", &c->numero, &c->emprestimo, &c->titulos, &c->qtdContas, &qtdHist) != 5) {
        return 0;
    }
    if (c->qtdContas < 0 || c->qtdContas > MAX_CONTAS || qtdHist < 0) {
        return 0;
    }
    if (fgets(c->nome, sizeof(c->nome), f) == NULL) {
        return 0;
    }
    c->nome[strcspn(c->nome, "\r\n")] = '\0';

    for (j = 0; j < c->qtdContas; j++) {
        if (fscanf(f, "%d %f", &tipo, &saldo) != 2) {
            return 0;
        }
        c->contas[j].tipo = tipo;
        c->contas[j].saldo = saldo;
    }
    for (j = 0; j < qtdHist; j++) {
        if (fscanf(f, "%f %c", &valor, &op) != 2 || !registrarTransacao(c, valor, op)) {
            liberarCliente(c);
            return 0;
        }
    }
    return 1;
}
