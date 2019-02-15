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

    if (argc != 8) {
        printf("Improper number of arguments.\n");
        return -1;
    }

    int numChairs = atoi(argv[1]);
    if (numChairs < 2 || numChairs > 4) {
        printf("There may only be 2-4 costume teams.\n");
        return -1;
    }

    int numPirates = atoi(argv[2]);
    if (numPirates < 10 || numPirates > 50) {
        printf("There may only be 10-50 pirates.\n");
        return -1;
    }

    int numNinjas = atoi(argv[3]);
    if (numNinjas < 10 || numNinjas > 50) {
        printf("There may only be 10-50 ninjas.\n");
        return -1;
    }

    int avgPCostume = atoi(argv[4]);
    int avgNCostume = atoi(argv[5]);
    if (avgPCostume < 0 || avgNCostume < 0) {
        printf("Average costume time must be positive.\n");
        return -1;
    }

    int avgPAdventure = atoi(argv[6]);
    int avgNAdventure = atoi(argv[7]);
    if (avgPAdventure < 0 || avgNAdventure < 0) {
        printf("Average arrival time must be positive.\n");
    }

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

    /***********************************************************
     * Calculate bill.
     ***********************************************************/

    int grossIncome = 0;
    int totalWait = 0;
    int totalVisits = 0;

    printf("-- Pirates --\n\n");
    for (int i = 0; i < numPirates && i < 50; i++) {
        printf("Pirate [%d] :\n", i);
        
        // Print information.
        int totalPrice = 0;
        visit_t* cur = pirates[i]->visit;
        while(cur != NULL) {
            totalVisits++;
            totalWait += cur->waitTime;
            totalPrice += cur->goldOwed;
            printf("\tDur: %d\tWait: %d\n", cur->visitTime, cur->waitTime);
            cur = cur->next;
        }
        
        // Print total gold.
        printf("\tTotal Gold Owed: %d\n", totalPrice);

        grossIncome += totalPrice; // Add the bill to the gross.
    }

    printf("\n");

    printf("-- Ninjas --\n\n");
    for (int i = 0; i < numNinjas && i < 50; i++) {
        printf("Ninja [%d] :\n", i);
        
        // Print information.
        int totalPrice = 0;
        visit_t* cur = ninjas[i]->visit;
        while(cur != NULL) {
            totalVisits++;
            totalWait += cur->waitTime;
            totalPrice += cur->goldOwed;
            printf("\tDur: %d\tWait: %d\n", cur->visitTime, cur->waitTime);
            cur = cur->next;
        }
        
        // Print total gold.
        printf("\tTotal Gold Owed: %d\n", totalPrice);

        grossIncome += totalPrice; // Add the bill to the gross.
    }
    
    printf("\n");

    printf("-- Department Statistics --\n\n");
    for (int i = 0; i < numChairs && i < 4; i++) {
        printf("Team [%d] : Busy: %ld\tFree: %ld\n", i, dept.chairs[i]->occupied, dept.chairs[i]->free);
    }

    printf("\n");

    printf("Average Queue: %f\n", (double)totalWait / (double)totalVisits);
    printf("Gross Revenue: %d\n", grossIncome);
    printf("Gold per Visit: %f\n", (double)grossIncome / (double)totalVisits);
    printf("Total Profits: %d\n", grossIncome - 5*numChairs);

    return 0;
}