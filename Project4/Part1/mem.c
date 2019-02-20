// Maximilian Thompson


#include <stdio.h>
#include <string.h>

#include "pageTable.h"

unsigned char mainMemory[PHYS_MEM_SIZE];
page_t procTable[MAX_PROCESSES];

int main() {
    #ifdef _DEBUG
        printf("Size of page struct: %u\n", sizeof(page_t));
    #endif

    // Initialize all proccess page tables to invalid.
    for (int i = 0; i < MAX_PROCESSES; i++) procTable->valid = 0;

    // Input loop.
    while(1) {
        char buf[120];
        printf("cmd: ");
        fgets(buf, 120, stdin);

        char* pidstr;
        char* instruction;
        char* addressstr;
        char* valuestr;

        pidstr = strtok(buf, ",");
        instruction = strtok(buf, ",");
        addressstr = strtok(buf, ",");
        valuestr = strtok(buf, ",");
        
        int pid = 0;
        int address = 0;
        int value = 0;

        if (pidstr != NULL) pid = atoi(pidstr);
        if (addressstr != NULL) address = atoi(addressstr);
        if (valuestr != NULL) value = atoi(valuestr);

        // MAP
        if (strcmp(instruction, "map,")) {
            map_page(pid, address, value);

        // STORE
        } else if (strcmp(instruction, "store,")) {
            store(pid, address, (unsigned char)value);

        // LOAD
        } else if (strcmp(instruction, "load,")) {
            unsigned char c;
            load(pid, address, &c);

        } else {
            printf("Invalid instruction. Please ensure it is in all lowercase\n");
            continue;
        }
    }
}