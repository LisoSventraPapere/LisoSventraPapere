#include <stdio.h>

int somma(int n) {
    if (n == 0)
        return 0;
    return n + somma(n - 1);
}

int main() {
    printf("%d\n", somma(3));
    return 0;
}








