#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "sync_lib.h"
#include <unistd.h>

#define NUM_READERS 5
#define NUM_WRITERS 2

// Estrutura compartilhada para sincronização
typedef struct {
    int data; // Dados compartilhados
    int readers_count; // Contador de leitores
    semaphore_t resource; // Controle de acesso ao recurso
    semaphore_t readers_mutex; // Proteção para o contador de leitores
} SharedResource;

SharedResource shared;

// Função para leitores
void* reader(void* arg) {
    int reader_id = *(int*)arg;
    free(arg);

    while (1) {
        // Entrada na região crítica para leitores
        sem_wait(&shared.readers_mutex);
        shared.readers_count++;
        if (shared.readers_count == 1) {
            // Primeiro leitor bloqueia o recurso
            sem_wait(&shared.resource);
        }
        sem_post(&shared.readers_mutex);

        // Leitura
        printf("Reader %d is reading data: %d\n", reader_id, shared.data);
        sleep(rand() % 2 + 1);

        // Saída da região crítica para leitores
        sem_wait(&shared.readers_mutex);
        shared.readers_count--;
        if (shared.readers_count == 0) {
            // Último leitor libera o recurso
            sem_post(&shared.resource);
        }
        sem_post(&shared.readers_mutex);

        // Simula um intervalo entre leituras
        sleep(rand() % 3 + 1);
    }

    return NULL;
}

// Função para escritores
void* writer(void* arg) {
    int writer_id = *(int*)arg;
    free(arg);

    while (1) {
        // Entrada na região crítica para escritores
        sem_wait(&shared.resource);

        // Escrita
        shared.data = rand() % 100;
        printf("Writer %d is writing data: %d\n", writer_id, shared.data);
        sleep(rand() % 2 + 1);

        // Saída da região crítica para escritores
        sem_post(&shared.resource);

        // Simula um intervalo entre escritas
        sleep(rand() % 3 + 1);
    }

    return NULL;
}

int main() {
    pthread_t readers[NUM_READERS], writers[NUM_WRITERS];

    // Inicializa a estrutura compartilhada
    shared.data = 0;
    shared.readers_count = 0;
    sem_init(&shared.resource, 1);
    sem_init(&shared.readers_mutex, 1);

    // Cria threads para leitores
    for (int i = 0; i < NUM_READERS; i++) {
        int* reader_id = malloc(sizeof(int));
        *reader_id = i;
        pthread_create(&readers[i], NULL, reader, reader_id);
    }

    // Cria threads para escritores
    for (int i = 0; i < NUM_WRITERS; i++) {
        int* writer_id = malloc(sizeof(int));
        *writer_id = i;
        pthread_create(&writers[i], NULL, writer, writer_id);
    }

    // Aguarda as threads (nunca termina neste caso)
    for (int i = 0; i < NUM_READERS; i++) {
        pthread_join(readers[i], NULL);
    }
    for (int i = 0; i < NUM_WRITERS; i++) {
        pthread_join(writers[i], NULL);
    }

    return 0;
}
