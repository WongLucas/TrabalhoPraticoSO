#ifndef SYNC_LIB_H
#define SYNC_LIB_H

#include <pthread.h>

// Estrutura do semáforo
typedef struct {
    int value;
    pthread_mutex_t lock;
    pthread_cond_t cond;
} semaphore_t;

// Funções do semáforo
void sem_init(semaphore_t *sem, int value);
void sem_wait(semaphore_t *sem);
void sem_post(semaphore_t *sem);

#endif // SYNC_LIB_H
