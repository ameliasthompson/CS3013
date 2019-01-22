// Maximilian Thompson

#ifndef UTIL_H
#define UTIL_H

#include <string.h>

/**
 * Replace the first newline character with a null terminator.
 * @note RESETS STRTOK
 * @arg	str	A pointer to the string to clean.
 */
void cleanNewline(char* str);

#endif
