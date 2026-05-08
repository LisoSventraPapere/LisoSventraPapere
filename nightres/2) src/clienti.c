/*Gestione anagrafica clienti */

#include "clienti.h"
#include "file_io.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

// Variabile statica per tracciare l'ID massimo
static int max_id_cliente = 0;

int genera_id_cliente(Cliente *clienti, int num_clienti) {
    // Trova l'ID massimo e restituisce il successivo
    max_id_cliente = 0;
    for (int i = 0; i < num_clienti; i++) {
        if (clienti[i].id > max_id_cliente) {
            max_id_cliente = clienti[i].id;
        }
    }
    return max_id_cliente + 1;
}

void inserisci_cliente(Cliente **clienti, int *num_clienti, int *capacita_clienti) {
    if (clienti == NULL || num_clienti == NULL || capacita_clienti == NULL) {
        printf("Errore: puntatori nulli\n");
        return;
    }
    
    // Ridimensiona se necessario
    if (*num_clienti >= *capacita_clienti) {
        *capacita_clienti = (*capacita_clienti == 0) ? 10 : *capacita_clienti * 2;
        *clienti = realloc(*clienti, *capacita_clienti * sizeof(Cliente));
        if (*clienti == NULL) {
            printf("Errore: memoria insufficiente\n");
            return;
        }
    }
    
    printf("\n=== REGISTRA NUOVO CLIENTE ===\n");
    
    // Genera ID automatico
    (*clienti)[*num_clienti].id = genera_id_cliente(*clienti, *num_clienti);
    
    printf("Nome: ");
    scanf("%49s", (*clienti)[*num_clienti].nome);
    
    printf("Cognome: ");
    scanf("%49s", (*clienti)[*num_clienti].cognome);
    int valid = 1;
    do{
        printf("Livello fedeltà (standard/gold/VIP): ");
        scanf("%19s", (*clienti)[*num_clienti].livello_fedelta);
        // Converti a minuscolo per confronto
        for (int i = 0; (*clienti)[*num_clienti].livello_fedelta[i]; i++) {
            (*clienti)[*num_clienti].livello_fedelta[i] = tolower((*clienti)[*num_clienti].livello_fedelta[i]);
        }
        if (strcmp((*clienti)[*num_clienti].livello_fedelta, "standard") == 0 ||
            strcmp((*clienti)[*num_clienti].livello_fedelta, "gold") == 0 ||
            strcmp((*clienti)[*num_clienti].livello_fedelta, "vip") == 0) {
            valid = 0; // Livello valido
        }
        else {
            printf("Livello fedeltà non valido. Inserire una delle seguenti: standard, gold, vip.\n");
        }
    } while (valid);
    
    // Inizializza lista prenotazioni
    (*clienti)[*num_clienti].prenotazioni = NULL;
    (*clienti)[*num_clienti].penale_totale = 0.0;
    
    (*num_clienti)++;
    
    // Salva immediatamente su file
    salva_clienti_csv(*clienti, *num_clienti, CLIENTI_CSV);
    
    printf("\n✓ Cliente registrato con ID: %d\n", (*clienti)[*num_clienti - 1].id);
}

int trova_cliente(Cliente *clienti, int num_clienti, int id_cliente) {
    for (int i = 0; i < num_clienti; i++) {
        if (clienti[i].id == id_cliente) {
            return i;
        }
    }
    return -1;
}

void cancella_cliente(Cliente **clienti, int *num_clienti, int id_cliente) {
    int indice = trova_cliente(*clienti, *num_clienti, id_cliente);
    
    if (indice == -1) {
        printf("Errore: Cliente non trovato!\n");
        return;
    }
    
    if ((*clienti)[indice].prenotazioni != NULL) {
        printf("Errore: il cliente ha prenotazioni attive o storiche. Cancella prima le prenotazioni.\n");
        return;
    }
    
    // Sposta elementi in avanti
    for (int i = indice; i < *num_clienti - 1; i++) {
        (*clienti)[i] = (*clienti)[i + 1];
    }
    
    (*num_clienti)--;
    
    // Salva immediatamente su file
    salva_clienti_csv(*clienti, *num_clienti, CLIENTI_CSV);
    
    printf("✓ Cliente eliminato!\n");
}

