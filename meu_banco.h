#ifndef MEU_BANCO_H
#define MEU_BANCO_H

/* tipos de conta */
#define CORRENTE 1
#define SALARIO 2
#define POUPANCA 3

/* limites e tamanhos */
#define MAX_CONTAS 2            /* cada cliente tem no maximo 2 contas */
#define CAPACIDADE_INICIAL 2    /* o banco comeca com espaco para 2 clientes e cresce sozinho */
#define TAM_TEXTO 80            /* tamanho maximo de uma linha do historico */

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
    char **historico;           /* vetor dinamico de textos (cada texto tambem e dinamico) */
    int qtdHistorico;           /* quantos textos ja existem */
    int capHistorico;           /* quantos cabem antes de precisar de realloc */
} Cliente;

typedef struct {
    Cliente *clientes;          /* vetor dinamico de clientes (malloc / realloc) */
    int qtdClientes;            /* quantos clientes ja existem */
    int capacidade;             /* quantos cabem antes de precisar de realloc */
    int aberto;
} Banco;

/* menu e entrada */
void mostrarMenu(void);
void limparBuffer(void);

/* ciclo de vida do banco (malloc / realloc / free) */
int iniciarBanco(Banco *b);
int adicionarCliente(Banco *b, const Cliente *c);
void liberarCliente(Cliente *c);
void liberarBanco(Banco *b);

/* regras de contas */
int combinacaoValida(int tipo1, int tipo2);
int podeAdicionarConta(const Cliente *c, int tipoNovo);
void adicionarConta(Cliente *c, int numero, int tipoNovo, float saldoInicial);
int temConta(const Cliente *c, int tipo);
float saldoDoTipo(const Cliente *c, int tipo);
const char *nomeTipo(int tipo);

/* cadastro */
void lerCliente(Cliente *c);
void lerContas(Cliente *c, int primeiroNumero);

/* emprestimo */
int podeEmprestimo(const Cliente *c);
float limiteEmprestimo(const Cliente *c);
void pegarEmprestimo(Cliente *c, float valor);

/* historico */
int registrarHistorico(Cliente *c, const char *texto);
void mostrarHistorico(const Banco *b);

/* listagens e relatorios */
void listarClientes(const Banco *b);
float saldoTotal(const Banco *b);
void mostrarRelatorios(const Banco *b);

#endif
