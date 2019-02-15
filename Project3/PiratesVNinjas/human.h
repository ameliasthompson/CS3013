// Maximilian Thompson

#ifndef HUMAN_H
#define HUMAN_H

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "department.h"
#include "normDist.h"

// Alignments: (ALL humans can either be pirates or ninjas. This is a weird place.)
#define PIRATE    0
#define NINJA     1
#define TO_PIRATE 2
#define TO_NINJA  3

// The standard deviation used for generating times (in minutes).
#define STD_DEV 2

// The amount of difference there has to be between queue sizes to swap.
#define SWAP_BUFFER 4

// The amount of time the head of a queue has to be to force a swap.
#define SWAP_TIMEOUT 20

typedef struct visit {
    int goldOwed;  // The amount of gold they owe for the visit.
    int waitTime;  // The amount of time they waited before.
    int visitTime; // The amount of time they visited for.
    struct visit* next; // The next visit.
} visit_t;

typedef struct human {
    int id;
    int alignment;      // Pirate or ninja?
    int avgCostume;     // Average time spent inside.
    int avgAdventure;   // Average time spent outside before arriving.
    int willReturn;     // Whether or not they will return again.
    time_t t;           // When the human entered the queue.
    visit_t* visit;     // The most recent visit.
    struct human* next;      // Used for outside linked lists.
    sem_t sem;          // Used for sleeping at the door.
} human_t;

/**
 * Allocate and initialize a human struct.
 * @param alignment    Either pirate or ninja.
 * @param avgCostume   The average time spent inside the department.
 * @param avgAdventure The average time spent adventuring.
 * @return human_t*    The freshly allocated human.
 */
human_t* create_human(int id, int alignment, int avgCostume, int avgAdventure);

/**
 * Allocate and initialize a visit struct.
 * @param waitTime  The amount of time the person waited.
 * @param visitTime The amount of time the person will visit for.
 * @return visit_t* The freshly allocated visit.
 */
visit_t* create_visit(int waitTime, int visitTime);

/**
 * The main function of a human thread.
 * @param args 
 * @return void* 
 */
void* human_main(void* args);

/**
 * Return the alignment opposite to the human provided.
 * @param hum  The human.
 * @return int The opposite alignment.
 */
int other_alignment(human_t* hum);

/**
 * Get how long the person has been waiting, rounded down to the nearest second.
 * @param hum  The human.
 * @return int The seconds in line.
 */
int current_wait_time(human_t* hum);

/**
 * Sleep for however long it takes to adventure.
 * @param hum The adventuring human.
 */
void adventure(human_t* hum);

/**
 * Sleep for however long it takes to costume.
 * @param hum The costuming human.
 */
void costume(human_t* hum);

/**
 * Queue a human in the queue corrisponding to their alignment.
 * @param hum The human to queue.
 */
void queue_human(human_t* hum);

/**
 * Remove the frontmost human of a queue, and then wake up the next one.
 * @param alignment The alignment of the queue to use.
 */
void dequeue_human(int alignment);

/**
 * Count the number of people in a queue for a faction.
 * @param alignment The faction to count.
 * @return int      The number of people.
 */
int queue_count(int alignment);

/**
 * Take the department binary semaphore, then enter the department and select
 * a chair.
 * @param hum 
 */
void enter_department(human_t* hum);

void leave_department(human_t* hum);

#ifdef _DEBUG

/**
 * Return the name of the alignment.
 * @param alignment The alignment to name.
 * @return char*    The name.
 */
char* team_name(int alignment);

#endif

#endif