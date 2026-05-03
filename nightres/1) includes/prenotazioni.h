/*Logica prenotazioni attesa e no-show*/

#ifndef PRENOTAZIONI_H
#define PRENOTAZIONI_H

#define PRENOTAZIONI_CSV "nightres/3) data/prenotazioni.csv"
#define STORICO_CSV "nightres/3) data/storico.csv"

#include <time.h>
#include "tavoli.h"
#include "clienti.h"

/* Fasce orarie */
typedef enum {
    APERTURA,    // 20:00 - 22:00
    PRIME_ORE,   // 22:00 - 00:00
    LATE_NIGHT   // 00:00 - 04:00
} FasciaOraria;

/* Stato prenotazione */
typedef enum {
    ATTIVA,
    COMPLETATA,
    CANCELLATA,
    NO_SHOW
} StatoPrenotazione;

/* Struttura prenotazione */
typedef struct Prenotazione {
    int id;
    int id_cliente;  // Per persistenza
    int id_tavolo;   // Per persistenza
    Cliente *cliente;  // Puntatore al cliente
    Tavolo *tavolo;    // Puntatore al tavolo
    time_t data_ora;              // Data/ora prenotazione (inizio turno)
    int numero_persone;
    FasciaOraria fascia;
    StatoPrenotazione stato;
    time_t ora_scadenza_no_show;  // turno + 30 minuti
    time_t data_cancellazione;    // orario cancellazione per storico
    float penale_applicata;       // penale calcolata alla cancellazione
    struct Prenotazione *next;    // Per lista collegata per cliente
} Prenotazione;

/* Struttura lista d'attesa (FIFO) */
typedef struct ElementoAttesa {
    int id_cliente;  // Per persistenza
    int id_tavolo;   // Per persistenza
    Cliente *cliente;  // Puntatore al cliente
    Tavolo *tavolo;    // Puntatore al tavolo
    time_t data_ora;
    int numero_persone;
    FasciaOraria fascia;
    struct ElementoAttesa *next;  // Per coda FIFO
} ElementoAttesa;

/* Struttura coda FIFO per lista d'attesa */
typedef struct CodaAttesa {
    ElementoAttesa *testa;
    ElementoAttesa *coda;
    int dimensione;
} CodaAttesa;
/* FUNZIONI PRENOTAZIONI */

float cancella_prenotazione(Prenotazione **prenotazioni, int *num_prenotazioni,
                            int id_prenotazione, time_t ora_cancellazione,
                            Tavolo *tavoli, int num_tavoli,
                            CodaAttesa *coda_attesa,
                            Cliente *clienti, int num_clienti, time_t data_ora, FasciaOraria fascia);

/**
 * Crea una nuova prenotazione con controllo disponibilità
 * @return ID prenotazione creata, -1 se fallisce
 */
int crea_prenotazione(Prenotazione **prenotazioni, int *num_prenotazioni, int *capacita_prenotazioni,
                      Tavolo *tavoli, int num_tavoli,
                      Cliente *cliente, Tavolo *tavolo, 
                      time_t data_ora, int num_persone, FasciaOraria fascia);

/**
 * Verifica disponibilità del tavolo per la fascia oraria
 * @return 1 se disponibile, 0 altrimenti
 */
int controlla_disponibilita(Prenotazione *prenotazioni, int num_prenotazioni,
                            Tavolo *tavolo, time_t data_ora, FasciaOraria fascia);

/**
 * Calcola orario scadenza no-show (turno + 30 minuti)
 */
time_t calcola_scadenza_no_show(time_t ora_turno);

/**
 * Cancella prenotazione e calcola penale
 * @return importo penale, -1 se fallisce
 */

/**
 * Calcola penale in base al tempo di anticipo cancellazione
 */
float calcola_penale(time_t ora_creazione, time_t ora_cancellazione,
                     time_t ora_prenotazione);

/**
 * Calcola l'orario di inizio del turno in base alla fascia oraria
 */
time_t calcola_orario_inizio_fascia(time_t ora_rif, FasciaOraria fascia);

/**
 * Visualizza prenotazioni della serata corrente (filtrate per data)
 */
void visualizza_prenotazioni_serata(Prenotazione *prenotazioni, 
                                    int num_prenotazioni, time_t data_serata);

/**
 * Controlla quali prenotazioni sono no-show e aggiorna stato
 */
void controlla_no_show(Prenotazione *prenotazioni, int num_prenotazioni);

/**
 * Controlla no-show con orario personalizzato (per testing)
 */
void controlla_no_show_con_orario(Prenotazione *prenotazioni, int num_prenotazioni, time_t ora_test,
                                     Cliente *clienti, int num_clienti);

/* FUNZIONI LISTA D'ATTESA */

/**
 * Inserisce cliente in lista d'attesa (coda FIFO)
 */
void inserisci_lista_attesa(CodaAttesa *coda_attesa,
                            Cliente *cliente, Tavolo *tavolo,
                            time_t data_ora, int num_persone, FasciaOraria fascia);

/**
 * Assegna automaticamente prenotazione al primo in attesa
 * (chiamato quando un tavolo si libera)
 * @return ID nuova prenotazione, -1 se fallisce
 */
int assegna_da_lista_attesa(CodaAttesa *coda_attesa,
                            Prenotazione **prenotazioni, int *num_prenotazioni, int *capacita_prenotazioni,
                            Tavolo *tavoli, int num_tavoli, Tavolo *tavolo);

/**
 * Visualizza lista d'attesa FIFO
 */
void visualizza_lista_attesa(CodaAttesa *coda_attesa);

/**
 * Converte time_t in struct tm *
 */
struct tm *converti_time_t_in_tm(time_t data_ora);

/**
 * Formatta time_t in stringa "HH:MM"
 */
void formatta_orario(time_t t, char *buffer, size_t size);

#endif