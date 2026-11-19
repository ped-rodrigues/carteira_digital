#include <stdio.h>
#include "meu_banco.h"

void limparBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }
}

int lerInteiro(int *v) {
    if (scanf("%d", v) != 1) {
        limparBuffer();
        return 0;
    }
    return 1;
}

int lerReal(float *v) {
    if (scanf("%f", v) != 1) {
        limparBuffer();
        return 0;
    }
    return 1;
}
