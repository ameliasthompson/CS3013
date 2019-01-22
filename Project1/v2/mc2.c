// Maximilian Thompson

#include <stdio.h>
#include <stdlib.h>

#include <sys/resource.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "cprocess.h"
#include "option.h"
#include "util.h"

#define MAX_ARGS		40
#define MAX_OPTIONS		32
#define MAX_DIR_LENGTH	128
#define MAX_BGPROCESSES	10



int main(void) {
	/*
	 *  Startup.
	 */
	
	// Initialize the background process tracker array.
	struct cprocess processes[MAX_BGPROCESSES];
	for (int i = 0; i < MAX_BGPROCESSES; i++) {
		processes[i].pid = 0; // Anything with this pid is not a processes.
	}

	// Initialize array of options with defaults.
	struct option* cmds[MAX_OPTIONS];
	int cmdsSize = 0; // The number of options.
	if ( // Add all default commands, and check to make sure they all succeed.
			appendOption(cmds, &cmdsSize, "whoami", "whoami", "Prints out the result of the 'whoami' command.", "-- Who am I? --", NULL, 0, 0, 1) < 0 ||
			appendOption(cmds, &cmdsSize, "last", "last", "Prints out the result of the 'last' command.", "-- Last logins --", NULL, 0, 0, 1) < 0 ||
			appendOption(cmds, &cmdsSize, "ls", "ls", "Prints out the result of the 'ls' command.", "-- Directory listing --", NULL, 1, 1, 1) < 0
			) {
		printf("Failed to add default commands.\n");
		exit (-1);
	}

	// Print header.
	printf("===== Mid-Day Commander, v2 =====\n");
	
	/*
	 *  Main loop.
	 */

	int finished = 0;
	while (!finished) {

		// Print prompt.
		printf("Taimi speaking. What command would you like to run?\n");
		for (int i = 0; i < cmdsSize; i++) {
			printf("   %d. %s\t: %s\n", i, cmds[i]->name, cmds[i]->desc);
		}
		printf("   a. add command : Adds a new command to the menu.\n");
		printf("   c. change directory : Changes the process working directory.\n");
		printf("   e. exit : Leave Mid-Day Commander.\n");
		printf("   p. pwd : Prints the working directory.\n");
		printf("Option?: ");
		
		int option = selectOption(cmdsSize);
		
		// Check for predefined letter options before selecting a command option.
		if (option == OPT_A) {
			// Add a command, and store its ID.
			int id = addOption(cmds, &cmdsSize);

			// Check to see if it actually succeeded.
			if (id < 0) {
				printf("Failed to add option.\n");

			} else {
				// It did succeed, and we need to print the confirmation.
				printf("Added with ID %d\n", id);
			}
			printf("\n"); // Padding before next prompt.

		} else if (option == OPT_C) {
			// Change working directory.
			char path[MAX_IN_LENGTH];
			printf("Path?: ");
			fgets(path, MAX_IN_LENGTH, stdin);
			cleanNewline(path);
			chdir(path);
			printf("\n"); // Padding before next prompt.

		} else if (option == OPT_E) {
			// Exit.
			finished = 1;

		} else if (option == OPT_P) {
			// Print working directory.
			char path[MAX_DIR_LENGTH];
			getcwd(path, MAX_DIR_LENGTH);
			printf("cwd: %s\n", path);
			printf("\n"); // Padding before next prompt.

		} else {
			// We want to actually run a command!
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
			
			// Unpack default arguments if there are any.
			if (cmds[option]->defArgs != NULL) {
				while (argsSize < MAX_DEF_ARGS && cmds[option]->defArgs[argsSize - 1] != NULL) {
					args[argsSize] = malloc(MAX_IN_LENGTH * sizeof(char));
					strcpy(args[argsSize], cmds[option]->defArgs[argsSize - 1]);
					argsSize++;
				}
			}

			// Get user arguments if required.
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
			
			// Get user path argument if required.
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

			// Terminate the arguments array.
			args[argsSize] = NULL;
			argsSize++; // So executeCommand() can properly free the memory.

			#ifdef _DEBUG
				printf ("COMMAND: ");
				for (int _ARG = 0; args[_ARG] != NULL; _ARG++) {
					printf("%s ", args[_ARG]);
				}
				printf("\n");
			#endif
			
			// Finally, it's time to execute.
			if (cmds[option]->blocking) {
				// Get a new process tracker.
				struct cprocess fgProcess;
				executeCommand(args, argsSize, &fgProcess);

				// We're blocking, so we're in for the long haul now.
				pid_t fin = 0;
				while (fin != fgProcess.pid) {
					// Wait for something to happen.
					struct rusage use;
					pid_t fin = wait3(NULL, 0, &use);

					if (fin == fgProcess.pid) {
						// It's the foreground process!
						cleanCommand(&fgProcess, &use);

					} else {
						// It's a background process!
						// Find the right tracker and pass it on.
						struct cprocess* proc = findProcess(processes, MAX_BGPROCESSES, fin);
						cleanCommand(proc, &use);
					}
				}
			} else {
				// It's not blocking!
				// Find an unused process tracker.
				struct cprocess* proc = findProcess(processes, MAX_BGPROCESSES, 0);
				executeCommand(args, argsSize, proc);
			}

			// Try to grab a child process while we're out here.
			// We have to try at least as many times as there are potential processes.
			for (int i = 0; i < MAX_BGPROCESSES; i++) {
				pid_t bgproc = 0;
				struct rusage bguse;
				bgproc = wait3(NULL, WNOHANG, &bguse);
				
				// Try to resolve something if it happened.
				if (bgproc > 0) {
					struct cprocess* tracker = findProcess(processes, MAX_BGPROCESSES, bgproc);
					cleanCommand(tracker, &bguse);
				}
			}
			
			printf("\n"); // A little padding before the next prompt.
		}
	}

	printf("Exiting...\n");
}
