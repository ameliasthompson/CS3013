// Maximilian Thompson


#include <stdio.h>
#include <string.h>

#include "pageTable.h"

unsigned char mainMemory[PHYS_MEM_SIZE];
storedFrame_t backingStore[STORE_FRAMES];
page_t procTable[MAX_PROCESSES];

int main() {
    #ifdef _DEBUG
        printf("Size of page struct: %u\n", sizeof(page_t));
    #endif

    // Initialize all proccess page tables to invalid.
    for (int i = 0; i < MAX_PROCESSES; i++) procTable[i].valid = 0;

    // Initialize backing store to unused.
    for (int i = 0; i < STORE_FRAMES; i++) backingStore[i].pid = -1;

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
        instruction = strtok(NULL, ",");
        addressstr = strtok(NULL, ",");
        valuestr = strtok(NULL, ",");
        
        int pid = 0;
        int address = 0;
        int value = 0;

        if (pidstr != NULL) pid = atoi(pidstr);
        if (addressstr != NULL) address = atoi(addressstr);
        if (valuestr != NULL) value = atoi(valuestr);

        // Check to see if it's out of bounds.
        if (address >= VIRT_ADD_SPACE || address < 0) {
            printf("Address out of bounds\n");
            continue;
        }

        // MAP
        if (strcmp(instruction, "map") == 0) {
            // Confirm arguments:
            if (value > 1 || value < 0) {
                printf("Write flag may only be 0 or 1\n");
                continue;
            }
            map_page(pid, address, value);

        // STORE
        } else if (strcmp(instruction, "store") == 0) {
            store(pid, address, (unsigned char)value);

        // LOAD
        } else if (strcmp(instruction, "load") == 0) {
            unsigned char c;
            load(pid, address, &c);

        } else {
            printf("Invalid instruction. Please ensure it is in all lowercase\n");
            continue;
        }
    }
}