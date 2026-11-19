#ifndef SORTEIO_H
#define SORTEIO_H

#include "meu_banco.h"

#define MAX_PARTICIPANTES 5
#define MULT_PREMIO 10

unsigned long long fibonacci(int n);
int sortearPosicao(int qtd);
int efetuarSorteio(Banco *b);

#endif
