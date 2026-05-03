/*Menu' e ciclo principale*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include "tavoli.h"
#include "clienti.h"
#include "prenotazioni.h"
#include "file_io.h"
#include "utils.h"

// Array dinamici
Tavolo *tavoli = NULL;
int num_tavoli = 0;
int capacita_tavoli = 0;

Cliente *clienti = NULL;
int num_clienti = 0;
int capacita_clienti = 0;

Prenotazione *prenotazioni = NULL;
int num_prenotazioni = 0;
int capacita_prenotazioni = 0;

CodaAttesa coda_attesa = {NULL, NULL, 0};

// Funzioni menu
void menu_principale() {
    printf("\n");
    stampa_titolo("SISTEMA PRENOTAZIONI RISTORANTE");
    printf("1. Gestione Tavoli\n");
    printf("2. Gestione Clienti\n");
    printf("3. Gestione Prenotazioni\n");
    printf("4. Lista d'Attesa\n");
    printf("5. Statistiche\n");
    printf("0. Esci\n");
    printf("Scelta: ");
}

void menu_tavoli() {
    int scelta;
    char zona[20];
    int capienza;
    
    do {
        printf("\n=== GESTIONE TAVOLI ===\n");
        printf("1. Inserisci tavolo\n");
        printf("2. Ricerca per zona\n");
        printf("3. Ricerca per capienza\n");
        printf("4. Modifica tavolo\n");
        printf("5. Elimina tavolo\n");
        printf("6. Visualizza tutti\n");
        printf("0. Torna indietro\n");
        printf("Scelta: ");
        scanf("%d", &scelta);
        
        switch (scelta) {
            case 1: {
                int nuovo_numero_tavolo = inserisci_tavolo(&tavoli, &num_tavoli, &capacita_tavoli);
                if (nuovo_numero_tavolo != -1) {
                    Tavolo *nuovo_tavolo = &tavoli[num_tavoli - 1];
                    int id_assegnato = assegna_da_lista_attesa(&coda_attesa,
                                                               &prenotazioni, &num_prenotazioni, &capacita_prenotazioni,
                                                               tavoli, num_tavoli,
                                                               nuovo_tavolo);
                    if (id_assegnato != -1) {
                        printf("✓ Cliente dalla lista d'attesa assegnato al tavolo %d!\n", nuovo_numero_tavolo);
                    }
                }
                break;
            }
            case 2:
                printf("Zona (VIP/DanceFloor/Lounge/esterno): ");
                scanf("%19s", zona);
                //convertiamo i caratteri in maiuscolo per uniformità
                for (int i = 0; zona[i] != '\0'; i++) {
                    zona[i] = toupper(zona[i]);
                }
                ricerca_tavolo_per_zona(tavoli, num_tavoli, zona);
                break;
            case 3:
                printf("Capienza minima: ");
                scanf("%d", &capienza);
                ricerca_tavolo_per_capienza(tavoli, num_tavoli, capienza);
                break;
            case 4:
                printf("Numero tavolo da modificare: ");
                scanf("%d", &capienza);
                modifica_tavolo(tavoli, num_tavoli, capienza,
                                &coda_attesa, &prenotazioni, &num_prenotazioni, &capacita_prenotazioni);
                break;
            case 5:
                printf("Numero tavolo da eliminare: ");
                scanf("%d", &capienza);
                cancella_tavolo(&tavoli, &num_tavoli, capienza);
                break;
            case 6:
                visualizza_tavoli(tavoli, num_tavoli);
                break;
        }
    } while (scelta != 0);
}

void menu_clienti() {
    int scelta, id;
    
    do {
        printf("\n=== GESTIONE CLIENTI ===\n");
        printf("1. Registra cliente\n");
        printf("2. Visualizza clienti\n");
        printf("3. Visualizza storico prenotazioni\n");
        printf("4. Elimina cliente\n");
        printf("0. Torna indietro\n");
        printf("Scelta: ");
        scanf("%d", &scelta);
        
        switch (scelta) {
            case 1:
                inserisci_cliente(&clienti, &num_clienti, &capacita_clienti);
                break;
            case 2:
                visualizza_clienti(clienti, num_clienti);
                break;
            case 3:
                printf("ID cliente: ");
                scanf("%d", &id);
                visualizza_storico_prenotazioni(clienti, num_clienti, id);
                break;
            case 4:
                printf("ID cliente da eliminare: ");
                scanf("%d", &id);
                cancella_cliente(&clienti, &num_clienti, id);
                break;
        }
    } while (scelta != 0);
}

void menu_prenotazioni() {
    int scelta, id_cliente, id_tavolo, num_persone;
    time_t data_ora;
    FasciaOraria fascia;
    
    do {
        printf("\n=== GESTIONE PRENOTAZIONI ===\n");
        printf("1. Crea prenotazione\n");
        printf("2. Visualizza prenotazioni oggi\n");
        printf("3. Cancella prenotazione\n");
        printf("4. [DEBUG] Controlla no-show con orario custom\n");
        printf("0. Torna indietro\n");
        printf("Scelta: ");
        scanf("%d", &scelta);
        
        switch (scelta) {
            case 1:
                printf("ID cliente: ");
                scanf("%d", &id_cliente);
                printf("ID tavolo: ");
                scanf("%d", &id_tavolo);
                printf("Numero persone: ");
                scanf("%d", &num_persone);
                printf("Fascia (0=apertura, 1=prime ore, 2=late night): ");
                scanf("%d", (int*)&fascia);
                
                data_ora = time(NULL);
                
                // Trova puntatori
                int idx_cliente = trova_cliente(clienti, num_clienti, id_cliente);
                int idx_tavolo = trova_tavolo(tavoli, num_tavoli, id_tavolo);
                
                if (idx_cliente == -1) {
                    printf("Errore: Cliente %d non trovato!\n", id_cliente);
                    break;
                }
                if (idx_tavolo == -1) {
                    printf("Errore: Tavolo %d non trovato!\n", id_tavolo);
                    break;
                }
                
                if (crea_prenotazione(&prenotazioni, &num_prenotazioni, &capacita_prenotazioni,
                                     tavoli, num_tavoli,
                                     &clienti[idx_cliente], &tavoli[idx_tavolo], data_ora,
                                     num_persone, fascia) == -1) {
                    printf("\n Inserire in lista d'attesa? (s/n): ");
                    char risposta;
                    scanf(" %c", &risposta);
                    if (risposta == 's') {
                        inserisci_lista_attesa(&coda_attesa,
                                              &clienti[idx_cliente], &tavoli[idx_tavolo],
                                              data_ora, num_persone, fascia);
                    }
                }
                break;
            case 2:
                data_ora = time(NULL);
                visualizza_prenotazioni_serata(prenotazioni, num_prenotazioni, data_ora);
                break;
            case 3:
                printf("ID prenotazione da cancellare: ");
                scanf("%d", &id_cliente);
                cancella_prenotazione(&prenotazioni, &num_prenotazioni,
                                     id_cliente, time(NULL),
                                     tavoli, num_tavoli,
                                     &coda_attesa,
                                     clienti, num_clienti,data_ora, fascia);
                break;
            case 4: {
                printf("\n[DEBUG] Inserisci orario di test (formato HH:MM): ");
                int ore, minuti;
                scanf("%d:%d", &ore, &minuti);
                
                // Ottieni data/ora attuale
                time_t ora_attuale = time(NULL);
                struct tm *tm_info = localtime(&ora_attuale);
                
                // Imposta l'orario desiderato mantenendo la data odierna
                tm_info->tm_hour = ore;
                tm_info->tm_min = minuti;
                tm_info->tm_sec = 0;
                
                // Converti back a time_t
                time_t ora_test = mktime(tm_info);
                
                controlla_no_show_con_orario(prenotazioni, num_prenotazioni, ora_test,
                                                clienti, num_clienti);
                break;
            }
        }
    } while (scelta != 0);
}

void salva_tutti_dati() {
    printf("\n💾 Salvataggio dati...\n");
    salva_tavoli_csv(tavoli, num_tavoli, "nightres/3) data/tavoli.csv");
    salva_clienti_csv(clienti, num_clienti, "nightres/3) data/clienti.csv");
    salva_prenotazioni_csv(prenotazioni, num_prenotazioni, "nightres/3) data/prenotazioni.csv");
    salva_storico_csv(prenotazioni, num_prenotazioni, clienti, num_clienti, "nightres/3) data/storico.csv");
    printf("✓ Dati salvati!\n");
}

void carica_tutti_dati() {
    printf("\n📂 Caricamento dati...\n");
    
    num_tavoli = carica_tavoli_csv(&tavoli, &capacita_tavoli, TAVOLI_CSV);
    num_clienti = carica_clienti_csv(&clienti, &capacita_clienti, CLIENTI_CSV);
    num_prenotazioni = carica_prenotazioni_csv(&prenotazioni, &capacita_prenotazioni, PRENOTAZIONI_CSV);
    
    // Imposta puntatori dopo caricamento
    for (int i = 0; i < num_prenotazioni; i++) {
        // Trova cliente
        for (int j = 0; j < num_clienti; j++) {
            if (clienti[j].id == prenotazioni[i].id_cliente) {
                prenotazioni[i].cliente = &clienti[j];
                break;
            }
        }
        // Trova tavolo
        for (int j = 0; j < num_tavoli; j++) {
            if (tavoli[j].numero == prenotazioni[i].id_tavolo) {
                prenotazioni[i].tavolo = &tavoli[j];
                break;
            }
        }
    }
    
    // Costruisci liste collegate per clienti
    for (int i = 0; i < num_clienti; i++) {
        clienti[i].prenotazioni = NULL;
    }
    for (int i = 0; i < num_prenotazioni; i++) {
        if (prenotazioni[i].cliente != NULL) {
            prenotazioni[i].next = prenotazioni[i].cliente->prenotazioni;
            prenotazioni[i].cliente->prenotazioni = &prenotazioni[i];
        }
    }
    
    carica_storico_csv(prenotazioni, &num_prenotazioni, clienti, num_clienti, STORICO_CSV);
    printf("✓ Dati caricati!\n");
}

int main() {
    int scelta;
    
    printf("╔════════════════════════════════════════╗\n");
    printf("║  Sistema Prenotazioni Ristorante v1.0  ║\n");
    printf("╚════════════════════════════════════════╝\n");
    
    // Carica dati al startup
    carica_tutti_dati();
    
    do {
        menu_principale();
        scanf("%d", &scelta);
        
        switch (scelta) {
            case 1:
                menu_tavoli();
                break;
            case 2:
                menu_clienti();
                break;
            case 3:
                menu_prenotazioni();
                break;
            case 4:
                visualizza_lista_attesa(&coda_attesa);
                break;
            case 5:
                visualizza_statistiche(prenotazioni, num_prenotazioni,
                                      clienti, num_clienti,
                                      tavoli, num_tavoli);
                break;
            case 0:
                printf("\nSalvataggio...\n");
                salva_tutti_dati();
                printf("Arrivederci! 👋\n");
                break;
            default:
                printf("Scelta non valida.\n");
        }
    } while (scelta != 0);
    
    return 0;
}