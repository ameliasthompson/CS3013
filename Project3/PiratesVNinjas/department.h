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

chair_t* create_chair();

void init_department(department_t* dept, int alignment, int chairs);

void update_chair(chair_t* chair, int visitTime);

int chair_free(department_t* dept);

int all_empty(department_t* dept);

#endif
