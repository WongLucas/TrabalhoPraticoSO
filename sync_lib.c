#include "sync_lib.h"
#include <stdio.h>

void sem_init(semaphore_t *sem, int value) {
    sem->value = value;
    pthread_mutex_init(&sem->lock, NULL);
    pthread_cond_init(&sem->cond, NULL);
}

void sem_wait(semaphore_t *sem) {
    pthread_mutex_lock(&sem->lock);
    while (sem->value == 0) {
        pthread_cond_wait(&sem->cond, &sem->lock);
    }
    sem->value--;
    pthread_mutex_unlock(&sem->lock);
}

void sem_post(semaphore_t *sem) {
    pthread_mutex_lock(&sem->lock);
    sem->value++;
    pthread_cond_signal(&sem->cond);
    pthread_mutex_unlock(&sem->lock);
}
