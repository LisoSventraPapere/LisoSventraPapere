#include <stdio.h>
#include <stdlib.h>

void riempi_spirale(int N, int *matrice) {
    int alto = 0, basso = N - 1, sinistra = 0, destra = N - 1;
    int valore = 1;

    while (valore <= N * N) {
        // riga superiore: sinistra -> destra
        for (int j = sinistra; j <= destra && valore <= N * N; ++j)
            matrice[alto * N + j] = valore++;
        ++alto;

        // colonna destra: alto -> basso
        for (int i = alto; i <= basso && valore <= N * N; ++i)
            matrice[i * N + destra] = valore++;
        --destra;

        // riga inferiore: destra -> sinistra
        for (int j = destra; j >= sinistra && valore <= N * N; --j)
            matrice[basso * N + j] = valore++;
        --basso;

        // colonna sinistra: basso -> alto
        for (int i = basso; i >= alto && valore <= N * N; --i)
            matrice[i * N + sinistra] = valore++;
        ++sinistra;
    }
}

void stampa_matrice(int N, int *matrice) {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            printf("%3d ", matrice[i * N + j]); // ogni riga contine N elementi quindi bisogna saltare i*N elementi per raggiungere ogni riga,
        }                                       // j da l'aggiunta della posizione in colonna dentro quella riga.
        printf("\n");
    }
}

int main(void) {
    int N = 5;
    int *matrice = malloc(N * N * sizeof(int)); // matrice di totale N*N elementi interi
    if (!matrice) {
        fprintf(stderr, "Errore: impossibile allocare memoria\n"); // strampa un errore nello stream di errore standard.
        return 1; // termine del programma in codice di uscita 1 per segnalare la presenza di un errore.
    }

    riempi_spirale(N, matrice);
    printf("Matrice %dx%d riempita a spirale:\n", N, N);
    stampa_matrice(N, matrice);

    free(matrice);
    return 0;
}
