#include <stdio.h>
#include "meu_banco.h"

int main (void) {
    char opcao;
    int aberto = 0;
 
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