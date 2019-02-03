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

void init_ancestry(ancestry_t* a) {
    for (int i = 0; i < 10; i++) a->ancestors[i] = 0;
    for (int i = 0; i < 100; i++) a->siblings[i] = 0;
    for (int i = 0; i < 100; i++) a->children[i] = 0;
}

int main () {
    pid_t target = getpid();
    ancestry_t ancestry;
    init_ancestry(&ancestry);
    long code = syscall(__NR_cs3013_syscall2, &target, &ancestry);

    if (code < 0) {
        printf("Failed with error code %ld\n", code);
        abort();
    }

    printf("stuff goes here\n");
}