// Maximilian Thompson

#ifndef DEPARTMENT_H
#define DEPARTMENT_H

#include <time.h>

#include "human.h"

// Number of seconds before stuff becomes free.
#define FREE_WAIT 30

typedef struct {
    struct human* hum;
    time_t last;
    time_t occupied;
    time_t free;
} chair_t;

typedef struct {
    int alignment;
    chair_t* chairs[4];
    sem_t sem;
} department_t;

/**
 * Allocate and initialize a chair struct.
 * @return chair_t* The freshly allocated chair.
 */
chair_t* create_chair();

/**
 * Initialize a department.
 * @param dept      The department to initialize.
 * @param alignment The starting alignment of the department.
 * @param chairs    The number of chairs it will have.
 */
void init_department(department_t* dept, int alignment, int chairs);

/**
 * Update the useage statistics on the chair.
 * @param chair     The chair to update.
 * @param visitTime The duration of the upcoming visit.
 */
void update_chair(chair_t* chair, int visitTime);

/**
 * Returns whether or not there is a free chair.
 * @param dept The department to check.
 * @return int Either 0 or 1.
 */
int chair_free(department_t* dept);

/**
 * Returns whether or not all chairs are empty.
 * @param dept The department to check.
 * @return int Either 0 or 1.
 */
int all_empty(department_t* dept);

/**
 * Returns either PIRATE or NINJA, converting the TO_PIRATE to PIRATE and TO_NINJA
 * to NINJA
 * @param dept The department to check.
 * @return int The alignment.
 */
int team_alignment(department_t* dept);

#endif
