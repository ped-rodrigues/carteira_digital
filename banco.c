#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "meu_banco.h"

/* ---------- menu e entrada ---------- */

void mostrarMenu(void) {
    printf("\n===Banco===\n");
    printf("1 - Abrir banco\n");
    printf("2 - Ver historico de transacoes\n");
    printf("3 - Cadastrar cliente\n");
    printf("4 - Listar clientes\n");
    printf("5 - Pedir emprestimo\n");
    printf("6 - Relatorios\n");
    printf("s - Sair do banco\n");
}

/* joga fora o que sobrou digitado ate o Enter */
void limparBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }
}

/* ---------- ciclo de vida do banco ---------- */

/* reserva (malloc) o vetor de clientes; devolve 1 se deu certo, 0 se faltou memoria */
int iniciarBanco(Banco *b) {
    b->clientes = malloc(CAPACIDADE_INICIAL * sizeof(Cliente));
    if (b->clientes == NULL) {
        return 0;
    }
    b->qtdClientes = 0;
    b->capacidade = CAPACIDADE_INICIAL;
    b->aberto = 0;
    return 1;
}

/* copia o cliente para o banco; se o vetor estiver cheio, dobra o tamanho (realloc) */
int adicionarCliente(Banco *b, const Cliente *c) {
    if (b->qtdClientes == b->capacidade) {
        int novaCapacidade = b->capacidade * 2;
        Cliente *novo = realloc(b->clientes, novaCapacidade * sizeof(Cliente));
        if (novo == NULL) {
            return 0; /* o vetor antigo continua valido */
        }
        b->clientes = novo;
        b->capacidade = novaCapacidade;
    }
    b->clientes[b->qtdClientes] = *c;
    b->qtdClientes = b->qtdClientes + 1;
    return 1;
}

/* devolve (free) o historico de um cliente: cada texto e depois o vetor de textos */
void liberarCliente(Cliente *c) {
    int j;

    for (j = 0; j < c->qtdHistorico; j++) {
        free(c->historico[j]);
    }
    free(c->historico);

    c->historico = NULL;
    c->qtdHistorico = 0;
    c->capHistorico = 0;
}

/* devolve tudo que foi pedido com malloc: o historico de cada cliente e o vetor de clientes */
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

/* ---------- regras de contas ---------- */

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

const char *nomeTipo(int tipo) {
    if (tipo == CORRENTE) {
        return "corrente";
    }
    if (tipo == SALARIO) {
        return "salario";
    }
    if (tipo == POUPANCA) {
        return "poupanca";
    }
    return "desconhecido";
}

