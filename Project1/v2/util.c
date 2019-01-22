// Maximilian Thompson

#include "util.h"

void cleanNewline(char* str) {
	if(str[0] == '\n') { // strtok doesn't work if the first character is.
		str[0] = '\0';
	} else {
		strtok(str, "\n");
	}	
}
