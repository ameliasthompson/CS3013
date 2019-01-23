// Maximilian Thompson

#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include "option.h"
#include "util.h"

#define MAX_ARGS	20
#define MAX_OPTIONS	50

/**
 * Fork and execute a command provided by an option struct.
 * @arg	args	A pointer to the array of arguments (including path).
 */
void executeCommand(char** args) {
	struct timespec start, stop;
	clock_gettime(CLOCK_MONOTONIC, &start); // Start the timer.
	pid_t cid = fork();
	if (cid == -1) {
		// If the fork has failed:
		fprintf(stderr, "Fork failed; aborting...\n");
	
	} else if (cid == 0) {
		// We are the child.
		execvp(args[0], args);
	
	} else {
		// We are the parent.
		struct rusage* stats = malloc(sizeof(struct rusage));
		wait3(NULL, 0, stats);
		clock_gettime(CLOCK_MONOTONIC, &stop); // End the timer.
		
		// Calculate time elapsed.
		time_t seconds = stop.tv_sec - start.tv_sec;
		long nanoseconds = stop.tv_nsec - start.tv_nsec;
		
		long milliseconds = seconds * 1000; // Convert seconds to milliseconds.
		milliseconds += nanoseconds / 1000000; // Convert nanoseconds to milliseconds.
		
		// Print statistics.
		printf("\n--- Statistics ---\n");
		if (milliseconds < 1) { // Print something special if almost no time passed.
			printf("Elapsed time: <1 millisecond\n");
		} else { // Otherwise just print the value.
			printf("Elapsed time: %ld milliseconds\n", milliseconds);
		}
		printf("Page faults: %ld\n", stats->ru_majflt);
		printf("Page faults (reclaimed): %ld\n", stats->ru_minflt);
	
		free(stats);
	}
	
	// Free arguments.
	// Child shouldn't need to free them because it's dead.
	// Start at 1 because 0 is a pointer to memory in a struct.
	for (int i = 1; i < MAX_ARGS + 1; i++) {
		if (args[i] != NULL) {
			free(args[i]);
		}
	}
}

int main(void) {
	/*
	 *  Startup.
	 */
	
	// Initialize array of options with defaults.
	struct option* cmds[MAX_OPTIONS];
	int cmdsSize = 0; // The number of options.
	appendOption(cmds, &cmdsSize, "/usr/bin/whoami", "whoami", "Prints out the result of the 'whoami' command.", "-- Who am I? --", 0, 0);
	appendOption(cmds, &cmdsSize, "/usr/bin/last", "last", "Prints out the result of the 'last' command.", "-- Last logins --", 0, 0);
	appendOption(cmds, &cmdsSize, "/bin/ls", "ls", "Prints out the result of the 'ls' command.", "-- Directory listing --", 1, 1);
	appendOption(cmds, &cmdsSize, "./seymour", "seymour", "Waits one thousand years.", "-- Waiting patiently --", 0, 0);
	
	// Print header.
	printf("===== Mid-Day Commander, v0 =====\n");
	
	/*
	 *  Main loop.
	 */

	int finished = 0; // Not that you can actually exit.
	while (!finished) {

		// Print prompt.
		printf("Taimi speaking. What command would you like to run?\n");
		for (int i = 0; i < cmdsSize; i++) {
			printf("   %d. %s\t: %s\n", i, cmds[i]->name, cmds[i]->desc);
		}
		printf("   a. add command.");
		printf("Option?: ");
		
		int option = selectOption(cmdsSize);
		
		// Print header and get arguments.
		printf("\n"); // Padding before header.
		printf("%s\n", cmds[option]->head); // Header.
		
		// Init arguments table.
		/* args:
		 * 0 - Pointer to constant string path to executable. DO NOT FREE.
		 * 1 - First argument.
		 * 2 - Second argument.
		 * . . .
		 * n - nth argument.
		 *
		 * Because this is reinitialized every loop, we don't need to worry about setting
		 * the terminator after reading the arguments.
		 */
		char* args[MAX_ARGS + 1]; // Maximum number of arguments, plus the terminator.
		args[0] = cmds[option]->path; // Set path of executable.
		int argsSize = 1; // The number of arguments.
		
		// Get arguments if required.
		if (cmds[option]->args) {
			// Get arguments:
			printf("Arguments?: ");
			char buf[MAX_IN_LENGTH]; // Input buffer.
			fgets(buf, MAX_IN_LENGTH, stdin); // Get arguments.
			cleanNewline(buf);
			
			// Check to see if there are any arguments inputted.
			if (buf[0] != '\0') {
				// Process arguments:
				char* tmp; // Buffer for the buffer.
				tmp = strtok(buf, " "); // Prime strtok().
			
				// Get the arguments if they exist. (Leave room for path argument.)
				while (argsSize < MAX_ARGS - 1 && tmp != NULL) {
					// tmp actually has an argument if you're here.
					args[argsSize] = malloc(MAX_IN_LENGTH * sizeof(char));
					strcpy(args[argsSize], tmp); // Write argument to array.
				
					tmp = strtok(NULL, " "); // Should keep going on buf.
					argsSize++;
				}
			}
		}
		
		// Get path argument if required.
		if (cmds[option]->argPath) {
			// Getting the path is SO much easier.
			printf("Path?: ");
			char buf[MAX_IN_LENGTH]; // Input buffer.
			fgets(buf, MAX_IN_LENGTH, stdin); // Get path argument.
			cleanNewline(buf);
			
			// Check to see if anything was actually inputted.
			if (buf[0] != '\0') {
				// Allocate and write the argument.
				args[argsSize] = malloc(MAX_IN_LENGTH * sizeof(char));
				strcpy(args[argsSize], buf); // Write path argument to array.
				argsSize++;
			}
		}

		#ifdef _DEBUG
			printf ("COMMAND: ");
			for (int _ARG = 0; args[_ARG] != NULL; _ARG++) {
				printf("%s ", args[_ARG]);
			}
			printf("\n");
		#endif
		
		// Finally, it's time to execute.
		executeCommand(args);
	
		// ARGUMENTS FREED BY EXECUTE FUNCTION (GOING TO BE IMPORTANT FOR THREADING)
		
		printf("\n"); // A little padding before the next prompt.
	}
}
