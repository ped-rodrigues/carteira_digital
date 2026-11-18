#include <stdio.h>
#include "meu_banco.h"

void mostrarMenu(){
    printf("\n===Banco===\n");
    printf("1 - Abrir banco\n");
    printf("2 - Ver historico de transacoes\n");
    printf("3 - Cadastrar cliente\n");
    printf("4 - Listar clientes\n");
    printf("s - Sair do banco\n");
}

int combinacaoValida(int tipo1, int tipo2){
    if (tipo1 == tipo2) {
        return 0;
    }

    if ((tipo1 == CORRENTE && tipo2 == SALARIO) || (tipo1 == SALARIO && tipo2 == CORRENTE))
    {
        return 0;
    }

    return 1;
    
}

Cliente lerCliente() {
    Cliente c;

    printf("Nome do cliente: ");
    scanf(" %49[^\n]", c.nome);

    c.qtdContas = 0;
    c.emprestimo = 0;
    c.qtdHistorico = 0;

    return c;
}

Cliente lerContas(Cliente c, int primeiroNumero) {
    int tipo;
    float saldo;
    int mais = 1;

    while (c.qtdContas < MAX_CONTAS && mais) {
        printf("Tipo de conta (1-corrente, 2-salario, 3-poupanca): ");
        scanf("%d", &tipo);

        if (tipo < CORRENTE || tipo > POUPANCA)
        {
            printf("Tipo inválido.\n");
        } else if (!podeAdicionarConta(c, tipo))
        {
            printf("Esse cliente nao pode ter essa conta.\n");
        } else {
            printf("Saldo inicial: ");
            scanf("%f", &saldo);
            c = adicionarConta(c, primeiroNumero + c.qtdContas, tipo , saldo);
        }
        
        if (c.qtdContas < MAX_CONTAS)
        {
            printf("Adicionar outra conta? (1-sim, 0-nao): ");
            scanf("%d", &mais);
        }    
    }
    return c;
}

int podeAdicionarConta(Cliente c, int tipoNovo) {
    if (c.qtdContas >= MAX_CONTAS) {
        return 0;
    }
    if (c.qtdContas == 1) {
        return combinacaoValida(c.contas[0].tipo, tipoNovo);
    }
    return 1;
}

Cliente adicionarConta(Cliente c, int numero, int tipoNovo, float saldoInicial) {
    c.contas[c.qtdContas].numero = numero ;
    c.contas[c.qtdContas].tipo = tipoNovo ;
    c.contas[c.qtdContas].saldo = saldoInicial;
    c.qtdContas = c.qtdContas + 1;
    return c ;
}

int temConta(Cliente c, int tipo) {
    int i;
    for ( i = 0; i < c.qtdContas ; i++)
    {
        if (c.contas[i].tipo == tipo)
        {
            return 1;
        }
    }
    return 0;
}

float saldoDoTipo(Cliente c, int  tipo) {
    int i;
    for ( i = 0; i < c.qtdContas; i++)
    {
        if (c.contas[i].tipo == tipo)
        {
            return c.contas[i].saldo;
        }
        
    }
    return 0;
}
