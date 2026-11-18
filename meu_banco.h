#ifndef MEU_BANCO_H
#define MEU_BANCO_H

/* tipos de conta */
#define CORRENTE 1
#define SALARIO 2
#define POUPANCA 3

/* limites */
#define MAX_CONTAS 2
#define MAX_CLIENTES 50
#define MAX_HISTORICO 20
#define TAM_TEXTO 80

/* regras do emprestimo (a confirmar com a professora) */
#define MULT_EMPRESTIMO 10      /* 10 vezes o saldo da poupanca ... */
#define PERC_EMPRESTIMO 0.25f   /* ... e o cliente pode pegar 25% disso */

/* faixa da lista de contas salario */
#define SALARIO_MIN 1000.0f
#define SALARIO_MAX 1500.0f


typedef struct {
    int numero;
    int tipo;
    float saldo;
} Conta;

typedef struct {
    char nome[50];
    Conta contas[MAX_CONTAS];
    int qtdContas;
    int emprestimo;             /* 0 = ainda nao pegou, 1 = ja pegou */
    char historico[MAX_HISTORICO][TAM_TEXTO];
    int qtdHistorico;
} Cliente;

typedef struct {
    Cliente clientes[MAX_CLIENTES];
    int qtdClientes;
    int aberto;
} Banco;

/* menu e entrada */
void mostrarMenu(void);
void limparBuffer(void);

/* regras de contas */
int combinacaoValida(int tipo1, int tipo2);
int podeAdicionarConta(Cliente c, int tipoNovo);
Cliente adicionarConta(Cliente c, int numero, int tipoNovo, float saldoInicial);
int temConta(Cliente c, int tipo);
float saldoDoTipo(Cliente c, int tipo);
const char *nomeTipo(int tipo);

/* cadastro */
Cliente lerCliente(void);
Cliente lerContas(Cliente c, int primeiroNumero);

/* emprestimo */
int podeEmprestimo(Cliente c);
float limiteEmprestimo(Cliente c);
Cliente pegarEmprestimo(Cliente c, float valor);

/* historico */
Cliente registrarHistorico(Cliente c, const char *texto);
void mostrarHistorico(const Banco *b);

/* listagens e relatorios */
void listarClientes(const Banco *b);
float saldoTotal(const Banco *b);
void mostrarRelatorios(const Banco *b);

#endif
