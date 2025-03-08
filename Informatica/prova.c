#include <stdio.h>

void cambia(int *x) {
    *x = *x + 10;
}

int main() {
    int numero = 5;
    cambia(&numero);
    printf("Numero: %d\n", numero);
    return 0;
}

