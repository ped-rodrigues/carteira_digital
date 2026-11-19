#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "meu_banco.h"

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