void visualizza_storico_prenotazioni(Cliente *clienti, int num_clienti,
                                     int id_cliente) {
    int indice = trova_cliente(clienti, num_clienti, id_cliente);
    
    if (indice == -1) {
        printf("Errore: Cliente non trovato!\n");
        return;
    }
    
    printf("\n=== STORICO PRENOTAZIONI ===\n");
    //stampa a schermo lo storico del cliente dal file storico.csv
    printf("%-5s | %-10s | %-10s | %-20s | %-10s | %-10s | %-12s\n",
           "ID", "Tavolo", "Persone", "Data/Ora", "Fascia", "Stato", "Penale");
    printf("-----------------------------------------------------------------------------------------\n");
    //leggi il file storico.csv e stampa le prenotazioni del cliente
    FILE *file = fopen(STORICO_CSV, "r");
    if (file == NULL) {
        printf("Errore: impossibile aprire storico.csv\n");
        return;
    }

    /* Salta header */
    char header[256];
    if (fgets(header, sizeof(header), file) == NULL) {
        fclose(file);
        return;
    }

    Storico s;
        long data_long = 0;
        /*
         * Formato file: id_prenotazione,id_cliente,numero_tavolo,data_ora,num_persone,fascia_oraria,stato,penale
         * data_ora è salvato come stringa numerica di time_t (vedi salva_storico_csv)
         */
    while(1) {
        if(scanf(file, "%d,%d,%d,%ld,%d,%d,%31[^,],%f",&s.id_prenotazione,
                            &s.id_cliente, &s.id_tavolo, &data_long, &s.numero_persone, &s.fascia, s.stato, &s.penale) != 8) {
            printf("Errore: formato storico.csv non valido\n");
            fclose(file);
            return;
        }
        s.data_ora = (time_t)data_long;

        if (s.id_cliente != id_cliente) {
            printf("Errore: nessuna prenotazione trovata per questo cliente.\n");
            return;
        }

        char data_ora_buf[32];
        formatta_orario(s.data_ora, data_ora_buf, sizeof(data_ora_buf));

        char fascia_str[16];
        switch (s.fascia) {
            case 0: strcpy(fascia_str, "Apertura"); break;
            case 1: strcpy(fascia_str, "Prime ore"); break;
            case 2: strcpy(fascia_str, "Late night"); break;
            default: strcpy(fascia_str, "Sconosciuta"); break;
        }

        /* Stato è già una stringa (es: CANCELLATA, NO_SHOW) */
        printf("%-5d | %-10d | %-10d | %-20s | %-10s | %-10s | €%-10.2f\n",
            s.id_prenotazione, s.id_tavolo, s.numero_persone, data_ora_buf, fascia_str, s.stato, s.penale);
    }
    fclose(file);
}

void visualizza_clienti(Cliente *clienti, int num_clienti) {
    printf("\n=== ELENCO CLIENTI ===\n");
    printf("%-5s | %-15s | %-15s | %-12s | %-12s\n",
           "ID", "Nome", "Cognome", "Fedeltà", "Penale Totale");
    printf("-----------------------------------------------------------------\n");
    FILE *file = fopen(CLIENTI_CSV, "r");
    if (file == NULL) {
        printf("Errore: impossibile aprire %s\n", CLIENTI_CSV);
        return;
    }
    /* Salta header */
    char header[256];
    if (fgets(header, sizeof(header), file) == NULL) {
        fclose(file);
        return;
    }
    Cliente c;
    while (1) {
        int scanned = fscanf(file, "%d,%49[^,],%49[^,],%19[^,],%f",
                             &c.id, c.nome, c.cognome, c.livello_fedelta, &c.penale_totale);
        if (scanned == EOF) break;
        if (scanned != 5) {
            printf("Errore: formato clienti.csv non valido\n");
            fclose(file);
            return;
        }
        printf("%-5d | %-15s | %-15s | %-12s | €%-10.2f\n",
               c.id, c.nome, c.cognome, c.livello_fedelta, c.penale_totale);
    }
    fclose(file);
}

