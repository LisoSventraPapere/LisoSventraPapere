#include "prenotazioni.h"
#include "file_io.h"
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

struct tm *converti_time_t_in_tm(time_t data_ora) {
    struct tm *info_tempo = localtime(&data_ora);
    if (info_tempo == NULL) {
        // Gestione errore: restituisci NULL o gestisci diversamente
        return NULL;
    }
    return info_tempo;
}

// Aggiungi questa nuova funzione helper per formattare l'orario in stringa leggibile
void formatta_orario(time_t t, char *buffer, size_t size) {
    struct tm *tm_info = localtime(&t);
    if (tm_info != NULL) {
        strftime(buffer, size, "%H:%M", tm_info);
    } else {
        // In caso di errore, copia una stringa vuota
        buffer[0] = '\0';
    }
}


time_t calcola_scadenza_no_show(time_t ora_turno) {
    // Aggiunge 30 minuti (1800 secondi)
    return ora_turno + (30 * 60);
}

/**
 * Calcola l'orario di inizio del turno in base alla fascia oraria
 */
time_t calcola_orario_inizio_fascia(time_t ora_rif, FasciaOraria fascia) {
    struct tm tm_data = *localtime(&ora_rif);

    switch (fascia) {
        case APERTURA:
            tm_data.tm_hour = 20;
            tm_data.tm_min = 0;
            tm_data.tm_sec = 0;
            break;
        case PRIME_ORE:
            tm_data.tm_hour = 22;
            tm_data.tm_min = 0;
            tm_data.tm_sec = 0;
            break;
        case LATE_NIGHT:
            if (tm_data.tm_hour >= 4) {
                tm_data.tm_mday += 1;
            }
            tm_data.tm_hour = 0;
            tm_data.tm_min = 0;
            tm_data.tm_sec = 0;
            break;
        default:
            tm_data.tm_hour = 20;
            tm_data.tm_min = 0;
            tm_data.tm_sec = 0;
            break;
    }

    return mktime(&tm_data);
}

int controlla_disponibilita(Prenotazione *prenotazioni, int num_prenotazioni,
                            Tavolo *tavolo, time_t data_ora, FasciaOraria fascia) {
    for (int i = 0; i < num_prenotazioni; i++) {
        if (prenotazioni[i].tavolo == tavolo &&
            prenotazioni[i].fascia == fascia &&
            prenotazioni[i].stato == ATTIVA) {
            
            // Controlla se è lo stesso giorno
            time_t giorno_prenotazione = (prenotazioni[i].data_ora / 86400) * 86400;
            time_t giorno_ricerca = (data_ora / 86400) * 86400;
            
            if (giorno_prenotazione == giorno_ricerca) {
                return 0;  // NON disponibile
            }
        }
    }
    return 1;  // Disponibile
}

