// Maximilian Thompson

#ifndef OPTION_H
#define OPTION_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "util.h"

// The maximum input length.
#define MAX_IN_LENGTH	200
#define MAX_DEF_ARGS	20

// Options that aren't numerical.
#define OPT_A -2
#define OPT_C -3
#define OPT_E -4
#define OPT_P -5

/**
 * @struct option
 * A struct representing an option entry on the list of commands presented
 * to the user.
 */
struct option {
	char path[MAX_IN_LENGTH]; // A path to the executable.
	char name[MAX_IN_LENGTH]; // The name of the option.
	char desc[MAX_IN_LENGTH]; // The description of the option.
	char head[MAX_IN_LENGTH]; // The header to print before the command.
	char** defArgs; // The default arguments the command should have.
	int args; // Should the command ask for arguments?
	int argPath; // Should the command ask for a path?
};

/**
 * Block the program waiting for user input. Once input has been received,
 * identify whether it is a valid option or not before returning.
 * @param	size	The current length of the options array.
 * @return	>= 0	The index of the selected option.
 * @return	-1		Failure.
 * @return	<= -1	A letter option.
 */
int selectOption(int size);

/**
 * Take user input regarding the command--including arguments--, and generate
 * the name, description, and header. Then add a new option to the menu. The
 * array provided must have enough memory allocated to fit the new option in at
 * cmds[size]. On a failure, the function will return -1, and both the array and
 * size will be unmodified. Otherwise the ID of the new option will be returned.
 * @param	cmds	An array of pointers to option structs. 
 * @param	size 	A pointer to the current length of the array.
 * @return	>= 1	The ID of the new option.
 * @return	-1		Failure.
 */
int addOption(struct option** cmds, int* size);

/**
 * Take user input regarding the command, the name, and the description, then
 * add a new option to the menu. The array provided must have enough memory
 * allocated to fit the new option in at cmds[size]. On a failure, the function
 * will return -1, and both the array and size will be unmodified. Otherwise the
 * ID of the new option will be returned.
 * @param	cmds	An array of pointers to option structs.
 * @param	size	A pointer to the current length of the array.
 * @return	>= 1	The ID of the new option.
 * @return	-1		Failure.
 */
int addOptionVerbose(struct option** cmds, int* size);

/**
 * Append a specified option at the end of the array. On failure, the function
 * will return -1, and both the array and size will be unmodified.
 * @param	cmds	An array of pointers to option structs.
 * @param	size	A pointer to the current length of the array.
 * @param	path	A string representing the path of the executable.
 * @param	name	A string representing the name of the option.
 * @param	desc	A string representing the description.
 * @param	head	A string representing the header to print before executing.
 * @param	defArgs	The default arguments applied to the command.
 * @param	args	Whether or not the command will take arguments.
 * @param	argPath	Whether or not the command will take a path argument.
 * @return	>= 1	The ID of the new option.
 * @return	-1		Failure.
 */
int appendOption(struct option** cmds, int* size, const char* path, const char* name, const char* desc, const char* head, char** defArgs, int args, int argPath);

#endif