void adicionarConta(Cliente *c, int numero, int tipoNovo, float saldoInicial) {
    char msg[TAM_TEXTO];

    c->contas[c->qtdContas].numero = numero;
    c->contas[c->qtdContas].tipo = tipoNovo;
    c->contas[c->qtdContas].saldo = saldoInicial;
    c->qtdContas = c->qtdContas + 1;

    snprintf(msg, TAM_TEXTO, "Abriu conta %d (%s) com saldo %.2f", numero, nomeTipo(tipoNovo), saldoInicial);
    registrarHistorico(c, msg);
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

/* ---------- cadastro ---------- */

/* preenche o cliente apontado por c (sem historico ainda: NULL, 0 e 0) */
void lerCliente(Cliente *c) {
    printf("Nome do cliente: ");
    scanf(" %49[^\n]", c->nome);

    c->qtdContas = 0;
    c->emprestimo = 0;
    c->historico = NULL;
    c->qtdHistorico = 0;
    c->capHistorico = 0;
}

void lerContas(Cliente *c, int primeiroNumero) {
    int tipo;
    float saldo;
    int mais = 1;

    /* repete enquanto cabem contas; o cliente precisa ter pelo menos 1 */
    while (c->qtdContas < MAX_CONTAS && (mais || c->qtdContas == 0) && !feof(stdin)) {
        printf("Tipo de conta (1-corrente, 2-salario, 3-poupanca): ");
        if (scanf("%d", &tipo) != 1) {
            limparBuffer();
            tipo = 0;
        }

        if (tipo < CORRENTE || tipo > POUPANCA) {
            printf("Tipo invalido.\n");
        } else if (!podeAdicionarConta(c, tipo)) {
            printf("Esse cliente nao pode ter essa conta.\n");
        } else {
            printf("Saldo inicial: ");
            if (scanf("%f", &saldo) != 1) {
                limparBuffer();
                printf("Saldo invalido.\n");
            } else {
                adicionarConta(c, primeiroNumero + c->qtdContas, tipo, saldo);
            }
        }

        if (c->qtdContas < MAX_CONTAS && c->qtdContas > 0) {
            printf("Adicionar outra conta? (1-sim, 0-nao): ");
            if (scanf("%d", &mais) != 1) {
                limparBuffer();
                mais = 0;
            }
        }
    }
}

/* ---------- emprestimo ---------- */

/* corrente + poupanca, poupanca com saldo e ainda nao pegou emprestimo */
int podeEmprestimo(const Cliente *c) {
    if (temConta(c, CORRENTE) && temConta(c, POUPANCA) && c->emprestimo == 0 && saldoDoTipo(c, POUPANCA) > 0) {
        return 1;
    }
    return 0;
}

/* 25% de 10 vezes o saldo da poupanca */
float limiteEmprestimo(const Cliente *c) {
    return saldoDoTipo(c, POUPANCA) * MULT_EMPRESTIMO * PERC_EMPRESTIMO;
}

/* credita o valor na conta corrente e marca que o cliente ja pegou (a conferencia do limite e feita antes) */
void pegarEmprestimo(Cliente *c, float valor) {
    int i;
    char msg[TAM_TEXTO];

    for (i = 0; i < c->qtdContas; i++) {
        if (c->contas[i].tipo == CORRENTE) {
            c->contas[i].saldo = c->contas[i].saldo + valor;
        }
    }
    c->emprestimo = 1;

    snprintf(msg, TAM_TEXTO, "Pegou emprestimo de %.2f (creditado na corrente)", valor);
    registrarHistorico(c, msg);
}

/* ---------- historico ---------- */

/* guarda uma copia do texto (malloc); o vetor de textos cresce com realloc. Devolve 1 se deu certo */
int registrarHistorico(Cliente *c, const char *texto) {
    char *copia;

    if (c->qtdHistorico == c->capHistorico) {
        int novaCapacidade = (c->capHistorico == 0) ? 4 : c->capHistorico * 2;
        char **novo = realloc(c->historico, novaCapacidade * sizeof(char *));
        if (novo == NULL) {
            return 0;
        }
        c->historico = novo;
        c->capHistorico = novaCapacidade;
    }

    copia = malloc(strlen(texto) + 1);
    if (copia == NULL) {
        return 0;
    }
    strcpy(copia, texto);

    c->historico[c->qtdHistorico] = copia;
    c->qtdHistorico = c->qtdHistorico + 1;
    return 1;
}

void mostrarHistorico(const Banco *b) {
    int i, j;

    if (b->qtdClientes == 0) {
        printf("Nenhum cliente cadastrado.\n");
        return;
    }

    for (i = 0; i < b->qtdClientes; i++) {
        printf("\n%s:\n", b->clientes[i].nome);
        if (b->clientes[i].qtdHistorico == 0) {
            printf("  (sem transacoes)\n");
        }
        for (j = 0; j < b->clientes[i].qtdHistorico; j++) {
            printf("  %d. %s\n", j + 1, b->clientes[i].historico[j]);
        }
    }
}

/* ---------- listagens e relatorios ---------- */

void listarClientes(const Banco *b) {
    int i, j;

    if (b->qtdClientes == 0) {
        printf("Nenhum cliente cadastrado.\n");
        return;
    }

    for (i = 0; i < b->qtdClientes; i++) {
        printf("%d) %s | %d conta(s) | emprestimo: %s\n", i + 1, b->clientes[i].nome,
               b->clientes[i].qtdContas, b->clientes[i].emprestimo ? "sim" : "nao");
        for (j = 0; j < b->clientes[i].qtdContas; j++) {
            printf("     conta %d | %s | saldo %.2f\n", b->clientes[i].contas[j].numero,
                   nomeTipo(b->clientes[i].contas[j].tipo), b->clientes[i].contas[j].saldo);
        }
    }
}

float saldoTotal(const Banco *b) {
    int i, j;
    float total = 0;

    for (i = 0; i < b->qtdClientes; i++) {
        for (j = 0; j < b->clientes[i].qtdContas; j++) {
            total = total + b->clientes[i].contas[j].saldo;
        }
    }
    return total;
}

void mostrarRelatorios(const Banco *b) {
    int i, j, achou;

    printf("\n--- Saldo total do banco ---\n");
    printf("R$ %.2f\n", saldoTotal(b));

    printf("\n--- Clientes aptos a emprestimo ---\n");
    achou = 0;
    for (i = 0; i < b->qtdClientes; i++) {
        if (podeEmprestimo(&b->clientes[i])) {
            printf("%s (limite R$ %.2f)\n", b->clientes[i].nome, limiteEmprestimo(&b->clientes[i]));
            achou = 1;
        }
    }
    if (!achou) {
        printf("(nenhum)\n");
    }

    printf("\n--- Clientes com saldo negativo ---\n");
    achou = 0;
    for (i = 0; i < b->qtdClientes; i++) {
        for (j = 0; j < b->clientes[i].qtdContas; j++) {
            if (b->clientes[i].contas[j].saldo < 0) {
                printf("%s | conta %d (%s) | R$ %.2f\n", b->clientes[i].nome,
                       b->clientes[i].contas[j].numero, nomeTipo(b->clientes[i].contas[j].tipo),
                       b->clientes[i].contas[j].saldo);
                achou = 1;
            }
        }
    }
    if (!achou) {
        printf("(nenhum)\n");
    }

    printf("\n--- Contas salario com saldo entre %.0f e %.0f ---\n", SALARIO_MIN, SALARIO_MAX);
    achou = 0;
    for (i = 0; i < b->qtdClientes; i++) {
        for (j = 0; j < b->clientes[i].qtdContas; j++) {
            if (b->clientes[i].contas[j].tipo == SALARIO && b->clientes[i].contas[j].saldo >= SALARIO_MIN &&
                b->clientes[i].contas[j].saldo <= SALARIO_MAX) {
                printf("%s | conta %d | R$ %.2f\n", b->clientes[i].nome, b->clientes[i].contas[j].numero,
                       b->clientes[i].contas[j].saldo);
                achou = 1;
            }
        }
    }
    if (!achou) {
        printf("(nenhuma)\n");
    }
}
