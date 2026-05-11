/*Lettura e scrittura CSV*/

#ifndef FILE_IO_H
#define FILE_IO_H

#include "tavoli.h"
#include "clienti.h"
#include "prenotazioni.h"

/* SALVATAGGIO CSV */

/**
 * Salva tavoli su CSV (tabella,numero,capienza,prezzo_minimo,zona)
 */
void salva_tavoli_csv(Tavolo *tavoli, int num_tavoli, const char *filename);

/**
 * Salva clienti su CSV (clienti.csv: id,nome,cognome,livello_fedelta)
 */
void salva_clienti_csv(Cliente *clienti, int num_clienti, const char *filename);

/**
 * Salva prenotazioni su CSV (prenotazioni.csv: dettagli + stato + no-show)
 */
void salva_prenotazioni_csv(Prenotazione *prenotazioni, int num_prenotazioni,
                            const char *filename);

/**
 * Salva storico cancellazioni su CSV (storico.csv: id_prenotazione, penale, data)
 */
void salva_storico_csv(Prenotazione *prenotazioni, int num_prenotazioni,
                       Cliente *clienti, int num_clienti,
                       const char *filename);

/* CARICAMENTO CSV - PARSING MANUALE */

/**
 * Carica tavoli da CSV (usa fgets, fscanf, strtok)
 * @return numero tavoli caricati
 */
int carica_tavoli_csv(Tavolo **tavoli, int *capacita_tavoli, const char *filename);

/**
 * Carica clienti da CSV (parsing riga per riga)
 * @return numero clienti caricati
 */
int carica_clienti_csv(Cliente **clienti, int *capacita_clienti, const char *filename);

/**
 * Carica prenotazioni da CSV
 * @return numero prenotazioni caricate
 */
int carica_prenotazioni_csv(Prenotazione **prenotazioni, int *capacita_prenotazioni, const char *filename);

/**
 * Converte stringa time a time_t
 */
time_t stringa_a_time(const char *stringa);

/**
 * Converte time_t a stringa
 */
void time_a_stringa(time_t t, char *buffer, size_t size);

#endif