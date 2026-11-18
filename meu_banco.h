#ifndef MEU_BANCO_H
#define MEU_BANCO_H

#define CORRENTE 1
#define SALARIO 2
#define POUPANCA 3

#define MAX_CONTAS 2
#define CAPACIDADE_INICIAL 2
#define TAM_TEXTO 80

/* regras do emprestimo (a confirmar com a professora) */
#define MULT_EMPRESTIMO 10
#define PERC_EMPRESTIMO 0.25f

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
    char **historico;
    int qtdHistorico;
    int capHistorico;
} Cliente;

typedef struct {
    Cliente *clientes;
    int qtdClientes;
    int capacidade;
    int aberto;
} Banco;

void mostrarMenu(void);
void limparBuffer(void);

int iniciarBanco(Banco *b);
int adicionarCliente(Banco *b, const Cliente *c);
void liberarCliente(Cliente *c);
void liberarBanco(Banco *b);

int combinacaoValida(int tipo1, int tipo2);
int podeAdicionarConta(const Cliente *c, int tipoNovo);
void adicionarConta(Cliente *c, int numero, int tipoNovo, float saldoInicial);
int temConta(const Cliente *c, int tipo);
float saldoDoTipo(const Cliente *c, int tipo);
const char *nomeTipo(int tipo);

void lerCliente(Cliente *c);
void lerContas(Cliente *c, int primeiroNumero);

int podeEmprestimo(const Cliente *c);
float limiteEmprestimo(const Cliente *c);
void pegarEmprestimo(Cliente *c, float valor);

int registrarHistorico(Cliente *c, const char *texto);
void mostrarHistorico(const Banco *b);

void listarClientes(const Banco *b);
float saldoTotal(const Banco *b);
void mostrarRelatorios(const Banco *b);

#endif
