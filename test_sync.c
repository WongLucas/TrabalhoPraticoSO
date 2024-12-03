#include <stdio.h>
#include <pthread.h>
#include "sync_lib.h"

#define BUFFER_SIZE 10

int buffer[BUFFER_SIZE];
int in = 0, out = 0;

semaphore_t empty_slots;
semaphore_t full_slots;
pthread_mutex_t buffer_lock;

void* producer(void* arg) {
    for (int i = 0; i < 20; i++) {
        sem_wait(&empty_slots);
        pthread_mutex_lock(&buffer_lock);

        buffer[in] = i;
        printf("Produced: %d\n", i);
        in = (in + 1) % BUFFER_SIZE;

        pthread_mutex_unlock(&buffer_lock);
        sem_post(&full_slots);
    }
    return NULL;
}

void* consumer(void* arg) {
    for (int i = 0; i < 20; i++) {
        sem_wait(&full_slots);
        pthread_mutex_lock(&buffer_lock);

        int item = buffer[out];
        printf("Consumed: %d\n", item);
        out = (out + 1) % BUFFER_SIZE;

        pthread_mutex_unlock(&buffer_lock);
        sem_post(&empty_slots);
    }
    return NULL;
}

int main() {
    pthread_t producers[2], consumers[3];

    sem_init(&empty_slots, BUFFER_SIZE);
    sem_init(&full_slots, 0);
    pthread_mutex_init(&buffer_lock, NULL);

    for (int i = 0; i < 2; i++) {
        pthread_create(&producers[i], NULL, producer, NULL);
    }
    for (int i = 0; i < 3; i++) {
        pthread_create(&consumers[i], NULL, consumer, NULL);
    }

    for (int i = 0; i < 2; i++) {
        pthread_join(producers[i], NULL);
    }
    for (int i = 0; i < 3; i++) {
        pthread_join(consumers[i], NULL);
    }

    pthread_mutex_destroy(&buffer_lock);
    return 0;
}