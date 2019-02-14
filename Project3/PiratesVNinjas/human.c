// Maximilian Thompson

#include "human.h"

extern human_t* queue[2];
extern sem_t queueSem[2];

extern department_t dept;

extern sem_t vCountSem;
extern int visitsLeft;

human_t* create_human(int id, int alignment, int avgCostume, int avgAdventure) {
    human_t* tmp = malloc(sizeof(human_t));
    tmp->id = id;
    tmp->alignment = alignment;
    tmp->avgCostume = avgCostume;
    tmp->avgAdventure = avgAdventure;
    tmp->willReturn = 1;
    tmp->next = NULL;
    sem_init(&tmp->sem, 0, 0);

    return tmp;
}

visit_t* create_visit(int waitTime, int visitTime) {
    visit_t* tmp = malloc(sizeof(visit_t));
    tmp->next = NULL;
    tmp->waitTime = waitTime;
    tmp->visitTime = visitTime;
    if (waitTime <= FREE_WAIT) {
        tmp->goldOwed = visitTime;
    } else {
        tmp->goldOwed = 0;
    }

    return tmp;
}

void* human_main(void* args) {
    human_t* self = (human_t*)args;

    #ifdef _DEBUG
        printf("DEBUG: New member of team %s with ID %d.\n", team_name(self->alignment), self->id);
    #endif

    while (self->willReturn) {
        self->willReturn--;

        adventure(self);
        
        sem_wait(&queueSem[self->alignment]);
        queue_human(self);
        
        // Check to see if we're the front of the queue.
        if (queue[self->alignment] == self) {
            sem_post(&self->sem);
        }
        sem_post(&queueSem[self->alignment]);

        sem_wait(&self->sem); // Wait if we're not at the front.

        // Now check to see if we can enter.
        while(1) {
            sem_wait(&dept.sem);
            if (dept.alignment == self->alignment && chair_free(&dept)) {
                // If we can, we have to do it without droping the dept semaphore.
                sem_wait(&queueSem[self->alignment]);
                dequeue_human(self->alignment);
                enter_department(self);
                sem_post(&queueSem[self->alignment]);
                sem_post(&dept.sem);
                break;
            } else {
                // Else, we drop the semaphore and wait on our own semaphore.
                sem_post(&dept.sem);
                sem_wait(&self->sem);
            }
        }

        costume(self);

        sem_wait(&dept.sem);
        leave_department(self);
        sem_post(&dept.sem);


        if(drand48() <= 0.25) {
            #ifdef _DEBUG
                printf("DEBUG: %s %d will return.\n", team_name(self->alignment), self->id);
            #endif
            self->willReturn++; // 25% chance to return.
        } else {
            // Else, there's one less visit to deal with.
            sem_wait(&vCountSem);
            visitsLeft--;
            sem_post(&vCountSem);
        }
    }

    return NULL;
}

void queue_human(human_t* hum) {
    #ifdef _DEBUG
        printf("DEBUG: %s %d is entering the queue.\n", team_name(hum->alignment), hum->id);
    #endif
    hum->t = time(NULL);
    
    if (queue[hum->alignment] == NULL) {
        // If there is no root.
        queue[hum->alignment] = hum;

    } else {
        // If there is a root.
        human_t* cur = queue[hum->alignment];
        while (cur->next != NULL) cur = cur->next;
        cur->next = hum;
    }
}

void dequeue_human(int alignment) {
    if (queue[alignment] != NULL) {
        human_t* tmp = queue[alignment];
        queue[alignment] = tmp->next;
        tmp->next = NULL;
    }

    // Queue could have become NULL.
    if (queue[alignment] != NULL) {
        sem_post(&queue[alignment]->sem);;
    }
}

int queue_count(int alignment) {
    if (queue[alignment] == NULL) {
        return 0;
    } else {
        int count = 1;
        human_t* cur = queue[alignment];
        while(cur->next != NULL) {
            cur = cur->next;
            count++;
        }

        return count;
    }

}

int other_alignment(human_t* hum) {
    switch(hum->alignment) {
    case PIRATE:
        return NINJA;
    case NINJA:
        return PIRATE;
    default:
        return -1;
    }
}

int current_wait_time(human_t* hum) {
    return hum->t - time(NULL);
}

void adventure(human_t* hum) {
    int t = (int)norm_dist(hum->avgAdventure, STD_DEV);
    #ifdef _DEBUG
        printf("DEBUG: %s %d is adventuring for %d seconds.\n", team_name(hum->alignment), hum->id, t);
    #endif
    sleep(t);
}

void costume(human_t* hum) {
    #ifdef _DEBUG
        printf("DEBUG: %s %d is costuming for %d seconds.\n", team_name(hum->alignment), hum->id, hum->visit->visitTime);
    #endif
    sleep(hum->visit->visitTime);
}