int crea_prenotazione(Prenotazione **prenotazioni, int *num_prenotazioni, int *capacita_prenotazioni,
                      Tavolo *tavoli, int num_tavoli,
                      Cliente *cliente, Tavolo *tavolo, 
                      time_t data_ora, int num_persone, FasciaOraria fascia) {
    
    if (prenotazioni == NULL || num_prenotazioni == NULL || capacita_prenotazioni == NULL) {
        printf("Errore: puntatori nulli\n");
        return -1;
    }
    
    if (num_persone <= 0) {
        printf("⚠ Numero persone deve essere maggiore di zero!\n");
        return -1;
    }
    if (num_persone > tavolo->capienza) {
        printf("⚠ Numero persone supera la capienza del tavolo!\n");
        return -1;
    }

    // Ridimensiona se necessario
    if (*num_prenotazioni >= *capacita_prenotazioni) {
        *capacita_prenotazioni = (*capacita_prenotazioni == 0) ? 10 : *capacita_prenotazioni * 2;
        *prenotazioni = realloc(*prenotazioni, *capacita_prenotazioni * sizeof(Prenotazione));
        if (*prenotazioni == NULL) {
            printf("Errore: memoria insufficiente\n");
            return -1;
        }
    }
    
    // Verifica che il tavolo sia valido (deve essere nell'array tavoli)
    int tavolo_valido = 0;
    for (int i = 0; i < num_tavoli; i++) {
        if (&tavoli[i] == tavolo) {
            tavolo_valido = 1;
            break;
        }
    }
    
    if (!tavolo_valido) {
        printf("Errore: Tavolo non valido!\n");
        return -1;
    }
    
    // Imposta l'orario di inizio turno in base alla fascia oraria
    time_t orario_inizio_turno = calcola_orario_inizio_fascia(data_ora, fascia);

    // Verifica disponibilità
    if (!controlla_disponibilita(*prenotazioni, *num_prenotazioni, 
                                 tavolo, orario_inizio_turno, fascia)) {
        printf("⚠ Tavolo %d NON disponibile per questa fascia!\n", tavolo->numero);
        printf("   Cliente inserito in LISTA D'ATTESA.\n");
        return -1;  // Inserire in lista d'attesa altrove
    }
    
    // Genera ID prenotazione
    int nuovo_id = 1;
    if (*num_prenotazioni > 0) {
        for (int i = 0; i < *num_prenotazioni; i++) {
            if ((*prenotazioni)[i].id >= nuovo_id) {
                nuovo_id = (*prenotazioni)[i].id + 1;
            }
        }
    }
    
    // Popola struttura
    (*prenotazioni)[*num_prenotazioni].id = nuovo_id;
    (*prenotazioni)[*num_prenotazioni].id_cliente = cliente->id;
    (*prenotazioni)[*num_prenotazioni].id_tavolo = tavolo->numero;
    (*prenotazioni)[*num_prenotazioni].cliente = cliente;
    (*prenotazioni)[*num_prenotazioni].tavolo = tavolo;
    (*prenotazioni)[*num_prenotazioni].data_ora = orario_inizio_turno;
    (*prenotazioni)[*num_prenotazioni].numero_persone = num_persone;
    (*prenotazioni)[*num_prenotazioni].fascia = fascia;
    (*prenotazioni)[*num_prenotazioni].stato = ATTIVA;
    (*prenotazioni)[*num_prenotazioni].ora_scadenza_no_show = 
        calcola_scadenza_no_show(orario_inizio_turno);
    (*prenotazioni)[*num_prenotazioni].data_cancellazione = 0;
    (*prenotazioni)[*num_prenotazioni].penale_applicata = 0.0f;
    (*prenotazioni)[*num_prenotazioni].next = NULL;
    
    // Aggiungi alla lista collegata del cliente
    if (cliente->prenotazioni == NULL) {
        cliente->prenotazioni = &(*prenotazioni)[*num_prenotazioni];
    } else {
        Prenotazione *current = cliente->prenotazioni;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = &(*prenotazioni)[*num_prenotazioni];
    }
    
    (*num_prenotazioni)++;
    
    // Salva immediatamente su file
    salva_prenotazioni_csv(*prenotazioni, *num_prenotazioni, PRENOTAZIONI_CSV);
    
    printf("✓ Prenotazione #%d creata con successo!\n", nuovo_id);
    return nuovo_id;
}

float calcola_penale(time_t ora_creazione, time_t ora_cancellazione,
                     time_t ora_prenotazione) {
    (void)ora_creazione;

    long differenza_secondi = ora_prenotazione - ora_cancellazione;
    long ore_anticipo = differenza_secondi / 3600;
    
    float penale = 0.0;
    
    if (ore_anticipo < 0) {
        // Cancellazione dopo l'orario: penale massima (50€)
        penale = 50.0;
    } else if (ore_anticipo < 2) {
        // Meno di 2 ore: penale 30€
        penale = 30.0;
    } else if (ore_anticipo < 24) {
        // Tra 2 e 24 ore: penale 15€
        penale = 15.0;
    }
    // Altrimenti penale = 0
    
    return penale;
}

