// Maximilian Thompson

#include "department.h"

chair_t* create_chair() {
    chair_t* tmp = malloc(sizeof(chair_t));
    tmp->free = 0;
    tmp->occupied = 0;
    tmp->last = time(NULL);
    tmp->hum = NULL;

    return tmp;
}

void init_department(department_t* dept, int alignment, int chairs) {
    dept->alignment = alignment;
    sem_init(&dept->sem, 0, 1);
    for (int i = 0; i < chairs && i < 4; i++) {
        dept->chairs[i] = create_chair();
    }
}

void update_chair(chair_t* chair, int visitTime) {
    time_t now = time(NULL);
    chair->free += now - chair->last;
    chair->occupied += visitTime;
    chair->last = now + visitTime;
}

int chair_free(department_t* dept) {
    for (int i = 0; i < 4; i++)
        if (dept->chairs[i] != NULL && dept->chairs[i]->hum == NULL)
            return 1;

    return 0;
}

int all_empty(department_t* dept) {
    for (int i = 0; i < 4; i++)
        if (dept->chairs[i] != NULL && dept->chairs[i]->hum != NULL)
            return 0;
    
    return 1;
}