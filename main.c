#include <stdio.h>
#include "meu_banco.h"

int main(void) {
    char opcao = ' ';
    Banco banco;

    if (!iniciarBanco(&banco)) {
        printf("Sem memoria para iniciar o banco.\n");
        return 1;
    }

    do {
        printf("\n===Banco===\n1 - Abrir banco\n2 - Consultar historico\n3 - Efetuar sorteio\n4 - Acessar conta\ns - Sair\nopcao: ");
        if (scanf(" %c", &opcao) != 1) {
            break;
        }

        switch (opcao) {
        case '1':
            abrirBanco(&banco);
            break;
        case '2':
            if (banco.aberto) {
                mostrarHistoricoBanco(&banco);
            } else {
                printf("Abra o banco primeiro.\n");
            }
            break;
        case '3':
            if (banco.aberto) {
                efetuarSorteio(&banco);
            } else {
                printf("Abra o banco primeiro.\n");
            }
            break;
        case '4':
            if (banco.aberto) {
                acessarConta(&banco);
            } else {
                printf("Abra o banco primeiro.\n");
            }
            break;
        case 's':
        case 'S':
            printf("Encerrando...\n");
            opcao = 's';
            break;
        default:
            printf("opcao invalida.\n");
            break;
        }

        if (banco.aberto) {
            salvarBanco(&banco);
        }
    } while (opcao != 's');

    liberarBanco(&banco);
    return 0;
}
