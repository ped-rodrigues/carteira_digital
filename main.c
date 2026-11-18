#include <stdio.h>
#include "meu_banco.h"

int main(void) {
    char opcao = ' ';
    int numCliente;
    float valor;
    float limite;
    Banco banco;

    if (!iniciarBanco(&banco)) {
        printf("Sem memoria para iniciar o banco.\n");
        return 1;
    }

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
            } else {
                Cliente novo;

                lerCliente(&novo);
                lerContas(&novo, 100 * banco.qtdClientes + 101);

                if (adicionarCliente(&banco, &novo)) {
                    printf("Cliente cadastrado.\n");
                } else {
                    liberarCliente(&novo); /* nao coube: devolve a memoria do historico dele */
                    printf("Sem memoria para cadastrar o cliente.\n");
                }
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
            } else {
                Cliente *cli = &banco.clientes[numCliente - 1];

                if (!podeEmprestimo(cli)) {
                    printf("Esse cliente nao pode pegar emprestimo.\n");
                } else {
                    limite = limiteEmprestimo(cli);
                    printf("Limite do emprestimo: R$ %.2f\n", limite);
                    printf("Valor desejado: ");
                    if (scanf("%f", &valor) != 1) {
                        limparBuffer();
                        valor = 0;
                    }

                    if (valor <= 0 || valor > limite) {
                        printf("Valor invalido (precisa ser maior que 0 e ate o limite).\n");
                    } else {
                        pegarEmprestimo(cli, valor);
                        printf("Emprestimo aprovado.\n");
                    }
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

    liberarBanco(&banco);

    return 0;
}
