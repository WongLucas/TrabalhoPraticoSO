#include "sync_lib.h"

// Função para inicializar o semáforo
void sem_init(semaphore_t *sem, int value) {
    sem->value = value;
    pthread_mutex_init(&sem->lock, NULL);
    pthread_cond_init(&sem->cond, NULL);
}

// Função para "esperar" (decrementar o semáforo)
void sem_wait(semaphore_t *sem) {
    pthread_mutex_lock(&sem->lock);
    while (sem->value <= 0) {
        pthread_cond_wait(&sem->cond, &sem->lock);
    }
    sem->value--;
    pthread_mutex_unlock(&sem->lock);
}

// Função para "postar" (incrementar o semáforo)
void sem_post(semaphore_t *sem) {
    pthread_mutex_lock(&sem->lock);
    sem->value++;
    pthread_cond_signal(&sem->cond);
    pthread_mutex_unlock(&sem->lock);
}