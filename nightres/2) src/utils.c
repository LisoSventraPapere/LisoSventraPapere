/*Ordinamento statistiche e stampa*/

#include "utils.h"
#include <stdio.h>
#include <string.h>

StatTavolo tavolo_piu_prenotato(Prenotazione *prenotazioni, int num_prenotazioni) {
    StatTavolo risultato = {0, 0};
    int contatori[100] = {0};  // Supponendo max 100 tavoli
    
    for (int i = 0; i < num_prenotazioni; i++) {
        if (prenotazioni[i].stato == ATTIVA || prenotazioni[i].stato == COMPLETATA) {
            contatori[prenotazioni[i].tavolo->numero]++;
        }
    }
    
    for (int i = 0; i < 100; i++) {
        if (contatori[i] > risultato.num_prenotazioni) {
            risultato.id_tavolo = i;
            risultato.num_prenotazioni = contatori[i];
        }
    }
    
    return risultato;
}

StatCliente cliente_piu_fedele(Prenotazione *prenotazioni, int num_prenotazioni) {
    StatCliente risultato = {0, 0};
    int contatori[5000] = {0};
    
    for (int i = 0; i < num_prenotazioni; i++) {
        if (prenotazioni[i].stato == ATTIVA || prenotazioni[i].stato == COMPLETATA) {
            contatori[prenotazioni[i].cliente->id]++;
        }
    }
    
    for (int i = 0; i < 5000; i++) {
        if (contatori[i] > risultato.num_serate) {
            risultato.id_cliente = i;
            risultato.num_serate = contatori[i];
        }
    }
    
    return risultato;
}

float calcula_tasso_no_show(Prenotazione *prenotazioni, int num_prenotazioni) {
    if (num_prenotazioni == 0) return 0.0;
    
    int count_no_show = 0;
    int count_totali = 0;
    
    for (int i = 0; i < num_prenotazioni; i++) {
        if (prenotazioni[i].stato == ATTIVA || prenotazioni[i].stato == NO_SHOW) {
            count_totali++;
            if (prenotazioni[i].stato == NO_SHOW) {
                count_no_show++;
            }
        }
    }
    
    if (count_totali == 0) return 0.0;
    return (float)count_no_show / count_totali * 100.0;
}

StatZona zona_piu_richiesta(Prenotazione *prenotazioni, int num_prenotazioni,
                            Tavolo *tavoli, int num_tavoli) {
    StatZona risultato = {"", 0};
    StatZona contatori[20];
    int num_zone = 0;
    
    // Conta per zona
    for (int i = 0; i < num_prenotazioni; i++) {
        // Usa il puntatore al tavolo
        const char *zona = prenotazioni[i].tavolo->zona;
        
        // Cerca se zona già tracciata
        int trovata = 0;
        for (int k = 0; k < num_zone; k++) {
            if (strcmp(contatori[k].zona, zona) == 0) {
                contatori[k].num_richieste++;
                trovata = 1;
                break;
            }
        }
        
        if (!trovata && num_zone < 20) {
            strcpy(contatori[num_zone].zona, zona);
            contatori[num_zone].num_richieste = 1;
            num_zone++;
        }
    }
    
    // Trova massimo
    for (int i = 0; i < num_zone; i++) {
        if (contatori[i].num_richieste > risultato.num_richieste) {
            risultato = contatori[i];
        }
    }
    
    return risultato;
}

StatIncasso calcola_incasso_medio(Prenotazione *prenotazioni, int num_prenotazioni,
                                 Cliente *clienti, int num_clienti) {
    StatIncasso risultato = {0, 0, 0};
    
    // Conta serate con prenotazioni completate
    for (int i = 0; i < num_prenotazioni; i++) {
        if (prenotazioni[i].stato == COMPLETATA) {
            risultato.num_serate_con_prenotazioni++;
        }
    }
    
    // Somma penali dai clienti
    for (int i = 0; i < num_clienti; i++) {
        risultato.incasso_totale += clienti[i].penale_totale;
    }
    
    if (risultato.num_serate_con_prenotazioni > 0) {
        risultato.incasso_medio = risultato.incasso_totale / 
                                  risultato.num_serate_con_prenotazioni;
    }
    
    return risultato;
}

void stampa_separatore(int lunghezza) {
    for (int i = 0; i < lunghezza; i++) printf("=");
    printf("\n");
}

void stampa_titolo(const char *titolo) {
    printf("\n");
    stampa_separatore(50);
    printf("  %s\n", titolo);
    stampa_separatore(50);
}

void visualizza_statistiche(Prenotazione *prenotazioni, int num_prenotazioni,
                            Cliente *clienti, int num_clienti,
                            Tavolo *tavoli, int num_tavoli) {
    
    stampa_titolo("STATISTICHE GENERALI");
    
    // 1. Tavolo più prenotato
    StatTavolo tavolo_max = tavolo_piu_prenotato(prenotazioni, num_prenotazioni);
    printf("\n📊 TAVOLO PIÙ PRENOTATO\n");
    printf("Tavolo #%d: %d\n", tavolo_max.id_tavolo, tavolo_max.num_prenotazioni);
    
    // 2. Cliente più fedele
    StatCliente cliente_max = cliente_piu_fedele(prenotazioni, num_prenotazioni);
    printf("\n👤 CLIENTE PIÙ FEDELE\n");
    printf("Cliente #%d: %d serate\n", cliente_max.id_cliente, cliente_max.num_serate);
    
    // 3. Tasso no-show
    float tasso = calcula_tasso_no_show(prenotazioni, num_prenotazioni);
    printf("\n⚠️  TASSO NO-SHOW\n");
    printf("Percentuale: %.1f%%\n", tasso);
    
    // 4. Zona più richiesta
    StatZona zona_max = zona_piu_richiesta(prenotazioni, num_prenotazioni, tavoli, num_tavoli);
    printf("\n🎯 ZONA PIÙ RICHIESTA\n");
    printf("Zona %s: %d richieste\n", zona_max.zona, zona_max.num_richieste);
    
    // 5. Incasso medio
    StatIncasso incasso = calcola_incasso_medio(prenotazioni, num_prenotazioni, clienti, num_clienti);
    printf("\n💰 INCASSO MEDIO PER SERATA\n");
    printf("Incasso totale: €%.2f\n", incasso.incasso_totale);
    printf("Incasso medio: €%.2f\n", incasso.incasso_medio);
}