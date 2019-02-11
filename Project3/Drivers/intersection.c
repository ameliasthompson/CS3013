// Maximilian Thompson

#include <stdlib.h>
#include <pthread.h>

#include "carThread.h"

pthread_mutex_t intersectionLock; // Mutex for checking if the intersection can be crossed.
pthread_mutex_t quadrantLock[4];  // Mutex for each quadrant of the intersection.
pthread_mutex_t queueLock[4];     // Mutex for each queue.

car_t* queue[4];                  // The queues of cars.

int quadrant[4];                  // What car is occupying each quadrant.

int main() {
    srand(time(NULL)); // Seed random number generator.

    // Init the locks and queues.
    pthread_mutex_init(&intersectionLock, NULL);
    for (int i = 0; i < 4; i++) {
        pthread_mutex_init(&quadrantLock[i], NULL);
        pthread_mutex_init(&queueLock[i], NULL);
        queue[i] = NULL;
        quadrant[i] = -1;
    }


    // Assemble vehicles.
    pthread_t threads[NUM_CARS];
    pthread_mutex_lock(&intersectionLock); // We should probably hold this to be safe.
    for (int i = 0; i < NUM_CARS; i++)
        pthread_create(&threads[i], NULL, car_main, create_car(i));
    pthread_mutex_unlock(&intersectionLock); // Now the cars can go!

    // Sleep forever.
    pthread_mutex_t mainMutex;
    pthread_cond_t mainCond;
    pthread_mutex_init(&mainMutex, NULL);
    pthread_cond_init(&mainCond, NULL);
    pthread_mutex_lock(&mainMutex);
    pthread_cond_wait(&mainCond, &mainMutex);

    return 0; // Never going to happen. :c
}