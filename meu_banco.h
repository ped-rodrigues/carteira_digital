#ifndef MEU_BANCO_H
#define MEU_BANCO_H

#define CORRENTE 1
#define SALARIO 2
#define POUPANCA 3

#define MAX_CONTAS 2
#define MAX_CLIENTES 50
#define MAX_HISTORICO 20
#define TAM_TEXTO 80


typedef struct {
    int numero;
    int tipo;
    float saldo;
} Conta;

typedef struct {
    char nome[50];
    Conta contas[MAX_CONTAS];
    int qtdContas;
    int emprestimo;
    char historico[MAX_HISTORICO][TAM_TEXTO];
    int qtdHistorico;
} Cliente;

typedef struct {
    Cliente clientes[MAX_CLIENTES];
    int qtdClientes;
    int aberto;
} Banco;

void mostrarMenu(void);

int combinacaoValida(int tipo1, int tipo2);

Cliente lerCliente(void);

int podeAdicionarConta(Cliente c, int tipoNovo);

Cliente adicionarConta(Cliente c, int numero, int tipoNovo, float saldoInicial);

Cliente lerContas(Cliente c, int primeiroNumero);

int temConta(Cliente c, int tipo);

float saldoDoTipo(Cliente c, int tipo);

#endif
