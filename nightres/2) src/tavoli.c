/*CRUD del catalogo tavoli*/

#include "tavoli.h"
#include "file_io.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

int inserisci_tavolo(Tavolo **tavoli, int *num_tavoli, int *capacita_tavoli) {
    // Validazione
    if (tavoli == NULL || num_tavoli == NULL || capacita_tavoli == NULL) {
        printf("Errore: puntatori nulli\n");
        return -1;
    }
    
    // Ridimensiona se necessario
    if (*num_tavoli >= *capacita_tavoli) {
        *capacita_tavoli = (*capacita_tavoli == 0) ? 10 : *capacita_tavoli * 2;
        *tavoli = realloc(*tavoli, *capacita_tavoli * sizeof(Tavolo));
        if (*tavoli == NULL) {
            printf("Errore: memoria insufficiente\n");
            return -1;
        }
    }
    
    // Input utente
    printf("=== INSERISCI NUOVO TAVOLO ===\n");
    do {
        printf("Numero tavolo: ");
        if (scanf("%d", &(*tavoli)[*num_tavoli].numero) != 1) {
            printf("Input non valido. Inserisci un numero intero.\n");
            while (getchar() != '\n');
            continue;
        }
        if ((*tavoli)[*num_tavoli].numero <= 0) {
            printf("Numero tavolo deve essere maggiore di 0.\n");
            continue;
        }
        if (trova_tavolo(*tavoli, *num_tavoli, (*tavoli)[*num_tavoli].numero) != -1) {
            printf("Errore: esiste già un tavolo con questo numero.\n");
            continue;
        }
        break;
    } while (1);
    
    int numero_tavolo = (*tavoli)[*num_tavoli].numero;
    
    do {
        printf("Capienza: ");
        if (scanf("%d", &(*tavoli)[*num_tavoli].capienza) != 1) {
            printf("Input non valido. Inserisci un numero intero.\n");
            while (getchar() != '\n');
            continue;
        }
        if ((*tavoli)[*num_tavoli].capienza <= 0) {
            printf("La capienza deve essere maggiore di 0.\n");
            continue;
        }
        break;
    } while (1);
    
    do {
        printf("Prezzo minimo: ");
        if (scanf("%f", &(*tavoli)[*num_tavoli].prezzo_minimo) != 1) {
            printf("Input non valido. Inserisci un numero.\n");
            while (getchar() != '\n');
            continue;
        }
        if ((*tavoli)[*num_tavoli].prezzo_minimo < 0.0f) {
            printf("Il prezzo minimo non può essere negativo.\n");
            continue;
        }
        break;
    } while (1);
    int valid = 1;
    do{
        printf("Zona (VIP/DanceFloor/Lounge/esterno): ");
        scanf("%19s", (*tavoli)[*num_tavoli].zona);
        //convertiamo i caratteri in maiuscolo per uniformità
        for (int i = 0; (*tavoli)[*num_tavoli].zona[i] != '\0'; i++) {
            (*tavoli)[*num_tavoli].zona[i] = toupper((*tavoli)[*num_tavoli].zona[i]);
        }

        if (strcmp((*tavoli)[*num_tavoli].zona, "VIP") == 0 ||
            strcmp((*tavoli)[*num_tavoli].zona, "DANCEFLOOR") == 0 ||
            strcmp((*tavoli)[*num_tavoli].zona, "LOUNGE") == 0 ||
            strcmp((*tavoli)[*num_tavoli].zona, "ESTERNO") == 0) {
            valid = 0; // Zona valida
        }
        else {
            printf("Zona non valida. Inserire una delle seguenti: VIP, DanceFloor, Lounge, esterno.\n");
        }
    } while(valid);

    
    // Increment counter
    (*num_tavoli)++;
    
    // Salva immediatamente su file
    salva_tavoli_csv(*tavoli, *num_tavoli, TAVOLI_CSV);
    
    printf("✓ Tavolo inserito con successo!\n");
    return numero_tavolo;
}

int trova_tavolo(Tavolo *tavoli, int num_tavoli, int numero_tavolo) {
    // Scorre array cercando corrispondenza
    for (int i = 0; i < num_tavoli; i++) {
        if (tavoli[i].numero == numero_tavolo) {
            return i;  // Restituisce indice
        }
    }
    return -1;  // Non trovato
}

void ricerca_tavolo_per_zona(Tavolo *tavoli, int num_tavoli, char *zona) {
    // Usa strstr per ricerca parziale
    printf("\n=== TAVOLI IN ZONA '%s' ===\n", zona);
    
    int trovati = 0;
    for (int i = 0; i < num_tavoli; i++) {
        if (strstr(tavoli[i].zona, zona) != NULL) {
            printf("Tavolo %d | Cap: %d | Prezzo min: €%.2f | Zona: %s\n",
                   tavoli[i].numero, tavoli[i].capienza, 
                   tavoli[i].prezzo_minimo, tavoli[i].zona);
            trovati++;
        }
    }
    
    if (trovati == 0) {
        printf("Nessun tavolo trovato in questa zona.\n");
    }
}

