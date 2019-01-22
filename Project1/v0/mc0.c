// Maximilian Thompson

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define MAX_OPTION 3
#define MAX_ARGS 20
#define MAX_IN_LENGTH 100

/**
 * Block the program waiting for user input. Once input has been received,
 * identify whether it is a valid option or not before returning.
 * @return On success, the option. On failure, -1.
 */
int getOption() {
	char optionString[MAX_IN_LENGTH];
	fgets(optionString, MAX_IN_LENGTH, stdin);
	
	// Identify if it at least starts with a number.
	if (isdigit(optionString[0])) {
		// If the option might actually be valid:
		int option = atoi(optionString);
		
		// Identify if it's in range.
		if (option >= 0 && option <= MAX_OPTION) {
			// If we now know it is 100% valid:
			return option;
		
		} else {
			// Otherwise the option is junk:
			return -1;
		}
	} else {
		// Otherwise the option is junk:
		return -1;
	}
}

/**
 * Replace the first newline character with a null terminator.
 * @note RESETS STRTOK
 * @arg	str	A pointer to the string to clean.
 */
void cleanNewline(char* str) {
	if(str[0] == '\n') { // strtok doesn't work if the first character is.
		str[0] = '\0';
	} else {
		strtok(str, "\n");
	}	
}

int main(void) {
	/*
	 *  Startup.
	 */
	
	printf("===== Mid-Day Commander, v0 =====\n"); // Print header.
	
	/*
	 *  Main loop.
	 */

	int finished = 0; // Not that you can actually exit.
	while (!finished) {

		// Print prompt.
		printf("Taimi speaking. What command would you like to run?\n");
		printf("   0. whoami\t: Prints out the result of the whoami command.\n");
		printf("   1. last\t: Prints out the result of the last command.\n");
		printf("   2. ls\t: Prints out the result of the ls command.\n");
		printf("   3. seymour\t: Wait.\n");
		printf("Option?: ");
		
		int option = getOption();
		
		// Confirm it actually is an option before doing any work.
		if (option >= 0 && option <= MAX_OPTION) {
		
			// Print header and get arguments.
			printf("\n"); // A little padding.
			
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
			for (int i = 0; i < MAX_ARGS + 1; i++) {
				args[i] = NULL; // Init all to null.
			}
		
		
			switch (option) {
			case 0: // whoami
				printf("-- Who am I? --\n");
				args[0] = "/usr/bin/whoami"; // Set executable path.
				break;
			
			case 1: // last
				printf("-- Last Logins --\n");
				args[0] = "/usr/bin/last"; // Set executable path.
				break;
		
			case 2: // ls
				printf("-- Directory Listing --\n");
				args[0] = "/bin/ls"; // Set executable path.
				
				// Get arguments:
				printf("Arguments?: ");
				char buf[MAX_IN_LENGTH]; // Input buffer.
				fgets(buf, MAX_IN_LENGTH, stdin); // Get arguments.
				cleanNewline(buf);
				
				// Process arguments:
				char* tmp; // Buffer for the buffer.
				tmp = strtok(buf, " "); // Prime strtok().
				
				// Get the arguments if they exist. (Leave room for path argument.)
				int i = 1; // Iterator needs to persist outside of this scope.
				while (i < MAX_ARGS - 1 && tmp != NULL) {
					// tmp actually has an argument if you're here.
					args[i] = malloc(MAX_IN_LENGTH * sizeof(char));
					strcpy(args[i], tmp); // Write argument to array.
					
					tmp = strtok(NULL, " "); // Should keep going on buf.
					i++;
				}

				// Getting the path is SO much easier.
				printf("Path?: ");
				fgets(buf, MAX_IN_LENGTH, stdin); // Get path.
				cleanNewline(buf);
				
				// Check to see if anything was actually inputted.
				if (buf[0] != '\0') {
					args[i] = malloc(MAX_IN_LENGTH * sizeof(char));
					strcpy(args[i], buf); // Write path argument to array.
					i++;
				}
				
				printf("\n"); // A little padding.
				break;
			
			case 3: // seymour
				printf("-- Waiting --\n");
				args[0] = "./seymour";
				break;
				
			default: // Error
				fprintf(stderr, "Somehow, nothing caught this invalid option.\n");
				exit(-1);
			}

			#ifdef _DEBUG
				printf ("COMMAND: ");
				for (int _ARG = 0; args[_ARG] != NULL; _ARG++) {
					printf("%s ", args[_ARG]);
				}
				printf("\n");
			#endif
		
		
			// Finally, it's time to execute.
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
			// Start at 1 because 0 is a pointer to a constant.
			for (int i = 1; i < MAX_ARGS + 1; i++) {
				if (args[i] != NULL) {
					free(args[i]);
				}
			}
			
			
		} else {
			// It wasn't an option.
			printf("Invalid option.\n");
		}
		
		printf("\n"); // A little padding before the next prompt.
	}
}
