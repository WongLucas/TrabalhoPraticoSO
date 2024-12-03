#ifndef SYNC_LIB_H
#define SYNC_LIB_H

#include <pthread.h>

// Definição da estrutura do semáforo
typedef struct {
    int value;                 // Contador do semáforo
    pthread_mutex_t lock;      // Mutex para proteger o semáforo
    pthread_cond_t cond;       // Variável de condição para sincronização
} semaphore_t;

// Declarações das funções
void sem_init(semaphore_t *sem, int value);
void sem_wait(semaphore_t *sem);
void sem_post(semaphore_t *sem);

#endif // SYNC_LIB_H