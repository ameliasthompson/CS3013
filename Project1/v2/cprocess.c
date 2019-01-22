// Maximilian Thompson

#include "cprocess.h"

extern int jobCounter;

void decrementJobs(struct cprocess* array, int size, int n) {
    for(int i = 0; i < size; i++) {
        if (array[i].jobnum > n) array[i].jobnum--;
    }
    jobCounter--;
}

struct cprocess* findProcess(struct cprocess* array, int size, pid_t pid) {
    for (int i = 0; i < size; i++) {
        if (array[i].pid == pid) return array + i;
    }

    return NULL;
}

struct cprocess* findJobnum(struct cprocess* array, int size, int n) {
    for (int i = 0; i < size; i++) {
        if (array[i].jobnum == n) return array + i;
    }

    return NULL;
}

void executeCommand(char** args, int argc, struct cprocess* tracker) {
	gettimeofday(&(tracker->start), NULL); // Start the timer.
	tracker->pid = fork();
	if (tracker->pid == -1) {
		// If the fork has failed:
		fprintf(stderr, "Fork failed; aborting...\n");
	
	} else if (tracker->pid == 0) {
		// We are the child.
		execvp(args[0], args);
	
	} else
		
	// Free arguments.
	// Child shouldn't need to free them because it's dead.
	// Start at 1 because 0 is a pointer to memory in a struct.
	for (int i = 1; i < argc; i++) {
		free(args[i]);
	}
}

void cleanCommand(struct cprocess* tracker, struct rusage* stats) {
	gettimeofday(&(tracker->stop), NULL); // End the timer.
	
	// Calculate time elapsed.
	time_t seconds = tracker->stop.tv_sec - tracker->start.tv_sec;
	long microseconds = tracker->stop.tv_usec - tracker->start.tv_usec;
	
	long milliseconds = seconds * 1000; // Convert seconds to milliseconds.
	milliseconds += microseconds / 1000; // Convert microseconds to milliseconds.
	
	// Print statistics.
	printf("\n--- Statistics ---\n");
	if (milliseconds < 1) { // Print something special if almost no time passed.
		printf("Elapsed time: <1 millisecond\n");
	} else { // Otherwise just print the value.
		printf("Elapsed time: %ld milliseconds\n", milliseconds);
	}
	printf("Page faults: %ld\n", stats->ru_majflt);
	printf("Page faults (reclaimed): %ld\n", stats->ru_minflt);

    tracker->pid = 0;
}