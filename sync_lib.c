#include "sync_lib.h"
#include <stdlib.h>
#include <string.h>
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

void init_bankers(BankersState *state, int num_processes, int num_resources) {
    state->num_processes = num_processes;
    state->num_resources = num_resources;

    state->Available = (int *)malloc(num_resources * sizeof(int));
    state->Max = (int **)malloc(num_processes * sizeof(int *));
    state->Allocation = (int **)malloc(num_processes * sizeof(int *));
    state->Need = (int **)malloc(num_processes * sizeof(int *));
    
    for (int i = 0; i < num_processes; i++) {
        state->Max[i] = (int *)malloc(num_resources * sizeof(int));
        state->Allocation[i] = (int *)malloc(num_resources * sizeof(int));
        state->Need[i] = (int *)malloc(num_resources * sizeof(int));
    }

    pthread_mutex_init(&state->lock, NULL);
    pthread_cond_init(&state->condition, NULL);
}

void destroy_bankers(BankersState *state) {
    for (int i = 0; i < state->num_processes; i++) {
        free(state->Max[i]);
        free(state->Allocation[i]);
        free(state->Need[i]);
    }
    free(state->Max);
    free(state->Allocation);
    free(state->Need);
    free(state->Available);

    pthread_mutex_destroy(&state->lock);
    pthread_cond_destroy(&state->condition);
}

bool is_safe_state(BankersState *state) {
    int *Work = (int *)malloc(state->num_resources * sizeof(int));
    bool *Finish = (bool *)malloc(state->num_processes * sizeof(bool));
    memcpy(Work, state->Available, state->num_resources * sizeof(int));
    memset(Finish, 0, state->num_processes * sizeof(bool));

    for (int i = 0; i < state->num_processes; i++) {
        if (!Finish[i]) {
            bool can_proceed = true;
            for (int j = 0; j < state->num_resources; j++) {
                if (state->Need[i][j] > Work[j]) {
                    can_proceed = false;
                    break;
                }
            }
            if (can_proceed) {
                for (int j = 0; j < state->num_resources; j++) {
                    Work[j] += state->Allocation[i][j];
                }
                Finish[i] = true;
                i = -1; // Restart the loop
            }
        }
    }

    bool safe = true;
    for (int i = 0; i < state->num_processes; i++) {
        if (!Finish[i]) {
            safe = false;
            break;
        }
    }

    free(Work);
    free(Finish);
    return safe;
}

bool bankers_request(BankersState *state, int process_id, int *request) {
    pthread_mutex_lock(&state->lock);

    // Check if request exceeds Need
    for (int i = 0; i < state->num_resources; i++) {
        if (request[i] > state->Need[process_id][i]) {
            pthread_mutex_unlock(&state->lock);
            return false;
        }
    }

    // Check if request exceeds Available
    for (int i = 0; i < state->num_resources; i++) {
        if (request[i] > state->Available[i]) {
            pthread_cond_wait(&state->condition, &state->lock);
        }
    }

    // Tentatively allocate resources
    for (int i = 0; i < state->num_resources; i++) {
        state->Available[i] -= request[i];
        state->Allocation[process_id][i] += request[i];
        state->Need[process_id][i] -= request[i];
    }

    // Check if the state is safe
    if (!is_safe_state(state)) {
        // Rollback allocation
        for (int i = 0; i < state->num_resources; i++) {
            state->Available[i] += request[i];
            state->Allocation[process_id][i] -= request[i];
            state->Need[process_id][i] += request[i];
        }
        pthread_mutex_unlock(&state->lock);
        return false;
    }

    pthread_cond_broadcast(&state->condition);
    pthread_mutex_unlock(&state->lock);
    return true;
}