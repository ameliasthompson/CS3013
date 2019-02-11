// Maximilian Thompson

#ifndef CARTHREAD_H
#define CARTHREAD_H

#include <pthread.h>
#include <stdlib.h>
#include <time.h>

#include "constants.h"

typedef struct {
    int cid;             // Car ID.
    int turn;            // Where the car is turning.
    car_t* next;         // Used for queue linked list.
    pthread_cond_t cond; // Used when the car is waiting in a queue, or waiting for the intersection. (Never for both).
} car_t;

/**
 * Create a car strict.
 * @param cid     The id of the new car.
 * @return car_t* The new car.
 */
car_t* create_car(int cid);

/**
 * The main function for a car thread. Handles 
 * 
 * @param car The car that this thread is.
 */
void car_main(car_t* car);

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
 * Attempt to obtain mutexs for all quadrants it passes through. If it fails,
 * sleep until someone finishes in the intersection and wakes it up.
 * @param car The car that's driving.
 * @param qid The ID of the queue the car is coming from.
 */
void drive(car_t* car, int qid);

/**
 * Big switch statements for turning.
 * @param car The car that is turning.
 * @param qid The ID of the queue the car is coming from.
 * @return int Whether or not the car actually enters the intersection.
 */
int enter_intersection(car_t* car, int qid);

/**
 * Attempt to take the lock for a quadrant. If successful, set the metadata
 * on that quadrant to the car ID. Otherwise, leave everything as it is and
 * return -1.
 * @param car  The car taking the quadrant.
 * @param quad The quadrant to take.
 * @return int Failure or success.
 */
int attempt_quadrant(car_t* car, int quad);

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
