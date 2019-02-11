// Maximilian Thompson

#ifndef CARTHREAD_H
#define CARTHREAD_H

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "constants.h"

struct carstruct {
    int cid;                // Car ID.
    int turn;               // Where the car is turning.
    struct carstruct* next; // Used for queue linked list.
    pthread_cond_t cond;    // Used when the car is waiting in a queue, or waiting for the intersection. (Never for both).
};

typedef struct carstruct car_t;

/**
 * Create a car strict.
 * @param cid     The id of the new car.
 * @return car_t* The new car.
 */
car_t* create_car(int cid);

/**
 * The main function for a car thread. Handles the sequencing of car related
 * events. No, we're not there yet. 
 * @param arg The car that this thread is.
 */
void* car_main(void* car);

/**
 * Add the car to a queue.
 * @param car The car to add.
 * @param qid The queue to add the car to.
 */
void queue_car(car_t* car, int qid);

/**
 * Check to see if the car is the front car in the queue. If it isn't, go to
 * sleep on a condition variable. Hopefully, the car in front honks its horn
 * after it finishes in the intersection.
 * @param car The car that's waiting.
 * @param qid The ID of the queue the car is in.
 */
void wait_for_queue(car_t* car, int qid);

/**
 * Big switch statements for turning.
 * @param car The car that is turning.
 * @param qid The ID of the queue the car is coming from.
 * @return int Whether or not the car actually enters the intersection.
 */
int enter_intersection(car_t* car, int qid);

/**
 * Check to see if the lock for the quadrant is available by reading the quadrant
 * owner. If it is available, become the owner and return 1. Otherwise do nothing
 * and return 0.
 * @param car  The car taking the quadrant.
 * @param quad The quadrant to take.
 * @return int Failure or success.
 */
int attempt_quadrant(car_t* car, int quad);

/**
 * Aquires all the locks enter_intersection promises it.
 * @param car The car entering the intersection.
 */
void aquire_locks(car_t* car);

/**
 * Free the quadrants the car occupies, and then try to see if the other cars
 * want a go.
 * @param car The car leaving the intersection. 
 */
void leave_intersection(car_t* car);

/**
 * Remove the frontmost car of a queue, and then wake up the one behind it if
 * it exists.
 * @param qid     The ID of the queue.
 * @return car_t* A pointer to the dequeued car.
 */
void dequeue_car(int qid);

#endif
