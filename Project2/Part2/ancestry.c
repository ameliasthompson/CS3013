// Maximilian Thompson

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/syscall.h>
#include <sys/types.h>

#define __NR_cs3013_syscall2 378

typedef struct {
    pid_t ancestors[10];
    pid_t siblings[100];
    pid_t children[100];
} ancestry_t;

int main () {
    pid_t target = getpid();
    ancestry_t* ancestry;
    long code = syscall(__NR_cs3013_syscall2, &target, &ancestry);

    if (code < 0) {
        printf("Failed with error code %ld\n", code);
        abort();
    }

    printf("stuff goes here\n");
}