// Maximilian Thompson

#ifndef WAITTHREAD_H
#define WAITTHREAD_H

#include <pthread.h>

#include "cprocess.h"

#define MAX_BGPROCESSES	100

/**
 * Wait and wait and wait and wait. Resolve commands when background processes
 * finish.
 */
void* waitThread();

#endif
