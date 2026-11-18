#include <stdio.h>
#include "meu_banco.h"

int main(void) {
    char opcao = ' ';
    int numCliente;
    float valor;
    float limite;

    Banco banco;
    banco.qtdClientes = 0;
    banco.aberto = 0;

    do {
        mostrarMenu();
        printf("opcao: ");
        if (scanf(" %c", &opcao) != 1) {
            break; /* acabou a entrada */
        }

        switch (opcao) {
        case '1':
            banco.aberto = 1;
            printf("Banco aberto.\n");
            break;

        case '2':
            if (banco.aberto) {
                mostrarHistorico(&banco);
            } else {
                printf("Abra o banco primeiro.\n");
            }
            break;

        case '3':
            if (!banco.aberto) {
                printf("Abra o banco primeiro.\n");
            } else if (banco.qtdClientes >= MAX_CLIENTES) {
                printf("Banco cheio.\n");
            } else {
                Cliente novo = lerCliente();

                novo = lerContas(novo, 100 * banco.qtdClientes + 101);

                banco.clientes[banco.qtdClientes] = novo;
                banco.qtdClientes = banco.qtdClientes + 1;

                printf("Cliente cadastrado.\n");
            }
            break;

        case '4':
            if (banco.aberto) {
                listarClientes(&banco);
            } else {
                printf("Abra o banco primeiro.\n");
            }
            break;

        case '5':
            if (!banco.aberto) {
                printf("Abra o banco primeiro.\n");
                break;
            }
            if (banco.qtdClientes == 0) {
                printf("Nenhum cliente cadastrado.\n");
                break;
            }

            listarClientes(&banco);
            printf("Numero do cliente: ");
            if (scanf("%d", &numCliente) != 1) {
                limparBuffer();
                numCliente = 0;
            }

            if (numCliente < 1 || numCliente > banco.qtdClientes) {
                printf("Cliente invalido.\n");
            } else if (!podeEmprestimo(banco.clientes[numCliente - 1])) {
                printf("Esse cliente nao pode pegar emprestimo.\n");
            } else {
                limite = limiteEmprestimo(banco.clientes[numCliente - 1]);
                printf("Limite do emprestimo: R$ %.2f\n", limite);
                printf("Valor desejado: ");
                if (scanf("%f", &valor) != 1) {
                    limparBuffer();
                    valor = 0;
                }

                if (valor <= 0 || valor > limite) {
                    printf("Valor invalido (precisa ser maior que 0 e ate o limite).\n");
                } else {
                    banco.clientes[numCliente - 1] = pegarEmprestimo(banco.clientes[numCliente - 1], valor);
                    printf("Emprestimo aprovado.\n");
                }
            }
            break;

        case '6':
            if (banco.aberto) {
                mostrarRelatorios(&banco);
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
    } while (opcao != 's');

    return 0;
}
