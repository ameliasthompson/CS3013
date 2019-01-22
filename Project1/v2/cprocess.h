// Maximilian Thompson

#ifndef CPROCESS_H
#define CPROCESS_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

struct cprocess {
	struct timeval start, stop;
	pid_t pid;
    int jobnum;
};

/**
 * Iterate over the array of processes and decrement the job number of all
 * processes higher than n.
 * @param   array   A pointer to the first element of an array.
 * @param   size    The size of the array.
 * @param   n       The number.
 */
void decrementJobs(struct cprocess* array, int size, int n);

/**
 * Find a process in a given array with the given pid.
 * @param   array   A pointer to the first element of an array.
 * @param   size    The size of the array.
 * @param   pid     The pid of the process.
 * @return  A pointer to the cprocess struct.
 */
struct cprocess* findProcess(struct cprocess* array, int size, pid_t pid);

/**
 * Find a process with a given job number.
 * @param   array   A pointer to the first element of an array.
 * @param   size    The size of the array.
 * @param   n       The job number to look for.
 * @return  A pointer to the cprocess struct.
 */
struct cprocess* findJobnum(struct cprocess* array, int size, int n);

/**
 * Fork and execute a command provided by an option struct.
 * @param	args	A pointer to the array of arguments (including path).
 * @param   argc    The number of arguments in the array.
 * @param   tracker A child process data tracker struct.
 */
void executeCommand(char** args, int argc, struct cprocess* tracker);

/**
 * Print statistics on a process and then reset the tracker so another can
 * use it later.
 * @param   tracker A child process data tracker struct.
 * @param   stats   Stats on the process.
 */
void cleanCommand(struct cprocess* tracker, struct rusage* stats);

#endif