void enter_department(human_t* hum) {
    #ifdef _DEBUG
        printf("DEBUG: Started servicing %s %d.\n", team_name(hum->alignment), hum->id);
    #endif
    // First, we're magic and know the future.
    if (hum->visit == NULL) {
        // It's their first visit!
        hum->visit = create_visit(current_wait_time(hum), (int)norm_dist(hum->avgCostume, STD_DEV));
    } else {
        // We need to push it to the linked list.
        visit_t* tmp = create_visit(current_wait_time(hum), (int)norm_dist(hum->avgCostume, STD_DEV));
        tmp->next = hum->visit;
        hum->visit = tmp;
    }
    
    // Then we we find an empty chair.
    for (int i = 0; i < 4; i++) {
        if (dept.chairs[i] != NULL && dept.chairs[i]->hum == NULL) {
            dept.chairs[i]->hum = hum;
            update_chair(dept.chairs[i], hum->visit->visitTime);
            
            break;
        }
    }

    // We should probably check fight conditions now to be safe.
    for (int i = 0; i < 4; i++) {
        if (dept.chairs[i] != NULL && dept.chairs[i]->hum != NULL && dept.chairs[i]->hum->alignment == other_alignment(hum)) {
            printf("There was a fight. No one survived.\n");
            abort();
        }
    }

    // Then we decide if it's time for things to change hands.
    // We swap on the following conditions:
    //   --The other queue is at least SWAP_BUFFER bigger.
    //   --The head of the other queue has waited at least SWAP_TIMEOUT seconds.
    //   --Our queue is empty.
    if ((dept.alignment == PIRATE || dept.alignment == NINJA) && queue[other_alignment(hum)] != NULL &&
            (queue_count(hum->alignment) + SWAP_BUFFER < queue_count(other_alignment(hum))
            || queue[dept.alignment] == NULL
            || current_wait_time(queue[other_alignment(hum)]) >= SWAP_TIMEOUT)) {

        switch(dept.alignment) {
        case PIRATE:
            dept.alignment = TO_NINJA;
            break;
        case NINJA:
            dept.alignment = TO_PIRATE;
            break;
        }

        #ifdef _DEBUG
            printf("DEBUG: Started swapping department alignment %s.\n", team_name(dept.alignment));
        #endif
    }
}

void leave_department(human_t* hum) {
    #ifdef _DEBUG
        printf("DEBUG: Finished servicing %s %d.\n", team_name(hum->alignment), hum->id);
    #endif
    
    // First, we get out of the chair.
    for (int i = 0; i < 4; i++) {
        if (dept.chairs[i] != NULL && dept.chairs[i]->hum == hum) {
            dept.chairs[i]->hum = NULL;
        }
    }

    // We have to check again when we're leaving to stop a deadlock.
    // Yeah, we only need to check as we leave to prevent all deadlocks,
    // but we want the speed of checking on entry, and the computation
    // here isn't *too* massive.
    if ((dept.alignment == PIRATE || dept.alignment == NINJA) && queue[other_alignment(hum)] != NULL &&
            (queue_count(hum->alignment) + SWAP_BUFFER < queue_count(other_alignment(hum))
            || queue[dept.alignment] == NULL
            || current_wait_time(queue[other_alignment(hum)]) >= SWAP_TIMEOUT)) {

        switch(dept.alignment) {
        case PIRATE:
            dept.alignment = TO_NINJA;
            break;
        case NINJA:
            dept.alignment = TO_PIRATE;
            break;
        }

        #ifdef _DEBUG
            printf("DEBUG: Started swapping department alignment %s.\n", team_name(dept.alignment));
        #endif
    }
    

    // Then, if the alignment is changing we check if we're the last one out.
    if ((dept.alignment == TO_PIRATE || dept.alignment == TO_NINJA) && all_empty(&dept)) {
        switch(dept.alignment) {
        case TO_PIRATE:
            dept.alignment = PIRATE;
            break;
        case TO_NINJA:
            dept.alignment = NINJA;
            break;
        }

        #ifdef _DEBUG
            printf("DEBUG: Swapped department alignment to %s.\n", team_name(dept.alignment));
        #endif
    }

    // Then we wake up whoever is at the head of the current owner's queue.
    sem_wait(&queueSem[team_alignment(&dept)]);
    if (queue[team_alignment(&dept)] != NULL) // CHECK TO SEE IF IT'S NULL FIRST
        sem_post(&queue[team_alignment(&dept)]->sem);
    sem_post(&queueSem[team_alignment(&dept)]);

    #ifdef _DEBUG
        printf("DEBUG: %s %d has left.\n", team_name(hum->alignment), hum->id);
    #endif
}

#ifdef _DEBUG

char* team_name(int alignment) {
    switch(alignment) {
    case PIRATE:
        return "PIRATE";
    case NINJA:
        return "NINJA";
    case TO_PIRATE:
        return "TO PIRATE";
    case TO_NINJA:
        return "TO NINJA";
    default:
        return "ERROR";
    }
}

#endif