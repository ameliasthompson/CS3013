// Maximilian Thompson

#include "carThread.h"

extern pthread_mutex_t intersectionLock; // Mutex for checking if the intersection can be crossed.
extern pthread_mutex_t quadrantLock[4];  // Mutex for each quadrant of the intersection.
extern pthread_mutex_t queueLock[4];     // Mutex for each queue.

extern car_t* queue[4];                  // The queues of cars.

extern int quadrant[4];                  // What car is occupying each quadrant.

car_t* create_car(int cid) {
    car_t* car = malloc(sizeof(car_t));
    car->cid  = cid;
    car->next = NULL;
    car->turn = NO_TURN;
    pthread_cond_init(&car->cond, NULL);

    return car;
}

void* car_main(void* args) {
    car_t* car = (car_t*)args; // Just case this to make my life easier.

    while (1) { // We're going to be driving for a LONG time.
        int qid = rand()%4; // Queue to enter.
        car->turn = rand()%3; // Pick a direction to turn.
        queue_car(car, qid); // Stick the car in the queue.
        wait_for_queue(car, qid); // Wait to be at the front.
        
        pthread_mutex_lock(&intersectionLock);
        // Check to see if the car still wants to turn, or if it's been forced
        // into the intersection by some kind person who just left it.
        while (car->turn == WLEFT || car->turn == WSTRAIGHT || car->turn == WRIGHT) // Loop to prevent spurious wakeups.
            if (!enter_intersection(car, qid)) // Actually attempt to enter the intersection.
                pthread_cond_wait(&car->cond, &intersectionLock); // Sleep if we failed.
        pthread_mutex_unlock(&intersectionLock);

        usleep(SLEEPTIME); // Go to sleep in the intersection.

        pthread_mutex_lock(&intersectionLock);
        leave_intersection(car); // Exit the intersection.
        pthread_mutex_unlock(&intersectionLock);

        dequeue_car(qid); // Move the next car to the front of the queue it came from.
    }
}

void queue_car(car_t* car, int qid) {
    pthread_mutex_lock(&queueLock[qid]); // Get the lock to change the queue.
    if (queue[qid] == NULL) { // If the queue is empty.
        queue[qid] = car;
    
    } else { //If we need to crawl the queue.
        car_t* cur = queue[qid];
        while (cur->next != NULL) cur = cur->next; // Go to the end.
        cur->next = car; // Add the car.
    }
    pthread_mutex_unlock(&queueLock[qid]); // And we're done, so let og.
}

void wait_for_queue(car_t* car, int qid) {
    pthread_mutex_lock(&queueLock[qid]); // Get the lock to check the queue.
    while (queue[qid] != car) // Loop while it isn't the front of the queue.
        pthread_cond_wait(&car->cond, &queueLock[qid]);
    
    pthread_mutex_unlock(&queueLock[qid]); // Stop accessing the queue.
}

int enter_intersection(car_t* car, int qid) {
    if (car->turn == NO_TURN)
        return 0; // We just called this 'from' this car.

    // Big switch statement because the idea I had to use a pile of #defined bitfields
    // is really overcomplicated and will just make things go wrong.
    switch (car->turn) {
    case WLEFT: // Turning left.
        switch(qid) {
        case N: // From north.
            if (attempt_quadrant(car, NW) && attempt_quadrant(car, SW) && attempt_quadrant(car, SE))
                car->turn = TLEFT; // We're actually turning!!!
            break;
        case S: // From south.
            if (attempt_quadrant(car, SE) && attempt_quadrant(car, NE) && attempt_quadrant(car, NW))
                car->turn = TLEFT;
            break;
        case E: // Etc...
            if (attempt_quadrant(car, NE) && attempt_quadrant(car, NW) && attempt_quadrant(car, SW))
                car->turn = TLEFT;
            break;
        case W:
            if (attempt_quadrant(car, SW) && attempt_quadrant(car, SE) && attempt_quadrant(car, NE))
                car->turn = TLEFT;
            break;
        }
        break;

    case WSTRAIGHT:
        switch(qid) {
        case N:
            if (attempt_quadrant(car, NW) && attempt_quadrant(car, SW))
                car->turn = TSTRAIGHT;
            break;
        case S:
            if (attempt_quadrant(car, SE) && attempt_quadrant(car, NE))
                car->turn = TSTRAIGHT;
            break;
        case E:
            if (attempt_quadrant(car, NE) && attempt_quadrant(car, NW))
                car->turn = TSTRAIGHT;
            break;
        case W:
            if (attempt_quadrant(car, SW) && attempt_quadrant(car, SE))
                car->turn = TSTRAIGHT;
            break;
        }
        break;

    case WRIGHT:
        switch(qid) {
        case N:
            if (attempt_quadrant(car, NW))
                car->turn = TRIGHT;
            break;
        case S:
            if (attempt_quadrant(car, SE))
                car->turn = TRIGHT;
            break;
        case E:
            if (attempt_quadrant(car, NE))
                car->turn = TRIGHT;
            break;
        case W:
            if (attempt_quadrant(car, SW))
                car->turn = TRIGHT;
            break;
        }
        break;
    }

    // If we still want to turn, then we failed to grab at least one lock.
    // As such, we need to release all the locks we actually managed to grab.
    if (car->turn == WLEFT || car->turn == WSTRAIGHT || car->turn == WRIGHT) {
        for (int i = 0; i < 4; i++) {
            if (quadrant[i] == car->cid) {
                pthread_mutex_unlock(&quadrantLock[i]);
                quadrant[i] = -1;
            }
        }

        return 0; // And we failed to enter, so return false.
    
    // Or else, we actually do want to turn, so if someone else is doing this they
    // should wake us up.
    } else {
        pthread_cond_signal(&car->cond);
        return 1; // And we did enter, so return true.
    }
}

int attempt_quadrant(car_t* car, int quad) {
    // This has a slightly higher potential to deadlock, but honestly that just
    // lets me know that I'm doing something wrong. Not something to do for
    // something in production though.
    if (quadrant[quad] == -1) {
        // The quadrant hopefully isn't occupied!!!
        pthread_mutex_lock(&quadrantLock[quad]);
        quadrant[quad] = car->cid;
        return 1;
    } else {
        return 0; // The quadrant was occupied.
    }
}

void leave_intersection(car_t* car) {
    // First, we free everything we're using.
    for (int i = 0; i < 4; i++)
        if (quadrant[i] == car->cid) pthread_mutex_unlock(&quadrantLock[i]);
    
    // Then we stop turning.
    car->turn = NO_TURN;

    // Now we go push some sleeping person's car into the middle of the intersection.
    // This IS going to call it on this car too, which kinda sucks, but it won't do
    // anything we because we went and set our turn to NO_TURN.
    for(int i = 0; i < 4; i++)
        if (queue[i] != NULL) enter_intersection(queue[i], i);
}

void dequeue_car(int qid) {
    pthread_mutex_lock(&queueLock[qid]); // Get the lock to change the queue.
    queue[qid] = queue[qid]->next; // Move the queue.
    if (queue[qid] != NULL)
        pthread_cond_signal(&queue[qid]->cond); // Wake the car.
    
    pthread_mutex_unlock(&queueLock[qid]); // Stop touching the queue.
}