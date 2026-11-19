#ifndef MEU_BANCO_H
#define MEU_BANCO_H

#define CORRENTE 1
#define SALARIO 2
#define POUPANCA 3

#define MAX_CONTAS 2
#define EXTRATO_QTD 5
#define PARTICIPANTES 5
#define NUMERO_INICIAL 1000
#define ARQUIVO "banco.dat"

#define MULT_EMPRESTIMO 10
#define PERC_EMPRESTIMO 0.25f
#define MULT_PREMIO 10
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
    int numero;                 /* 1000, 1001, ... ; a conta e numero-tipo, ex.: 1000-01 */
    char nome[50];
    Conta contas[MAX_CONTAS];
    int qtdContas;
    int emprestimo;             /* 0 = ainda nao pegou, 1 = ja pegou */
    int titulos;                /* 1 = adquiriu titulos de capitalizacao */
    Transacao extrato[EXTRATO_QTD];
    int qtdExtrato;
} Cliente;

typedef struct {
    Cliente *clientes;
    int qtdClientes;
    int capacidade;
    int aberto;
    int ganhador;               /* indice do ultimo contemplado no sorteio (-1 = nenhum) */
    float premio;
} Banco;

void limparBuffer(void);
int lerInteiro(int *v);
int lerReal(float *v);

int iniciarBanco(Banco *b);
int adicionarCliente(Banco *b, Cliente *c);
void liberarBanco(Banco *b);
int salvarBanco(Banco *b);
int carregarBanco(Banco *b);
void abrirBanco(Banco *b);
int cadastrarClientes(Banco *b);
void acessarConta(Banco *b);

Cliente *buscarCliente(Banco *b, int numero);
Conta *buscarConta(Cliente *c, int tipo);
float saldoCliente(Cliente *c);
int podeAdicionarConta(Cliente *c, int tipo);
void lerCliente(Cliente *c, int numero);
void registrar(Cliente *c, float valor, char op);
float limiteEmprestimo(Cliente *c);
int podeTitulos(Cliente *c);
void sessaoCliente(Cliente *c, Conta *conta);

int fibonacci(int n);
void efetuarSorteio(Banco *b);
void mostrarHistoricoBanco(Banco *b);

#endif
