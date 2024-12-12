#ifndef SYNC_LIB_H
#define SYNC_LIB_H

#include <pthread.h>
#include <stdbool.h>

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

typedef struct {
    int *Available;      // Vetor de recursos disponíveis
    int **Max;           // Matriz de recursos máximos que cada processo pode requisitar
    int **Allocation;    // Matriz de recursos alocados atualmente
    int **Need;          // Matriz de recursos necessários
    int num_processes;   // Número de processos
    int num_resources;   // Número de tipos de recursos
    pthread_mutex_t lock; // Mutex para proteger as operações no algoritmo
    pthread_cond_t condition; // Condição para processos esperando recursos
} BankersState;

// Inicializa o estado do algoritmo dos banqueiros
void init_bankers(BankersState *state, int num_processes, int num_resources);

// Libera recursos do algoritmo dos banqueiros
void destroy_bankers(BankersState *state);

// Função para solicitar recursos
bool bankers_request(BankersState *state, int process_id, int *request);


#endif // SYNC_LIB_H
