// Maximilian Thompson

#include "option.h"

int selectOption(int size) {
	// Read from stdin.
	char choiceStr[MAX_IN_LENGTH];
	fgets(choiceStr, MAX_IN_LENGTH, stdin);
	
	// Identify if the input at least starts with a number.
	if (isdigit(choiceStr[0])) {
		// We now know that if atoi returns 0, it's actually a 0 and not an error.
		int option = atoi(choiceStr);
		
		// Identify if it's in range.
		if (option >= 0 && option <= size) {
			// We now know the option is 100% valid.
			return option;
		}
	}
	
	// Otherwise the option is junk, so we error.
	return -1;
}

int addOption(struct option** cmds, int* size) {
	// Read user selections. (Leave room for autogen header padding in name.)
	char pathStr[MAX_IN_LENGTH], descStr[MAX_IN_LENGTH], nameStr[MAX_IN_LENGTH-6], headStr[MAX_IN_LENGTH];
	printf("Path to command?: ");
	fgets(pathStr, MAX_IN_LENGTH, stdin);
	printf("Name of command?: ");
	fgets(nameStr, MAX_IN_LENGTH-6, stdin);
	printf("Description?: ");
	fgets(descStr, MAX_IN_LENGTH, stdin);
	
	// AT THE MOMENT, IT IS ASSUMED THE USER ALWAYS WANTS ARGS.
	// AT THE MOMENT, IT IS ASSUMED THE USER ALWAYS WANTS A PATH ARGUMENT.
	
	// Clean up the strings.
	cleanNewline(pathStr);
	cleanNewline(nameStr);
	cleanNewline(descStr);
	
	// Generate header.
	headStr[0] = '\0'; // Init string to be blank.
	strcat(headStr, "-- ");
	strcat(headStr, nameStr);
	strcat(headStr, " --");
	
	// Append the option, and return what it does.
	return appendOption(cmds, size, pathStr, nameStr, descStr, headStr, 1, 1);
}

int appendOption(struct option** cmds, int* size, const char* path, const char* name, const char* desc, const char* head, int args, int argPath) {
	// Allocate new struct.
	struct option* opt = malloc(sizeof(struct option*)); // Could just do void*, because all pointers are the same size.
	if (opt == NULL) return -1; // Error if allocation failed.
	
	// Populate struct with arguments.
	strcpy(opt->path, path);
	strcpy(opt->name, name);
	strcpy(opt->desc, desc);
	strcpy(opt->head, head);
	opt->args = args;
	opt->argPath = argPath;
	
	// Push it to the array.
	cmds[(*size)++] = opt;
	
	
	return 1; // Return true cause we did good.
}
