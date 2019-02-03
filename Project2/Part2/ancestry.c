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
    for (int i = 0; i < 10; i++) a->ancestors[i] = -1;
    for (int i = 0; i < 100; i++) a->siblings[i] = -1;
    for (int i = 0; i < 100; i++) a->children[i] = -1;
}

void print_siblings(ancestry_t* a) {
    printf("SIBLINGS:\n");
    for (int i = 0; i < 100 && a->siblings[i] != -1; i++)
        printf("%d, ", a->siblings[i]);
    
    printf("\n");
}

void print_children(ancestry_t* a) {
    printf("CHILDREN:\n");
    for (int i = 0; i < 100 && a->children[i] != -1; i++)
        printf("%d, ", a->children[i]);
    
    printf("\n");
}

void print_ancestors(ancestry_t* a) {
    printf("ANCESTORS:\n");
    for (int i = 9; i >= 0; i--)
        if (a->ancestors[i] > 0)
            printf("%d -> ", a->ancestors[i]);
    
    printf("{target}\n\n");
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

    print_children(&ancestry);
    print_siblings(&ancestry);
    print_ancestors(&ancestry);

    return 0;
}