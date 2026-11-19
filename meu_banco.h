#ifndef MEU_BANCO_H
#define MEU_BANCO_H

#include "cliente.h"

#define CAPACIDADE_INICIAL 2
#define NUMERO_INICIAL 1000
#define ARQUIVO_BANCO "banco.dat"

typedef struct {
    Cliente *clientes;
    int qtdClientes;
    int capacidade;
    int aberto;
    int ganhador;               /* indice do ultimo contemplado no sorteio (-1 = nenhum) */
    float premio;
} Banco;

void mostrarMenu(void);

int iniciarBanco(Banco *b);
int adicionarCliente(Banco *b, const Cliente *c);
void liberarBanco(Banco *b);
Cliente *buscarCliente(Banco *b, int numero);

int salvarBanco(const Banco *b, const char *arquivo);
int carregarBanco(Banco *b, const char *arquivo);

int cadastrarClientes(Banco *b);
void abrirBanco(Banco *b);

float saldoTotal(const Banco *b);
void mostrarHistoricoBanco(const Banco *b);

#endif
