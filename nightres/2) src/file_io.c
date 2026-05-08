/*Lettura e scrittura CSV*/

#include "file_io.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

static int riga_vuota(const char *riga) {
    for (int i = 0; riga[i] != '\0'; i++) {
        if (riga[i] == '\r' || riga[i] == '\n') {
            continue;
        }
        if (!isspace((unsigned char)riga[i])) {
            return 0;
        }
    }
    return 1;
}

// SALVATAGGIO CSV

void salva_tavoli_csv(Tavolo *tavoli, int num_tavoli, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Errore: impossibile aprire %s per scrittura!\n", filename);
        return;
    }
    
    // Header
    fprintf(file, "numero,capienza,prezzo_minimo,zona\n");
    for (int i = 0; i < num_tavoli; i++) {
        fprintf(file, "%d,%d,%.2f,%s\n",
                tavoli[i].numero, tavoli[i].capienza, tavoli[i].prezzo_minimo, tavoli[i].zona);
    }
    
    fclose(file);
    printf("✓ Tavoli salvati su %s\n", filename);
}

void salva_clienti_csv(Cliente *clienti, int num_clienti, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Errore: impossibile aprire %s per scrittura!\n", filename);
        return;
    }
    
    fprintf(file, "id,nome,cognome,livello_fedelta,penale_totale\n");
    
    for (int i = 0; i < num_clienti; i++) {
        fprintf(file, "%d,%s,%s,%s,%.2f\n",
                clienti[i].id, clienti[i].nome, clienti[i].cognome, clienti[i].livello_fedelta, clienti[i].penale_totale);
    }
    
    fclose(file);
    printf("✓ Clienti salvati su %s\n", filename);
}

void salva_prenotazioni_csv(Prenotazione *prenotazioni, int num_prenotazioni,
                            const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Errore: impossibile aprire %s per scrittura!\n", filename);
        return;
    }
    
    // Solo prenotazioni attive salvate nel file principale
    fprintf(file, "id_prenotazione,id_cliente,id_tavolo,data_ora,num_persone,fascia_oraria\n");
    
    for (int i = 0; i < num_prenotazioni; i++) {
        char buffer[32];
        time_a_stringa(prenotazioni[i].data_ora, buffer, sizeof(buffer));
        fprintf(file, "%d,%d,%d,%s,%d,%d\n",
                prenotazioni[i].id, prenotazioni[i].id_cliente, prenotazioni[i].id_tavolo, buffer, prenotazioni[i].numero_persone, prenotazioni[i].fascia);
    }
    
    fclose(file);
    printf("✓ Prenotazioni salvate su %s\n", filename);
}

void salva_storico_csv(Prenotazione *prenotazioni, int num_prenotazioni,
                       Cliente *clienti, int num_clienti,
                       const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Errore: impossibile aprire %s per scrittura!\n", filename);
        return;
    }
    
    fprintf(file, "id_prenotazione,id_cliente,numero_tavolo,data_ora,num_persone,fascia_oraria,stato,penale\n");
    for (int i = 0; i < num_prenotazioni; i++) {
        if (prenotazioni[i].stato == CANCELLATA || prenotazioni[i].stato == NO_SHOW) {
            char buffer_data_ora[32];
            char stato_str[16];
            time_a_stringa(prenotazioni[i].data_ora, buffer_data_ora, sizeof(buffer_data_ora));
            if (prenotazioni[i].stato == CANCELLATA) {
                strcpy(stato_str, "CANCELLATA");
            } else {
                strcpy(stato_str, "NO_SHOW");
            }
            fprintf(file, "%d,%d,%d,%s,%d,%d,%s,%.2f\n",
                    prenotazioni[i].id, prenotazioni[i].id_cliente, prenotazioni[i].id_tavolo, buffer_data_ora, prenotazioni[i].numero_persone, prenotazioni[i].fascia, stato_str, prenotazioni[i].penale_applicata);
        }
    }
    
    fclose(file);
    printf("✓ Storico salvato su %s\n", filename);
}

time_t stringa_a_time(const char *stringa) {
    // Converte stringa numerica a time_t
    return (time_t)atol(stringa);
}

void time_a_stringa(time_t t, char *buffer, size_t size) {
    snprintf(buffer, size, "%ld", (long)t);
}

