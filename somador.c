#include <stdlib.h>
#include <stdio.h>
#include <time.h> 
#include <pthread.h>

pthread_mutex_t m;
int *vet = NULL;
int total = 0;

int *inicializaVetor(int *vet, int size);
void *soma(void *divisor);
void *somaLocal(void *divisor);

typedef struct args {
    int divisor;
    int nThread;
    int nVet;
    int soma;//usado para a opção de soma parcial
} args;

int main(int argc, char *argv[]) {
    int tamVet = atoi(argv[1]); //tamanho do vetor
    int numThreads = atoi(argv[2]); //número de threads
    int opcao = atoi(argv[3]); //opção de execução: 0 para soma compatilhada (c/ mutex)
                                                  //1 para soma local (s/mutex) 
    pthread_t tids[numThreads];
    struct args *divisorVetor = (args*) malloc(numThreads * sizeof (args));
    
    srand(time(0));

    vet = inicializaVetor(vet, tamVet);

    //for (int i = 0; i < tamVet; i++) {
    //    printf("%d\n", vet[i]);
    //}

    for (int i = 0; i < numThreads; i++) {
        divisorVetor[i].divisor = i;
        divisorVetor[i].nThread = numThreads;
        divisorVetor[i].nVet = tamVet;
    }
    
    if(!opcao){//opcao soma compartilhada
        for (int i = 0; i < numThreads; i++) {
            pthread_create(&tids[i], NULL, soma, (void *) &divisorVetor[i]);
        }
    }else{//opcao soma local
        for (int i = 0; i < numThreads; i++) {
            pthread_create(&tids[i], NULL, somaLocal, (void *) &divisorVetor[i]);
        }
    }
    
    for(int i = 0 ; i < numThreads ; i++ ){
        pthread_join(tids[i],NULL);
    }

    if(opcao){//opcao soma local
        for(int i = 0; i < numThreads; i++){
            total += divisorVetor[i].soma;
        }
    }
    
    printf("Soma: %d\n", total);
    free(vet);
    free(divisorVetor);
}

int *inicializaVetor(int *vet, int size) {
    vet = (int*) malloc(size * sizeof (int));
    for (int i = 0; i < size; i++) {
        //vet[i] = rand() % 10;
        vet[i] = rand() % 100;
        //vet[i] = i;
    }
    return vet;
}

void *soma(void *divisor) {
    int div = (*(args*) divisor).divisor;
    int tam = (*(args*) divisor).nVet / (*(args*) divisor).nThread;
    int aux = 0;

    for (int i = div * tam; i < (div + 1) * tam; i++) {
        aux += vet[i];
    }
    
    //printf("%d\n", aux);
    pthread_mutex_lock(&m);
    total += aux;
    pthread_mutex_unlock(&m);
}

void *somaLocal(void *divisor) {
    int div = (*(args*) divisor).divisor;
    int tam = (*(args*) divisor).nVet / (*(args*) divisor).nThread;
    int aux = 0;

    for (int i = div * tam; i < (div + 1) * tam; i++) {
        aux += vet[i];
    }
    
    (*(args*) divisor).soma = aux;
    //printf("%d\n", (*(args*) divisor).soma);
}