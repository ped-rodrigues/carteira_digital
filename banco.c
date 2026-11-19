#include <stdio.h>
#include <stdlib.h>
#include "meu_banco.h"
#include "entrada.h"

void mostrarMenu(void) {
    printf("\n===Banco===\n");
    printf("1 - Abrir banco\n");
    printf("2 - Consultar historico\n");
    printf("3 - Efetuar sorteio\n");
    printf("4 - Acessar conta\n");
    printf("5 - Cadastrar cliente\n");
    printf("s - Sair\n");
}

int iniciarBanco(Banco *b) {
    b->clientes = malloc(CAPACIDADE_INICIAL * sizeof(Cliente));
    if (b->clientes == NULL) {
        return 0;
    }
    b->qtdClientes = 0;
    b->capacidade = CAPACIDADE_INICIAL;
    b->aberto = 0;
    b->ganhador = -1;
    b->premio = 0;
    return 1;
}

int adicionarCliente(Banco *b, const Cliente *c) {
    if (b->qtdClientes == b->capacidade) {
        int novaCapacidade = b->capacidade * 2;
        Cliente *novo = realloc(b->clientes, novaCapacidade * sizeof(Cliente));
        if (novo == NULL) {
            return 0;
        }
        b->clientes = novo;
        b->capacidade = novaCapacidade;
    }
    b->clientes[b->qtdClientes] = *c;
    b->qtdClientes = b->qtdClientes + 1;
    return 1;
}

void liberarBanco(Banco *b) {
    int i;

    for (i = 0; i < b->qtdClientes; i++) {
        liberarCliente(&b->clientes[i]);
    }
    free(b->clientes);

    b->clientes = NULL;
    b->qtdClientes = 0;
    b->capacidade = 0;
}

Cliente *buscarCliente(Banco *b, int numero) {
    int i;
    for (i = 0; i < b->qtdClientes; i++) {
        if (b->clientes[i].numero == numero) {
            return &b->clientes[i];
        }
    }
    return NULL;
}

int salvarBanco(const Banco *b, const char *arquivo) {
    FILE *f = fopen(arquivo, "w");
    int i;

    if (f == NULL) {
        return 0;
    }

    fprintf(f, "%d %d %.2f\n", b->qtdClientes, b->ganhador, b->premio);
    for (i = 0; i < b->qtdClientes; i++) {
        escreverCliente(f, &b->clientes[i]);
    }

    fclose(f);
    return 1;
}

int carregarBanco(Banco *b, const char *arquivo) {
    FILE *f = fopen(arquivo, "r");
    int n, i, ganhador;
    float premio;

    if (f == NULL) {
        return 0;
    }
    if (fscanf(f, "%d %d %f", &n, &ganhador, &premio) != 3 || n < 0) {
        fclose(f);
        return 0;
    }

    for (i = 0; i < n; i++) {
        Cliente c;

        if (!lerClienteArquivo(f, &c)) {
            fclose(f);
            liberarBanco(b);
            iniciarBanco(b);
            return 0;
        }
        if (!adicionarCliente(b, &c)) {
            liberarCliente(&c);
            fclose(f);
            liberarBanco(b);
            iniciarBanco(b);
            return 0;
        }
    }
    fclose(f);

    b->ganhador = (ganhador >= 0 && ganhador < b->qtdClientes) ? ganhador : -1;
    b->premio = premio;
    return 1;
}

int cadastrarClientes(Banco *b) {
    int n, i;
    int cadastrados = 0;

    printf("Quantos clientes deseja cadastrar? ");
    if (!lerInteiro(&n) || n < 1) {
        printf("Quantidade invalida.\n");
        return 0;
    }

    for (i = 0; i < n && !feof(stdin); i++) {
        Cliente novo;

        printf("\nCliente %d de %d\n", i + 1, n);
        lerCliente(&novo, NUMERO_INICIAL + b->qtdClientes);
        if (novo.nome[0] == '\0') {
            break;
        }
        lerContas(&novo);
        if (novo.qtdContas == 0) {
            liberarCliente(&novo);
            break;
        }
        if (!adicionarCliente(b, &novo)) {
            liberarCliente(&novo);
            printf("Sem memoria para cadastrar o cliente.\n");
            break;
        }
        printf("Cliente cadastrado. Numero: %d\n", novo.numero);
        cadastrados++;
    }
    return cadastrados;
}

void abrirBanco(Banco *b) {
    if (b->aberto) {
        printf("O banco ja esta aberto.\n");
        return;
    }

    if (carregarBanco(b, ARQUIVO_BANCO) && b->qtdClientes > 0) {
        printf("Banco aberto. %d cliente(s) carregado(s) de %s.\n", b->qtdClientes, ARQUIVO_BANCO);
    } else {
        printf("Nenhum dado salvo. Cadastre os clientes (todas as contas comecam com saldo zero).\n");
        if (cadastrarClientes(b) == 0) {
            printf("Nenhum cliente cadastrado. O banco continua fechado.\n");
            return;
        }
        printf("Banco aberto.\n");
    }
    b->aberto = 1;
}

float saldoTotal(const Banco *b) {
    int i;
    float total = 0;

    for (i = 0; i < b->qtdClientes; i++) {
        total = total + saldoCliente(&b->clientes[i]);
    }
    return total;
}

void mostrarHistoricoBanco(const Banco *b) {
    int i, j, achou;
    char texto[16];

    printf("\n== Saldo Total do banco ==\n");
    printf("%d | R$ %.2f\n", b->qtdClientes, saldoTotal(b));

    printf("\n== Clientes Aptos a Emprestimos ==\n");
    achou = 0;
    for (i = 0; i < b->qtdClientes; i++) {
        if (podeEmprestimo(&b->clientes[i])) {
            textoConta(texto, b->clientes[i].numero, CORRENTE);
            printf("%s|%s|%.2f\n", b->clientes[i].nome, texto, limiteEmprestimo(&b->clientes[i]));
            achou = 1;
        }
    }
    if (!achou) {
        printf("(nenhum)\n");
    }

    printf("\n== Clientes com Saldo Negativo na Conta Salario ==\n");
    achou = 0;
    for (i = 0; i < b->qtdClientes; i++) {
        for (j = 0; j < b->clientes[i].qtdContas; j++) {
            int tipo = b->clientes[i].contas[j].tipo;
            if ((tipo == CORRENTE || tipo == SALARIO) && b->clientes[i].contas[j].saldo < 0) {
                textoConta(texto, b->clientes[i].numero, tipo);
                printf("%s|%s|%.2f\n", b->clientes[i].nome, texto, b->clientes[i].contas[j].saldo);
                achou = 1;
            }
        }
    }
    if (!achou) {
        printf("(nenhum)\n");
    }

    printf("\n== Clientes Aptos a Compra de Titulos ==\n");
    achou = 0;
    for (i = 0; i < b->qtdClientes; i++) {
        if (podeComprarTitulos(&b->clientes[i])) {
            textoConta(texto, b->clientes[i].numero, SALARIO);
            printf("%s|%s|%.2f\n", b->clientes[i].nome, texto, saldoDoTipo(&b->clientes[i], SALARIO));
            achou = 1;
        }
    }
    if (!achou) {
        printf("(nenhum)\n");
    }

    printf("\n== Clientes Contemplado no Sorteio ==\n");
    if (b->ganhador >= 0 && b->ganhador < b->qtdClientes) {
        textoConta(texto, b->clientes[b->ganhador].numero, SALARIO);
        printf("%s|%s|%.2f\n", b->clientes[b->ganhador].nome, texto, b->premio);
    } else {
        printf("(nenhum)\n");
    }
}
