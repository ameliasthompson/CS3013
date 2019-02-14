// Maximilian Thompson

#include <sched.h>
#include <stdlib.h>

#include "department.h"
#include "human.h"

human_t* queue[2];
sem_t queueSem[2];

department_t dept;

sem_t vCountSem;
int visitsLeft;

human_t* pirates[50];
human_t* ninjas[50];

pthread_t pirThreads[50];
pthread_t ninThreads[50];

#ifdef _DEBUG
int check_sem(sem_t* sem) {
    int v;
    sem_getvalue(sem, &v);
    return v;
}
#endif

int main (int argc, char** argv) {
    srand48(time(NULL));
    sem_init(&queueSem[0], 0, 1);
    sem_init(&queueSem[1], 0, 1);
    sem_init(&vCountSem, 0, 1);

    // DO INPUT
    int numPirates = 10;
    int avgPCostume = 12;
    int avgPAdventure = 8;
    int avgNCostume = 8;
    int avgNAdventure = 12;
    int numNinjas = 10;
    int numChairs = 2;

    visitsLeft = numPirates + numNinjas;

    // Determine starting alignment with a coin flip.
    int startAlignment;
    if (drand48() <= 0.50) {
        startAlignment = PIRATE;
    } else {
        startAlignment = NINJA;
    }

    init_department(&dept, startAlignment, numChairs);

    // Get the party started.
    for (int i = 0; i < 50; i++) {
        if (i < numPirates) {
            pirates[i] = create_human(i, PIRATE, avgPCostume, avgPAdventure);
            pthread_create(&pirThreads[i], NULL, human_main, pirates[i]);
        } else {
            pirates[i] = NULL;
            // We're never actually iterating over the threads, so those don't need to NULL.
        }

        if (i < numNinjas) {
            ninjas[i] = create_human(i + 50, NINJA, avgNCostume, avgNAdventure);
            pthread_create(&ninThreads[i], NULL, human_main, ninjas[i]);
        } else {
            pirates[i] = NULL;
        }
    }

    while (visitsLeft > 0) sched_yield();

    #ifdef _DEBUG
        printf("DEBUG: Finished servicing all people.\n");
    #endif

    return 0;
}