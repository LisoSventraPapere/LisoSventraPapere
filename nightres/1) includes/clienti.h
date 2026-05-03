/*Gestione anagrafica clienti */

/***
 * struttura dati per la gestione dei clienti
 * Funzione per inserire un nuovo cliente e inserire l'id univoco del cliente e il suo livello di fedeltà (standard, gold, VIP)
 * Funzione per cancellare un cliente
 * Funzione per visualizzare lo storico delle prenotazioni di un cliente
*/

#ifndef CLIENTI_H
#define CLIENTI_H

#define CLIENTI_CSV "nightres/3) data/clienti.csv"

typedef struct Prenotazione Prenotazione;  // Forward declaration

typedef struct {
    int id;
    char nome[50];
    char cognome[50];
    char livello_fedelta[20];  // standard, gold, VIP
    float penale_totale;      // Totale penali accumulate
    Prenotazione *prenotazioni; // Testa della lista collegata delle prenotazioni
} Cliente;

/* CRUD */

/**
 * Registra nuovo cliente (ID generato automaticamente)
 */
void inserisci_cliente(Cliente **clienti, int *num_clienti, int *capacita_clienti);

/**
 * Cancella un cliente
 */
void cancella_cliente(Cliente **clienti, int *num_clienti, int id_cliente);

/**
 * Visualizza storico prenotazioni di un cliente
 */
void visualizza_storico_prenotazioni(Cliente *clienti, int num_clienti,
                                     int id_cliente);

/**
 * Visualizza tutti i clienti
 */
void visualizza_clienti(Cliente *clienti, int num_clienti);

/**
 * Trova un cliente per ID
 * @return indice nel array, -1 se non trovato
 */
int trova_cliente(Cliente *clienti, int num_clienti, int id_cliente);

/**
 * Genera automaticamente nuovo ID univoco cliente
 */
int genera_id_cliente(Cliente *clienti, int num_clienti);

#endif
