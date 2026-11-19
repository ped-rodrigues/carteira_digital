#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "sorteio.h"

unsigned long long fibonacci(int n) {
    unsigned long long a = 0, b = 1, t;
    int i;

    for (i = 0; i < n; i++) {
        t = a + b;
        a = b;
        b = t;
    }
    return a;
}

int sortearPosicao(int qtd) {
    int n = 20 + (int)(time(NULL) % 30);
    return (int)(fibonacci(n) % qtd);
}

int efetuarSorteio(Banco *b) {
    int n = b->qtdClientes;
    int k = (n < MAX_PARTICIPANTES) ? n : MAX_PARTICIPANTES;
    int top[MAX_PARTICIPANTES];
    int *usado;
    int i, p, melhor, pos;
    int faltam = 0;
    Cliente *vencedor;
    Conta *salario;
    float premio;
    char texto[16];

    if (n == 0) {
        printf("Nenhum cliente cadastrado.\n");
        return 0;
    }

    usado = calloc(n, sizeof(int));
    if (usado == NULL) {
        printf("Sem memoria para o sorteio.\n");
        return 0;
    }
    for (p = 0; p < k; p++) {
        melhor = -1;
        for (i = 0; i < n; i++) {
            if (!usado[i] && (melhor == -1 || saldoCliente(&b->clientes[i]) > saldoCliente(&b->clientes[melhor]))) {
                melhor = i;
            }
        }
        usado[melhor] = 1;
        top[p] = melhor;
    }
    free(usado);

    printf("\nParticipantes (maiores saldos):\n");
    for (p = 0; p < k; p++) {
        printf("%s | R$ %.2f | titulos: %s\n", b->clientes[top[p]].nome, saldoCliente(&b->clientes[top[p]]),
               b->clientes[top[p]].titulos ? "sim" : "nao");
        if (!b->clientes[top[p]].titulos) {
            faltam = 1;
        }
    }
    if (faltam) {
        printf("Sorteio cancelado: todos os participantes precisam ter adquirido titulos de capitalizacao.\n");
        return 0;
    }

    pos = sortearPosicao(k);
    vencedor = &b->clientes[top[pos]];
    salario = buscarConta(vencedor, SALARIO);
    if (salario == NULL) {
        printf("Sorteio cancelado: o contemplado nao tem conta salario.\n");
        return 0;
    }

    premio = salario->saldo * MULT_PREMIO;
    if (premio > 0) {
        if (!registrarTransacao(vencedor, premio, 'D')) {
            printf("Sem memoria para registrar o premio.\n");
            return 0;
        }
        salario->saldo = salario->saldo + premio;
    }
    b->ganhador = top[pos];
    b->premio = premio;

    textoConta(texto, vencedor->numero, SALARIO);
    printf("\nContemplado: %s | %s | premio R$ %.2f\n", vencedor->nome, texto, premio);
    return 1;
}
