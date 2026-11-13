#include <stdio.h>
#include "meu_banco.h"

void mostrarMenu(){
    printf("\n===Banco===\n");
    printf("1 - Abrir banco\n");
    printf("2 - Ver historico de transacoes\n");
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

int podeAdicionarConta(Cliente c, int tipoNovo) {
    if (c.qtdContas >= MAX_CONTAS) {
        return 0;
    }
    if (c.qtdContas == 1) {
        return combinacaoValida(c.contas[0].tipo, tipoNovo);
    }
    return 1;
}