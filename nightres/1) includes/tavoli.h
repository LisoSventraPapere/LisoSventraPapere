/*CRUD del catalogo tavoli*/

/***
 * struct per la gestione dei tavoli
 * Funzione per inserire un nuovo tavolo e inserire il numero del tavolo, la capienza, prezzo minimo di consumazione e la sua zona (VIP, DanceFloor, Lounge, esterno)
 * funzione per la ricerca di un tavolo in basea alla sua zona la ricerca scorre l'array e usa confronti numerici e strstr per le zone
 * Funzione per la modifica di un tavolo
 * Funzione per cancellare un tavolo
 */

#ifndef TAVOLI_H
#define TAVOLI_H

#define TAVOLI_CSV "nightres/3) data/tavoli.csv"

typedef struct Prenotazione Prenotazione;
typedef struct CodaAttesa CodaAttesa;

typedef struct {
    int numero;
    int capienza;
    float prezzo_minimo;
    char zona[20];  // VIP, DanceFloor, Lounge, esterno
} Tavolo;

/* CRUD */

/**
 * Inserisce nuovo tavolo
 * @return numero tavolo appena creato, -1 se errore
 */
int inserisci_tavolo(Tavolo **tavoli, int *num_tavoli, int *capacita_tavoli);

/**
 * Ricerca tavoli per zona (usa strstr)
 */
void ricerca_tavolo_per_zona(Tavolo *tavoli, int num_tavoli, char *zona);

/**
 * Ricerca tavoli per capienza minima (confronto numerico)
 */
void ricerca_tavolo_per_capienza(Tavolo *tavoli, int num_tavoli, int capienza_minima);

/**
 * Modifica un tavolo (numero, capienza, prezzo, zona)
 */
void modifica_tavolo(Tavolo *tavoli, int num_tavoli, int numero_tavolo,
                     CodaAttesa *coda_attesa,
                     Prenotazione **prenotazioni, int *num_prenotazioni, int *capacita_prenotazioni);

/**
 * Elimina un tavolo
 */
void cancella_tavolo(Tavolo **tavoli, int *num_tavoli, int numero_tavolo);

/**
 * Trova un tavolo per numero
 * @return indice nel array, -1 se non trovato
 */
int trova_tavolo(Tavolo *tavoli, int num_tavoli, int numero_tavolo);

/**
 * Visualizza tutti i tavoli
 */
void visualizza_tavoli(Tavolo *tavoli, int num_tavoli);

/**
 * Trova un tavolo per ID/numero
 * @return indice nel array, -1 se non trovato
 */
int trova_tavolo(Tavolo *tavoli, int num_tavoli, int numero_tavolo);

#endif