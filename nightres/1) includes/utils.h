/*Ordinamento statistiche e stampa*/

#ifndef UTILS_H
#define UTILS_H

#include "tavoli.h"
#include "clienti.h"
#include "prenotazioni.h"

/* STRUTTURE PER STATISTICHE */

typedef struct {
    int id_tavolo;
    int num_prenotazioni;
} StatTavolo;

typedef struct {
    int id_cliente;
    int num_serate;
} StatCliente;

typedef struct {
    char zona[20];
    int num_richieste;
} StatZona;

typedef struct {
    float incasso_totale;
    float incasso_medio;
    int num_serate_con_prenotazioni;
} StatIncasso;

/* STATISTICHE */

/**
 * Calcola tavolo più prenotato
 * @return structure con ID e numero prenotazioni
 */
StatTavolo tavolo_piu_prenotato(Prenotazione *prenotazioni, int num_prenotazioni);

/**
 * Calcola cliente con più serate
 */
StatCliente cliente_piu_fedele(Prenotazione *prenotazioni, int num_prenotazioni);

/**
 * Calcola tasso no-show (percentuale)
 * @return percentuale no-show
 */
float calcula_tasso_no_show(Prenotazione *prenotazioni, int num_prenotazioni);

/**
 * Trova zona più richiesta
 */
StatZona zona_piu_richiesta(Prenotazione *prenotazioni, int num_prenotazioni,
                            Tavolo *tavoli, int num_tavoli);

/**
 * Calcola incasso medio per serata (considera penali)
 */
StatIncasso calcola_incasso_medio(Prenotazione *prenotazioni, int num_prenotazioni,
                                 Cliente *clienti, int num_clienti);

/**
 * Visualizza TUTTE le statistiche con barre testuali
 */
void visualizza_statistiche(Prenotazione *prenotazioni, int num_prenotazioni,
                            Cliente *clienti, int num_clienti,
                            Tavolo *tavoli, int num_tavoli);

/**
 * Funzioni ausiliarie di visualizzazione
 */
void stampa_separatore(int lunghezza);
void stampa_titolo(const char *titolo);

#endif