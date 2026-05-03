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
                tavoli[i].numero,
                tavoli[i].capienza,
                tavoli[i].prezzo_minimo,
                tavoli[i].zona);
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
                clienti[i].id,
                clienti[i].nome,
                clienti[i].cognome,
                clienti[i].livello_fedelta,
                clienti[i].penale_totale);
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
        if (prenotazioni[i].stato != ATTIVA) {
            continue;
        }
        if (prenotazioni[i].cliente == NULL || prenotazioni[i].tavolo == NULL) {
            printf("Errore: Prenotazione %d ha puntatori nulli, saltata\n", prenotazioni[i].id);
            continue;
        }
        char buffer[32];
        time_a_stringa(prenotazioni[i].data_ora, buffer, sizeof(buffer));
        fprintf(file, "%d,%d,%d,%s,%d,%d\n",
                prenotazioni[i].id,
                prenotazioni[i].id_cliente,
                prenotazioni[i].id_tavolo,
                buffer,
                prenotazioni[i].numero_persone,
                prenotazioni[i].fascia);
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
                    prenotazioni[i].id,
                    prenotazioni[i].id_cliente,
                    prenotazioni[i].id_tavolo,
                    buffer_data_ora,
                    prenotazioni[i].numero_persone,
                    prenotazioni[i].fascia,
                    stato_str,
                    prenotazioni[i].penale_applicata);
        }
    }
    
    fclose(file);
    printf("✓ Storico salvato su %s\n", filename);
}

// CARICAMENTO CSV - PARSING MANUALE

char* estrai_campo_csv(char *riga, int numero_campo, char delimiter) {
    static char campo[256];
    int campo_attuale = 0;
    int indice_campo = 0;
    
    for (int i = 0; riga[i] != '\0' && riga[i] != '\n'; i++) {
        if (riga[i] == delimiter) {
            if (campo_attuale == numero_campo) {
                campo[indice_campo] = '\0';
                return campo;
            }
            campo_attuale++;
            indice_campo = 0;
        } else {
            if (campo_attuale == numero_campo && indice_campo < 255) {
                campo[indice_campo++] = riga[i];
            }
        }
    }
    
    if (campo_attuale == numero_campo) {
        campo[indice_campo] = '\0';
        return campo;
    }
    
    return "";
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

    // Parsing riga per riga
    while (fgets(riga, sizeof(riga), file) != NULL) {
        if (riga_vuota(riga)) {
            continue;
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
        (*tavoli)[num_tavoli].numero = atoi(estrai_campo_csv(riga, 0, ','));
        (*tavoli)[num_tavoli].capienza = atoi(estrai_campo_csv(riga, 1, ','));
        (*tavoli)[num_tavoli].prezzo_minimo = atof(estrai_campo_csv(riga, 2, ','));
        strcpy((*tavoli)[num_tavoli].zona, estrai_campo_csv(riga, 3, ','));
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

    while (fgets(riga, sizeof(riga), file) != NULL) {
        if (riga_vuota(riga)) {
            continue;
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
        (*clienti)[num_clienti].id = atoi(estrai_campo_csv(riga, 0, ','));
        strcpy((*clienti)[num_clienti].nome, estrai_campo_csv(riga, 1, ','));
        strcpy((*clienti)[num_clienti].cognome, estrai_campo_csv(riga, 2, ','));
        strcpy((*clienti)[num_clienti].livello_fedelta, estrai_campo_csv(riga, 3, ','));
        (*clienti)[num_clienti].penale_totale = atof(estrai_campo_csv(riga, 4, ','));
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

    while (fgets(riga, sizeof(riga), file) != NULL) {
        if (riga_vuota(riga)) {
            continue;
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
        (*prenotazioni)[num_prenotazioni].id = atoi(estrai_campo_csv(riga, 0, ','));
        (*prenotazioni)[num_prenotazioni].id_cliente = atoi(estrai_campo_csv(riga, 1, ','));
        (*prenotazioni)[num_prenotazioni].id_tavolo = atoi(estrai_campo_csv(riga, 2, ','));
        (*prenotazioni)[num_prenotazioni].data_ora = stringa_a_time(estrai_campo_csv(riga, 3, ','));
        (*prenotazioni)[num_prenotazioni].numero_persone = atoi(estrai_campo_csv(riga, 4, ','));
        (*prenotazioni)[num_prenotazioni].fascia = atoi(estrai_campo_csv(riga, 5, ','));
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
    
    while (fgets(riga, sizeof(riga), file) != NULL) {
        if (riga_vuota(riga)) {
            continue;
        }
        num_letti++;
    }
    
    fclose(file);
    printf("✓ Storico caricato\n");
    return num_letti;
}