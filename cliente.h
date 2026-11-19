#ifndef CLIENTE_H
#define CLIENTE_H

#include <stdio.h>

#define CORRENTE 1
#define SALARIO 2
#define POUPANCA 3

#define MAX_CONTAS 2
#define EXTRATO_QTD 5

#define MULT_EMPRESTIMO 10
#define PERC_EMPRESTIMO 0.25f
#define TITULO_MIN 1000.0f
#define TITULO_MAX 1500.0f


typedef struct {
    float valor;
    char op;                    /* 'S' saque, 'D' deposito, 'E' emprestimo */
} Transacao;

typedef struct {
    int tipo;
    float saldo;
} Conta;

typedef struct {
    int numero;                 /* 1000, 1001, ... : a conta e numero-tipo, ex.: 1000-01 */
    char nome[50];
    Conta contas[MAX_CONTAS];
    int qtdContas;
    int emprestimo;             /* 0 = ainda nao pegou, 1 = ja pegou */
    int titulos;                /* 0 = nao tem, 1 = adquiriu titulos de capitalizacao */
    Transacao *historico;
    int qtdHistorico;
    int capHistorico;
} Cliente;

void textoConta(char *dest, int numero, int tipo);
void mostrarMenuCliente(void);
void liberarCliente(Cliente *c);

int combinacaoValida(int tipo1, int tipo2);
int podeAdicionarConta(const Cliente *c, int tipoNovo);
void adicionarConta(Cliente *c, int tipoNovo, float saldoInicial);
int temConta(const Cliente *c, int tipo);
float saldoDoTipo(const Cliente *c, int tipo);
Conta *buscarConta(Cliente *c, int tipo);
float saldoCliente(const Cliente *c);

void lerCliente(Cliente *c, int numero);
void lerContas(Cliente *c);

int registrarTransacao(Cliente *c, float valor, char op);
int depositar(Cliente *c, Conta *conta, float valor);
int sacar(Cliente *c, Conta *conta, float valor);
void mostrarExtrato(const Cliente *c);

int podeEmprestimo(const Cliente *c);
float limiteEmprestimo(const Cliente *c);
int pegarEmprestimo(Cliente *c, float valor);

int podeComprarTitulos(const Cliente *c);
int comprarTitulos(Cliente *c);

void sessaoCliente(Cliente *c, Conta *conta);

void escreverCliente(FILE *f, const Cliente *c);
int lerClienteArquivo(FILE *f, Cliente *c);

#endif