void ricerca_tavolo_per_capienza(Tavolo *tavoli, int num_tavoli, int capienza_minima) {
    // Confronto numerico
    printf("\n=== TAVOLI CON CAPIENZA >= %d ===\n", capienza_minima);
    
    int trovati = 0;
    for (int i = 0; i < num_tavoli; i++) {
        if (tavoli[i].capienza >= capienza_minima) {
            printf("Tavolo %d | Cap: %d | Zona: %s\n",
                   tavoli[i].numero, tavoli[i].capienza, tavoli[i].zona);
            trovati++;
        }
    }
    
    if (trovati == 0) {
        printf("Nessun tavolo disponibile con questa capienza.\n");
    }
}

void modifica_tavolo(Tavolo *tavoli, int num_tavoli, int numero_tavolo,
                     CodaAttesa *coda_attesa,
                     Prenotazione **prenotazioni, int *num_prenotazioni, int *capacita_prenotazioni) {
    int indice = trova_tavolo(tavoli, num_tavoli, numero_tavolo);
    
    if (indice == -1) {
        printf("Errore: Tavolo %d non trovato!\n", numero_tavolo);
        return;
    }
    
    printf("\n=== MODIFICA TAVOLO %d ===\n", numero_tavolo);
    printf("1. Capienza (attuale: %d): ", tavoli[indice].capienza);
    scanf("%d", &tavoli[indice].capienza);
    
    printf("2. Prezzo minimo (attuale: €%.2f): ", tavoli[indice].prezzo_minimo);
    scanf("%f", &tavoli[indice].prezzo_minimo);
    int valid = 1;
    do{
        char zona_attuale[20];
        printf("3. Zona (attuale: %s): ", tavoli[indice].zona);
        scanf("%s", zona_attuale);
        //convertiamo i caratteri in maiuscolo per uniformità
        for (int i = 0; zona_attuale[i] != '\0'; i++) {
            zona_attuale[i] = toupper(zona_attuale[i]);
        }
        if (strcmp(zona_attuale, "VIP") == 0 ||
            strcmp(zona_attuale, "DANCEFLOOR") == 0 ||
            strcmp(zona_attuale, "LOUNGE") == 0 ||
            strcmp(zona_attuale, "ESTERNO") == 0) {
            valid = 0; // Zona valida
            strcpy(tavoli[indice].zona, zona_attuale);
        }
        else {
            printf("Zona non valida. Inserire una delle seguenti: VIP, DanceFloor, Lounge, esterno.\n");
        }
    } while(valid);
    // Salva immediatamente su file
    salva_tavoli_csv(tavoli, num_tavoli, TAVOLI_CSV);
    
    printf("✓ Tavolo modificato!\n");

    //controlla se qualcuno è in lista d'attesa per questo tavolo e data/ora
    //se sì, prova ad assegnare e rimuovere dalla lista d'attesa
    int id_assegnato = assegna_da_lista_attesa(coda_attesa,
                                               prenotazioni, num_prenotazioni, capacita_prenotazioni,
                                               tavoli, num_tavoli,
                                               &tavoli[indice]);
    if (id_assegnato != -1) {
        printf("✓ Cliente dalla lista d'attesa assegnato al tavolo %d!\n", numero_tavolo);
    }
}

void cancella_tavolo(Tavolo **tavoli, int *num_tavoli, int numero_tavolo) {
    int indice = trova_tavolo(*tavoli, *num_tavoli, numero_tavolo);
    
    if (indice == -1) {
        printf("Errore: Tavolo non trovato!\n");
        return;
    }
    
    // Sposta elementi in avanti per coprire il buco
    for (int i = indice; i < *num_tavoli - 1; i++) {
        (*tavoli)[i] = (*tavoli)[i + 1];
    }
    
    (*num_tavoli)--;
    
    // Salva immediatamente su file
    salva_tavoli_csv(*tavoli, *num_tavoli, TAVOLI_CSV);
    
    printf("✓ Tavolo eliminato!\n");
}

void visualizza_tavoli(Tavolo *tavoli, int num_tavoli) {
    printf("\n=== ELENCO TUTTI I TAVOLI ===\n");
    printf("%-10s | %-10s | %-15s | %-20s\n",
           "Numero", "Capienza", "Prezzo Min", "Zona");
    printf("----------------------------------------------\n");
    
    for (int i = 0; i < num_tavoli; i++) {
        printf("%-10d | %-10d | €%-14.2f | %-20s\n",
               tavoli[i].numero,
               tavoli[i].capienza,
               tavoli[i].prezzo_minimo,
               tavoli[i].zona);
    }
}

