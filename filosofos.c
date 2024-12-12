#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "sync_lib.h"

#define NUM_PHILOSOPHERS 5

typedef enum { THINKING, HUNGRY, EATING } State;

typedef struct {
    State states[NUM_PHILOSOPHERS];
    semaphore_t semaphores[NUM_PHILOSOPHERS];
    pthread_mutex_t mutex;
} Philosophers;

Philosophers table;

void test(int philosopher_id) {
    int left = (philosopher_id + NUM_PHILOSOPHERS - 1) % NUM_PHILOSOPHERS;
    int right = (philosopher_id + 1) % NUM_PHILOSOPHERS;

    if (table.states[philosopher_id] == HUNGRY &&
        table.states[left] != EATING &&
        table.states[right] != EATING) {

        table.states[philosopher_id] = EATING;
        sem_post(&table.semaphores[philosopher_id]);
    }
}

void take_forks(int philosopher_id) {
    pthread_mutex_lock(&table.mutex);

    table.states[philosopher_id] = HUNGRY;
    printf("Philosopher %d is HUNGRY\n", philosopher_id);

    test(philosopher_id);

    pthread_mutex_unlock(&table.mutex);

    sem_wait(&table.semaphores[philosopher_id]);
}

void put_forks(int philosopher_id) {
    pthread_mutex_lock(&table.mutex);

    table.states[philosopher_id] = THINKING;
    printf("Philosopher %d is THINKING\n", philosopher_id);

    int left = (philosopher_id + NUM_PHILOSOPHERS - 1) % NUM_PHILOSOPHERS;
    int right = (philosopher_id + 1) % NUM_PHILOSOPHERS;

    test(left);
    test(right);

    pthread_mutex_unlock(&table.mutex);
}

void* philosopher(void* arg) {
    int philosopher_id = *(int*)arg;
    free(arg);

    while (1) {
        // Pensando
        printf("Philosopher %d is THINKING\n", philosopher_id);
        sleep(rand() % 3 + 1);

        // Pegar garfos
        take_forks(philosopher_id);

        // Comendo
        printf("Philosopher %d is EATING\n", philosopher_id);
        sleep(rand() % 3 + 1);

        // Devolver garfos
        put_forks(philosopher_id);
    }

    return NULL;
}

int main() {
    pthread_t philosophers[NUM_PHILOSOPHERS];

    // Inicializar a estrutura
    pthread_mutex_init(&table.mutex, NULL);
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        table.states[i] = THINKING;
        sem_init(&table.semaphores[i], 0);
    }

    // Criar threads para os filósofos
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        int* philosopher_id = malloc(sizeof(int));
        *philosopher_id = i;
        pthread_create(&philosophers[i], NULL, philosopher, philosopher_id);
    }

    // Aguardar as threads (nunca termina, pois os filósofos continuam rodando)
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_join(philosophers[i], NULL);
    }

    // Destruir os semáforos e mutex
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_mutex_destroy(&table.mutex);
    }

    return 0;
}