float cancella_prenotazione(Prenotazione **prenotazioni, int *num_prenotazioni,
                             int id_prenotazione, time_t ora_cancellazione,
                             Tavolo *tavoli, int num_tavoli,
                             CodaAttesa *coda_attesa,
                             Cliente *clienti, int num_clienti, time_t data_ora, FasciaOraria fascia) {
    
    // Trova prenotazione
    int indice = -1;
    for (int i = 0; i < *num_prenotazioni; i++) {
        if ((*prenotazioni)[i].id == id_prenotazione) {
            indice = i;
            break;
        }
    }
    
    if (indice == -1) {
        printf("Errore: Prenotazione non trovata!\n");
        return -1.0;
    }
    
    Prenotazione *pren = &(*prenotazioni)[indice];
    
    // Rimuovi dalla lista collegata del cliente
    Cliente *cliente = pren->cliente;
    if (cliente->prenotazioni == pren) {
        cliente->prenotazioni = pren->next;
    } else {
        Prenotazione *current = cliente->prenotazioni;
        while (current != NULL && current->next != pren) {
            current = current->next;
        }
        if (current != NULL) {
            current->next = pren->next;
        }
    }
    
    // Cerca se c'è qualcuno in attesa per questo tavolo
    if (coda_attesa->testa != NULL && coda_attesa->testa->tavolo == pren->tavolo) {
        // Decoda il primo in attesa
        ElementoAttesa *primo = coda_attesa->testa;
        coda_attesa->testa = primo->next;
        if (coda_attesa->testa == NULL) {
            coda_attesa->coda = NULL;
        }
        coda_attesa->dimensione--;
        
        // Trasferisci prenotazione al cliente in attesa
        pren->cliente = primo->cliente;
        pren->id_cliente = primo->cliente->id;
        
        // Aggiungi alla lista collegata del nuovo cliente
        if (primo->cliente->prenotazioni == NULL) {
            primo->cliente->prenotazioni = pren;
        } else {
            Prenotazione *current = primo->cliente->prenotazioni;
            while (current->next != NULL) {
                current = current->next;
            }
            current->next = pren;
        }
        pren->next = NULL;
        
        // Libera memoria del nodo coda
        free(primo);
        
        // Salva immediatamente su file
        salva_prenotazioni_csv(*prenotazioni, *num_prenotazioni, PRENOTAZIONI_CSV);
        
        printf("✓ Prenotazione trasferita al cliente in attesa!\n");
        printf("  Nuovo cliente: %d\n", pren->cliente->id);
        
        return 0.0;  // Nessuna penale
    } else {
        // Nessun cliente in attesa: procedi con cancellazione normale
        
        // Calcola penale
        time_t orario_inizio_turno = calcola_orario_inizio_fascia(data_ora, fascia);
        float penale = calcola_penale(pren->data_ora, ora_cancellazione, orario_inizio_turno);
        
        // Aggiungi penale al cliente
        pren->cliente->penale_totale += penale;
        pren->penale_applicata = penale;
        pren->data_cancellazione = ora_cancellazione;
        
        // Marca come cancellata
        pren->stato = CANCELLATA;
        
        // Salva immediatamente su file
        salva_prenotazioni_csv(*prenotazioni, *num_prenotazioni, PRENOTAZIONI_CSV);
        salva_storico_csv(*prenotazioni, *num_prenotazioni, clienti, num_clienti, STORICO_CSV);
        salva_clienti_csv(clienti, num_clienti, CLIENTI_CSV);
        
        printf("✓ Prenotazione cancellata.\n");
        printf("  Penale applicata: €%.2f\n", penale);
        
        return penale;
    }
}

static int prenotazione_appartiene_serata_corrente(time_t data_prenotazione,
                                                     time_t ora_corrente) {
    struct tm tm_now = *localtime(&ora_corrente);
    struct tm tm_start = tm_now;
    struct tm tm_end = tm_now;

    if (tm_now.tm_hour >= 20) {
        tm_start.tm_hour = 20;
        tm_start.tm_min = 0;
        tm_start.tm_sec = 0;

        tm_end.tm_mday += 1;
        tm_end.tm_hour = 4;
        tm_end.tm_min = 0;
        tm_end.tm_sec = 0;
    } else if (tm_now.tm_hour < 4) {
        tm_start.tm_mday -= 1;
        tm_start.tm_hour = 20;
        tm_start.tm_min = 0;
        tm_start.tm_sec = 0;

        tm_end.tm_hour = 4;
        tm_end.tm_min = 0;
        tm_end.tm_sec = 0;
    } else {
        tm_start.tm_hour = 20;
        tm_start.tm_min = 0;
        tm_start.tm_sec = 0;

        tm_end.tm_mday += 1;
        tm_end.tm_hour = 4;
        tm_end.tm_min = 0;
        tm_end.tm_sec = 0;
    }

    time_t start = mktime(&tm_start);
    time_t end = mktime(&tm_end);
    return data_prenotazione >= start && data_prenotazione < end;
}

