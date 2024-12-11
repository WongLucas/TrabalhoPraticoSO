#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>

#define BUFFER_SIZE 20

int buffer[BUFFER_SIZE];
int count = 0;

sem_t empty_slots; // Semáforo para slots vazios
sem_t full_slots;  // Semáforo para slots preenchidos
pthread_mutex_t buffer_lock; // Mutex para proteger o buffer

void *producer(void *arg) {
    for (int i = 0; i < 100; i++) { // Produz 100 itens
        sem_wait(&empty_slots); // Aguarda espaço no buffer
        pthread_mutex_lock(&buffer_lock); // Bloqueia o acesso ao buffer

        buffer[count++] = i; // Adiciona item ao buffer
        printf("Produced: %d\n", i);

        pthread_mutex_unlock(&buffer_lock); // Libera o acesso ao buffer
        sem_post(&full_slots); // Incrementa o número de itens disponíveis
    }
    return NULL;
}

void *consumer(void *arg) {
    for (int i = 0; i < 100; i++) { // Consome 100 itens
        sem_wait(&full_slots); // Aguarda itens no buffer
        pthread_mutex_lock(&buffer_lock); // Bloqueia o acesso ao buffer

        int item = buffer[--count]; // Remove item do buffer
        printf("Consumed: %d\n", i);

        pthread_mutex_unlock(&buffer_lock); // Libera o acesso ao buffer
        sem_post(&empty_slots); // Incrementa o número de slots vazios
    }
    return NULL;
}

int main() {
    pthread_t producer_thread, consumer_thread;

    // Inicializa os semáforos e o mutex
    sem_init(&empty_slots, 0, BUFFER_SIZE);
    sem_init(&full_slots, 0, 0);
    pthread_mutex_init(&buffer_lock, NULL);

    // Inicia o temporizador
    clock_t start_time = clock();

    // Cria as threads de produtor e consumidor
    pthread_create(&producer_thread, NULL, producer, NULL);
    pthread_create(&consumer_thread, NULL, consumer, NULL);

    // Aguarda as threads terminarem
    pthread_join(producer_thread, NULL);
    pthread_join(consumer_thread, NULL);

    // Para o temporizador
    clock_t end_time = clock();

    // Calcula o tempo de execução
    double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("Elapsed time: %.10f seconds\n", elapsed_time);

    // Destroi os semáforos e o mutex
    sem_destroy(&empty_slots);
    sem_destroy(&full_slots);
    pthread_mutex_destroy(&buffer_lock);

    printf("All operations completed successfully.\n");
    return 0;
}