int carica_tavoli_csv(Tavolo **tavoli, int *capacita_tavoli, const char *filename) {
    if (tavoli == NULL || capacita_tavoli == NULL) {
        return 0;
    }

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Avviso: file %s non trovato (primo avvio?)\n", filename);
        *capacita_tavoli = 10;
        *tavoli = malloc(*capacita_tavoli * sizeof(Tavolo));
        return 0;
    }

    char riga[256];
    int num_tavoli = 0;
    *capacita_tavoli = 10;
    *tavoli = malloc(*capacita_tavoli * sizeof(Tavolo));
    if (*tavoli == NULL) {
        printf("Errore: memoria insufficiente\n");
        fclose(file);
        return 0;
    }

    // Salta header
    if (fgets(riga, sizeof(riga), file) == NULL) {
        fclose(file);
        return 0;
    }

    // Parsing riga per riga con fscanf
    while (1) {
        int numero, capienza;
        float prezzo;
        char zona[20];
        if(fscanf(file, "%d,%d,%f,%19[\n\r\t ],%*c", &numero, &capienza, &prezzo, zona) != 5) {
            printf("Errore: formato %s non valido\n", filename);
            fclose(file);
            return 0;
        }

        if (num_tavoli >= *capacita_tavoli) {
            *capacita_tavoli *= 2;
            *tavoli = realloc(*tavoli, *capacita_tavoli * sizeof(Tavolo));
            if (*tavoli == NULL) {
                printf("Errore: memoria insufficiente\n");
                fclose(file);
                return num_tavoli;
            }
        }

        (*tavoli)[num_tavoli].numero = numero;
        (*tavoli)[num_tavoli].capienza = capienza;
        (*tavoli)[num_tavoli].prezzo_minimo = prezzo;
        strncpy((*tavoli)[num_tavoli].zona, zona, sizeof((*tavoli)[num_tavoli].zona) - 1);
        (*tavoli)[num_tavoli].zona[sizeof((*tavoli)[num_tavoli].zona) - 1] = '\0';
        num_tavoli++;
    }

    fclose(file);
    printf("✓ %d tavoli caricati da %s\n", num_tavoli, filename);
    return num_tavoli;
}

int carica_clienti_csv(Cliente **clienti, int *capacita_clienti, const char *filename) {
    if (clienti == NULL || capacita_clienti == NULL) {
        return 0;
    }

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Avviso: file %s non trovato (primo avvio?)\n", filename);
        *capacita_clienti = 10;
        *clienti = malloc(*capacita_clienti * sizeof(Cliente));
        return 0;
    }

    char riga[256];
    int num_clienti = 0;
    *capacita_clienti = 10;
    *clienti = malloc(*capacita_clienti * sizeof(Cliente));
    if (*clienti == NULL) {
        printf("Errore: memoria insufficiente\n");
        fclose(file);
        return 0;
    }

    // Salta header
    if (fgets(riga, sizeof(riga), file) == NULL) {
        fclose(file);
        return 0;
    }

    while (1) {
        int id;
        char nome[50];
        char cognome[50];
        char livello[20];
        float penale;
        if(fscanf(file, "%d,%49[^,],%49[^,],%19[^,],%f",
                             &id, nome, cognome, livello, &penale) != 5) {
            printf("Errore: formato clienti.csv non valido\n");
            fclose(file);
            return 0;
        }

        if (num_clienti >= *capacita_clienti) {
            *capacita_clienti *= 2;
            *clienti = realloc(*clienti, *capacita_clienti * sizeof(Cliente));
            if (*clienti == NULL) {
                printf("Errore: memoria insufficiente\n");
                fclose(file);
                return num_clienti;
            }
        }

        (*clienti)[num_clienti].id = id;
        strncpy((*clienti)[num_clienti].nome, nome, sizeof((*clienti)[num_clienti].nome) - 1);
        (*clienti)[num_clienti].nome[sizeof((*clienti)[num_clienti].nome) - 1] = '\0';
        strncpy((*clienti)[num_clienti].cognome, cognome, sizeof((*clienti)[num_clienti].cognome) - 1);
        (*clienti)[num_clienti].cognome[sizeof((*clienti)[num_clienti].cognome) - 1] = '\0';
        strncpy((*clienti)[num_clienti].livello_fedelta, livello, sizeof((*clienti)[num_clienti].livello_fedelta) - 1);
        (*clienti)[num_clienti].livello_fedelta[sizeof((*clienti)[num_clienti].livello_fedelta) - 1] = '\0';
        (*clienti)[num_clienti].penale_totale = penale;
        (*clienti)[num_clienti].prenotazioni = NULL;
        num_clienti++;
    }

    fclose(file);
    printf("✓ %d clienti caricati da %s\n", num_clienti, filename);
    return num_clienti;
}