void visualizza_prenotazioni_serata(Prenotazione *prenotazioni, 
                                    int num_prenotazioni, time_t data_serata) {
    printf("\n=== PRENOTAZIONI SERATA ===\n");
    printf("%-5s | %-10s | %-8s | %-8s | %-10s | %-10s | %-12s\n",
           "ID", "Cliente", "Tavolo", "Persone", "Orario", "Fascia", "Stato");
    printf("------------------------------------------------------------------------\n");
    
    int count = 0;
    for (int i = 0; i < num_prenotazioni; i++) {
        if (prenotazioni[i].stato == ATTIVA &&
            prenotazione_appartiene_serata_corrente(prenotazioni[i].data_ora, data_serata)) {
            const char *fascia_str;
            switch (prenotazioni[i].fascia) {
                case APERTURA: fascia_str = "Apertura"; break;
                case PRIME_ORE: fascia_str = "Prime ore"; break;
                case LATE_NIGHT: fascia_str = "Late night"; break;
                default: fascia_str = "?";
            }
            
            char orario_buffer[6];
            formatta_orario(prenotazioni[i].data_ora, orario_buffer, sizeof(orario_buffer));
            
            printf("%-5d | %-10d | %-8d | %-10d | %-10s | %-12s | ATTIVA\n",
                   prenotazioni[i].id,
                   prenotazioni[i].cliente->id,
                   prenotazioni[i].tavolo->numero,
                   prenotazioni[i].numero_persone,
                   orario_buffer,
                   fascia_str);
            count++;
        }
    }
    
    if (count == 0) {
        printf("Nessuna prenotazione per questa serata.\n");
    }
}

void controlla_no_show_con_orario(Prenotazione *prenotazioni, int num_prenotazioni, time_t ora_test,
                                     Cliente *clienti, int num_clienti) {
    int no_show_count = 0;
    
    printf("\n=== CONTROLLO NO-SHOW [ORARIO CUSTOM] ===\n");
    printf("Ora di test: %ld\n", (long)ora_test);
    printf("Verifica in corso...\n\n");
    
    for (int i = 0; i < num_prenotazioni; i++) {
        if (prenotazioni[i].stato == ATTIVA) {
            printf("Prenotazione #%d: scadenza alle %ld (tra %ld secondi)\n", 
                   prenotazioni[i].id,
                   (long)prenotazioni[i].ora_scadenza_no_show,
                   (long)(prenotazioni[i].ora_scadenza_no_show - ora_test));
            
            if (ora_test > prenotazioni[i].ora_scadenza_no_show) {
                prenotazioni[i].stato = NO_SHOW;
                float penale = calcola_penale(prenotazioni[i].data_ora, ora_test, prenotazioni[i].data_ora);
                prenotazioni[i].penale_applicata = penale;
                if (prenotazioni[i].cliente != NULL) {
                    prenotazioni[i].cliente->penale_totale += penale;
                }
                printf("  ⚠ MARCATA COME NO-SHOW!\n");
                no_show_count++;
            } else {
                printf("  ✓ Ancora valida\n");
            }
        }
    }
    
    printf("\n--- RISULTATI ---\n");
    printf("Prenotazioni marcate come NO-SHOW: %d\n", no_show_count);
    
    if (no_show_count > 0) {
        salva_prenotazioni_csv(prenotazioni, num_prenotazioni, PRENOTAZIONI_CSV);
        salva_storico_csv(prenotazioni, num_prenotazioni, clienti, num_clienti, STORICO_CSV);
        salva_clienti_csv(clienti, num_clienti, CLIENTI_CSV);
        printf("✓ Dati salvati su CSV\n");
    } else {
        printf("Nessuna prenotazione scaduta.\n");
    }
}

