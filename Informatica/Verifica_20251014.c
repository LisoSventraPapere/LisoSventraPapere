#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int* aggiuntaPunteggio(int *_array, int dim, int num){
	num=rand()%181;
	_array = (int *)realloc(_array, (dim + 1) * sizeof(int));
	if(_array == NULL){
		printf("ERRORE DI ALLOCAZIONE");
		exit (1);
	}
	_array[dim]=num;
	return _array;
}

void stampaPunteggi(int *_array, int dim){
	printf("Gli elementi dell'array sono: \n");
    for(int i = 0; i < dim; i++){ // ciclo per stampa elementi
        printf("punteggio %d- %d \n",i+1, _array[i]); // stampa elemento
    }
    printf("\n");
}

void verificaPunteggio(int *_array, int dim){
	int cnt1=0;
	int cnt2=0;
	for(int i=0; i < dim; i++){
		if(_array[i]>150)
		cnt1++;
		else if(_array[i]<80)
		cnt2++;
	}
	printf("i punteggi superiori di 150 sono %d, mentre inferiori di 80 sono %d\n",cnt1, cnt2);
}

int eliminaPunteggi(int *_array, int dim){
	printf("eliminazione punteggi inferiori di 50...\n");
            for(int i = 0; i < dim; i++){
                if(_array[i] < 50){
                    for(int j = i; j < dim - 1; j++){
                        _array[j] = _array[j + 1];
                    }
                    dim--;
                    _array = (int *)realloc(_array, dim * sizeof(int));
                    if(_array == NULL && dim > 0){
                        printf("Errore di allocazione!\n");
                        return 1;
                    }
                    i--; // per controllare l'elemento spostato
                    printf("Punteggi eliminati\n");
                }

            }
    return dim;
}

int* ordinaPunteggi(int *_array, int dim){
	int temp;
	int flag = 0;
	int i = 0;
	// Bubble sort decrescente
	while(flag == 0){
		flag = 1;
		for(int j = 0; j < dim - 1 - i; j++){
			if(_array[j] < _array[j+1]){
				flag = 0;
				temp = _array[j];
				_array[j] = _array[j+1];
				_array[j+1] = temp;
			}
		}
		i++;
	}
	return _array;
}


int main(){
	srand(time(NULL));
	int *array = NULL; //array dinamico
    int dim = 0; //quanti elementi ci sono
    int num=0;
    int cnt2=0;
	int scelta;
	do{
		printf("\t----MENU----\n");
		printf("1) Aggiungi punteggio di uno sportivo\n");
		printf("2) Visualizza i punteggi inseriti\n");
		printf("3) Visualizza quanti hanno totalizzato un punteggio superiore ai 150 e quanti un punteggio inferiore a 80\n");
		printf("4) Elimina tutti i punteggi inferiori a 50\n");
		printf("5) Ordina i punteggi restanti dal pių alto in classifica\n");
		printf("scelta: ");
		scanf("%d", &scelta);
		
		switch(scelta){
			case 1:
	                array = aggiuntaPunteggio(array, dim, num);
	                dim++;
	            break;
	        case 2:
	            	stampaPunteggi(array, dim);
	            break;
	        case 3:
	                verificaPunteggio(array, dim); //funzione che mostra quanti hanno totalizzato punteggio superiore a 150 e quanti inferiore a 80
	            break;
	        case 4:
	                dim = eliminaPunteggi(array, dim); //funzione che elimina i punteggi inferiori di 50 riallocando la memoria ogni volta all'eliminazione
	            break;
	        case 5:
	        		array = ordinaPunteggi(array, dim); //funzione di ordinamento in crescente trasformata in decrescente
	        	break;
	        default:
	        	printf("scelta non valida\n");
		}
	}while(scelta!=6);
    free (array);
	return 0;
}