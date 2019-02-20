// Maximilian Thompson


#include <stdio.h>

#include "pageTable.h"



unsigned char mainMemory[PHYS_MEM_SIZE];
page_t* procTable[MAX_PROCESSES];

int main() {
    #ifdef _DEBUG
        printf("Size of page struct: %u\n", sizeof(page_t));
    #endif


}