// FUNZIONI LISTA D'ATTESA

void inserisci_lista_attesa(CodaAttesa *coda_attesa,
                            Cliente *cliente, Tavolo *tavolo,
                            time_t data_ora, int num_persone, FasciaOraria fascia) {
    
    ElementoAttesa *nuovo = malloc(sizeof(ElementoAttesa));
    if (nuovo == NULL) {
        printf("Errore: memoria insufficiente\n");
        return;
    }
    
    nuovo->id_cliente = cliente->id;
    nuovo->id_tavolo = tavolo->numero;
    nuovo->cliente = cliente;
    nuovo->tavolo = tavolo;
    nuovo->data_ora = data_ora;
    nuovo->numero_persone = num_persone;
    nuovo->fascia = fascia;
    nuovo->next = NULL;
    
    if (coda_attesa->coda == NULL) {
        coda_attesa->testa = nuovo;
        coda_attesa->coda = nuovo;
    } else {
        coda_attesa->coda->next = nuovo;
        coda_attesa->coda = nuovo;
    }
    
    coda_attesa->dimensione++;
    
    printf("✓ Cliente inserito in lista d'attesa (posizione %d)\n", coda_attesa->dimensione);
}

int assegna_da_lista_attesa(CodaAttesa *coda_attesa,
                            Prenotazione **prenotazioni, int *num_prenotazioni, int *capacita_prenotazioni,
                            Tavolo *tavoli, int num_tavoli, Tavolo *tavolo) {
    
    if (coda_attesa->testa == NULL) {
        return -1;  // Nessuno in attesa
    }
    
    // Trova il primo in attesa per questo tavolo
    ElementoAttesa *current = coda_attesa->testa;
    ElementoAttesa *prev = NULL;
    while (current != NULL && current->tavolo != tavolo) {
        prev = current;
        current = current->next;
    }
    
    if (current == NULL) {
        return -1;  // Nessuno in attesa per questo tavolo
    }
    
    // Crea prenotazione per il primo in attesa
    int id_nuova = crea_prenotazione(prenotazioni, num_prenotazioni, capacita_prenotazioni,
                                      tavoli, num_tavoli,
                                      current->cliente,
                                      current->tavolo,
                                      current->data_ora,
                                      current->numero_persone,
                                      current->fascia);
    
    if (id_nuova != -1) {
        // Rimuovi dalla coda
        if (prev == NULL) {
            coda_attesa->testa = current->next;
        } else {
            prev->next = current->next;
        }
        if (coda_attesa->coda == current) {
            coda_attesa->coda = prev;
        }
        coda_attesa->dimensione--;
        
        free(current);
        
        printf("✓ Cliente assegnato dalla lista d'attesa!\n");
    }
    
    return id_nuova;
}

void visualizza_lista_attesa(CodaAttesa *coda_attesa) {
    printf("\n=== LISTA D'ATTESA (FIFO) ===\n");
    
    if (coda_attesa->dimensione == 0) {
        printf("Nessuno in lista d'attesa.\n");
        return;
    }
    
    printf("Pos | Cliente | Tavolo | Persone | Fascia\n");
    printf("-------------------------------------------\n");
    
    ElementoAttesa *current = coda_attesa->testa;
    int pos = 1;
    while (current != NULL) {
        const char *fascia_str;
        switch (current->fascia) {
            case APERTURA: fascia_str = "Apertura"; break;
            case PRIME_ORE: fascia_str = "Prime ore"; break;
            case LATE_NIGHT: fascia_str = "Late night"; break;
            default: fascia_str = "?";
        }
        
        printf("%d  | %d | %d | %d | %s\n",
               pos,
               current->cliente->id,
               current->tavolo->numero,
               current->numero_persone,
               fascia_str);
        
        current = current->next;
        pos++;
    }
}