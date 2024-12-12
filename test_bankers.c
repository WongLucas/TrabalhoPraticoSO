#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "sync_lib.h"

#define NUM_PROCESSES 5
#define NUM_RESOURCES 3

BankersState state;

// Função para simular solicitações de processos
void* process_simulation(void* arg) {
    int process_id = *(int*)arg;
    free(arg);

    int request[NUM_RESOURCES];

    // Cada processo fará algumas solicitações aleatórias
    for (int i = 0; i < 3; i++) {
        // Gerar uma solicitação aleatória que respeite a Need
        pthread_mutex_lock(&state.lock);
        for (int j = 0; j < NUM_RESOURCES; j++) {
            request[j] = rand() % (state.Need[process_id][j] + 1);
        }
        pthread_mutex_unlock(&state.lock);

        printf("Process %d requesting resources: ", process_id);
        for (int j = 0; j < NUM_RESOURCES; j++) {
            printf("%d ", request[j]);
        }
        printf("\n");

        // Solicitar os recursos
        if (bankers_request(&state, process_id, request)) {
            printf("Process %d: Request granted\n", process_id);
        } else {
            printf("Process %d: Request denied (unsafe state)\n", process_id);
        }

        // Simular liberação de todos os recursos alocados
        pthread_mutex_lock(&state.lock);
        for (int j = 0; j < NUM_RESOURCES; j++) {
            state.Available[j] += state.Allocation[process_id][j];
            state.Allocation[process_id][j] = 0;
            state.Need[process_id][j] = state.Max[process_id][j];
        }
        pthread_mutex_unlock(&state.lock);
    }

    return NULL;
}

int main() {
    // Inicializar o estado do algoritmo dos banqueiros
    init_bankers(&state, NUM_PROCESSES, NUM_RESOURCES);

    // Configurar os recursos disponíveis
    int available_resources[NUM_RESOURCES] = {10, 5, 7};
    for (int i = 0; i < NUM_RESOURCES; i++) {
        state.Available[i] = available_resources[i];
    }

    // Configurar as matrizes Max e Allocation
    int max_resources[NUM_PROCESSES][NUM_RESOURCES] = {
        {7, 5, 3},
        {3, 2, 2},
        {9, 0, 2},
        {2, 2, 2},
        {4, 3, 3}
    };

    for (int i = 0; i < NUM_PROCESSES; i++) {
        for (int j = 0; j < NUM_RESOURCES; j++) {
            state.Max[i][j] = max_resources[i][j];
            state.Allocation[i][j] = 0;
            state.Need[i][j] = max_resources[i][j];
        }
    }

    // Criar threads para simular processos
    pthread_t threads[NUM_PROCESSES];
    for (int i = 0; i < NUM_PROCESSES; i++) {
        int* process_id = malloc(sizeof(int));
        *process_id = i;
        pthread_create(&threads[i], NULL, process_simulation, process_id);
    }

    // Aguardar threads finalizarem
    for (int i = 0; i < NUM_PROCESSES; i++) {
        pthread_join(threads[i], NULL);
    }

    // Liberar recursos alocados
    destroy_bankers(&state);

    return 0;
}
