// Maximilian Thompson

#include "waitthread.h"

extern struct cprocess processes[MAX_BGPROCESSES];
pthread_mutex_t canWait;

void* waitThread() {
    while(1) {
        pid_t pid = 0;
        struct rusage use;
        pthread_mutex_lock(&canWait);
        pid = wait3(NULL, WNOHANG, &use);
        pthread_mutex_unlock(&canWait);
        
        // Try to resolve something if it happened.
        if (pid > 0) {
            struct cprocess* tracker = findProcess(processes, MAX_BGPROCESSES, pid);
            // Print the background process header.
            printf("\n-- Job Complete [%d] --\n", tracker->jobnum);
            printf("Process ID: %d\n", tracker->pid);
            // Clean up and print statistics.
            decrementJobs(processes, MAX_BGPROCESSES, tracker->jobnum);
            cleanCommand(tracker, &use);
        }
        // Probably shouldn't yield because this doesn't block the processes it's
        // waiting for.
    }
}