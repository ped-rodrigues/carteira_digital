#include <stdio.h>
#include "meu_banco.h"
#include "entrada.h"
#include "sorteio.h"

int main(void) {
    char opcao = ' ';
    int numero, tipo;
    Cliente *cliente;
    Conta *conta;
    Banco banco;

    if (!iniciarBanco(&banco)) {
        printf("Sem memoria para iniciar o banco.\n");
        return 1;
    }

    do {
        mostrarMenu();
        printf("opcao: ");
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
            if (!banco.aberto) {
                printf("Abra o banco primeiro.\n");
                break;
            }
            printf("Numero da conta (ex.: 1000-01): ");
            if (scanf("%d-%d", &numero, &tipo) != 2) {
                limparBuffer();
                printf("Numero de conta invalido.\n");
                break;
            }
            cliente = buscarCliente(&banco, numero);
            conta = (cliente != NULL) ? buscarConta(cliente, tipo) : NULL;
            if (conta == NULL) {
                printf("Conta nao encontrada.\n");
            } else {
                sessaoCliente(cliente, conta);
            }
            break;

        case '5':
            if (banco.aberto) {
                cadastrarClientes(&banco);
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
            salvarBanco(&banco, ARQUIVO_BANCO);
        }
    } while (opcao != 's');

    liberarBanco(&banco);

    return 0;
}
