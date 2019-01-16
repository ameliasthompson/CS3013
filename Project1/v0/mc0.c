// Maximilian Thompson

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_OPTION 2
#define MAX_ARGS 2
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
		printf("Option?: ");
		
		int option = getOption();
		
		// Print header and get arguments.
		printf("\n"); // A little padding.
		char* args[MAX_ARGS + 1]; // Maximum number of arguments, plus the terminator.
		for (int i = 0; i < MAX_ARGS + 1; i++) {
			args[i] = NULL; // Init all to null.
		}
		
		switch (option) {
			case 0: // whoami
				printf("-- Who am I? --\n");
				args[0] = malloc(MAX_IN_LENGTH * sizeof(char));
				args[0][0] = '\0'; // whoami takes no arguments.
				args[1] = NULL; // Terminator.
				break;
				
			case 1: // last
				printf("-- Last Logins --\n");
				args[0] = malloc(MAX_IN_LENGTH * sizeof(char));
				args[0][0] = '\0'; // last takes no arguments.
				args[1] = NULL; // Terminator.
				break;
			
			case 2: // ls
				printf("-- Directory Listing --\n");
				
				// ls takes two arguments.
				args[0] = malloc(MAX_IN_LENGTH * sizeof(char));
				args[1] = malloc(MAX_IN_LENGTH * sizeof(char));
				
				printf("Arguments?: ");
				fgets(args[0], MAX_IN_LENGTH, stdin); // Get arguments.
				printf("Path?: ");
				fgets(args[1], MAX_IN_LENGTH, stdin); // Get path.
				args[2] = (char*)NULL; // Null terminator.
				printf("\n"); // A little padding.
				break;
				
			default: // Error
				fprintf(stderr, "Somehow, nothing caught this invalid option.\n");
		}
		
		
		// Finally, it's time to execute.
		pid_t cid = fork();
		if (cid == -1) {
			// If the fork has failed:
			fprintf(stderr, "Fork failed; aborting...\n");
			
		} else if (cid == 0) {
			// If we are the child:
			switch (option) {
			case 0: // whoami
				execvp("/bin/whoami", args);
				// We don't need breaks because all of this ceases to exist.
				
			case 1: // last
				execvp("/bin/last", args);
			
			case 2: // ls
				execvp("/bin/ls", args);
			
			default: // Error
				fprintf(stderr, "Something horrible has happened.\n");
				exit(-1);
			}
		} else {
			// We are the parent.
			wait3(NULL, 0, NULL);
			
		}
		
		// Free arguments.
		for (int i = 0; i < MAX_ARGS + 1; i++) {
			if (args[i] != NULL) free(args[i]);
		}
		
		printf("\n"); // A little padding before the next prompt.
	}
}
