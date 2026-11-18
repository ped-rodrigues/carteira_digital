#include <stdio.h>
#include "meu_banco.h"

int main (void) {
    char opcao;
    int aberto = 0;
    
    Banco banco;
    banco.qtdClientes = 0;
 
    do
    {
        mostrarMenu();
        printf("opcao: ");
        scanf(" %c", &opcao);

        
        switch (opcao)
        {
        case '1':
            aberto = 1;
            printf("Banco aberto.\n");
            break;
        case '2':
            if (aberto)
            {
                printf("(aqui entra o historico)\n");
            } else {
                printf("Abra o banco primeiro.\n");
            }
            break;
        case '3':
            if (!aberto)
            {
                printf("Abra o banco primeiro\n");
            } else if (banco.qtdClientes >= MAX_CLIENTES)
            {
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
              for ( int i = 0; i < banco.qtdClientes; i++)
              {
                printf("%d) %s | %d conta(s)\n", i + 1, banco.clientes[i].nome, banco.clientes[i].qtdContas);
              }
              break;  
        case 's':
            printf("Encerrando...\n");
            break;
        default:
            printf("opcao invalida.\n");
            break;
        }
    } while (opcao != 's');
    
          

    return 0;
}