int carica_prenotazioni_csv(Prenotazione **prenotazioni, int *capacita_prenotazioni, const char *filename) {
    if (prenotazioni == NULL || capacita_prenotazioni == NULL) {
        return 0;
    }

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Avviso: file %s non trovato (primo avvio?)\n", filename);
        *capacita_prenotazioni = 10;
        *prenotazioni = malloc(*capacita_prenotazioni * sizeof(Prenotazione));
        return 0;
    }

    char riga[512];
    int num_prenotazioni = 0;
    *capacita_prenotazioni = 10;
    *prenotazioni = malloc(*capacita_prenotazioni * sizeof(Prenotazione));
    if (*prenotazioni == NULL) {
        printf("Errore: memoria insufficiente\n");
        fclose(file);
        return 0;
    }

    if (fgets(riga, sizeof(riga), file) == NULL) {
        fclose(file);
        return 0;
    }

    while (1) {
        int id, id_cliente, id_tavolo, numero_persone, fascia;
        long data_long;
        if(fscanf(file, "%d,%d,%d,%ld,%d,%d",
                             &id, &id_cliente, &id_tavolo, &data_long, &numero_persone, &fascia) != 6) {
            printf("Errore: formato prenotazioni.csv non valido\n");
            fclose(file);
            return 0;
        }

        if (num_prenotazioni >= *capacita_prenotazioni) {
            *capacita_prenotazioni *= 2;
            *prenotazioni = realloc(*prenotazioni, *capacita_prenotazioni * sizeof(Prenotazione));
            if (*prenotazioni == NULL) {
                printf("Errore: memoria insufficiente\n");
                fclose(file);
                return num_prenotazioni;
            }
        }

        (*prenotazioni)[num_prenotazioni].id = id;
        (*prenotazioni)[num_prenotazioni].id_cliente = id_cliente;
        (*prenotazioni)[num_prenotazioni].id_tavolo = id_tavolo;
        (*prenotazioni)[num_prenotazioni].data_ora = (time_t)data_long;
        (*prenotazioni)[num_prenotazioni].numero_persone = numero_persone;
        (*prenotazioni)[num_prenotazioni].fascia = fascia;
        (*prenotazioni)[num_prenotazioni].stato = ATTIVA;
        (*prenotazioni)[num_prenotazioni].ora_scadenza_no_show =
            calcola_scadenza_no_show((*prenotazioni)[num_prenotazioni].data_ora);
        (*prenotazioni)[num_prenotazioni].next = NULL;
        (*prenotazioni)[num_prenotazioni].cliente = NULL;
        (*prenotazioni)[num_prenotazioni].tavolo = NULL;
        num_prenotazioni++;
    }

    fclose(file);
    printf("✓ %d prenotazioni caricate da %s\n", num_prenotazioni, filename);
    return num_prenotazioni;
}

int carica_storico_csv(Prenotazione *prenotazioni, int *num_prenotazioni,
                       Cliente *clienti, int num_clienti,
                       const char *filename) {
    (void)prenotazioni;
    (void)num_prenotazioni;
    (void)clienti;
    (void)num_clienti;
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Avviso: file storico %s non trovato\n", filename);
        return 0;
    }
    
    char riga[256];
    int num_letti = 0;

    if (fgets(riga, sizeof(riga), file) == NULL) {
        fclose(file);
        return 0;
    }

    /* Leggi righe con fscanf per contare/parsing */
    while (1) {
        int id_pren, id_cliente, id_tavolo, num_pers, fascia;
        long data_long;
        char stato[32];
        float penale;
        if(fscanf(file, "%d,%d,%d,%ld,%d,%d,%31[^,],%f",
                             &id_pren, &id_cliente, &id_tavolo, &data_long,
                             &num_pers, &fascia, stato, &penale) != 8) {
            printf("Errore: formato storico.csv non valido\n");
            fclose(file);
            return 0;
        }

        (void)id_pren; (void)id_cliente; (void)id_tavolo; (void)data_long;
        (void)num_pers; (void)fascia; (void)stato; (void)penale;
        num_letti++;
    }
    
    fclose(file);
    printf("✓ Storico caricato\n");
    return num_letti;
}