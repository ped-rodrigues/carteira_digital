#include <stdio.h>
#include <stdlib.h>
#include "meu_banco.h"

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
