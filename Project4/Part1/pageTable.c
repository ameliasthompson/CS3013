// Maximilian Thompson

#include "pageTable.h"

extern unsigned char mainMemory[PHYS_MEM_SIZE];
extern page_t procTable[MAX_PROCESSES];

int frame_allocated(int frame) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (procTable[i].valid) {
            if (procTable[i].frame == frame) {
                return 1; // If that frame is a page table.
            } else {
                for (int j = procTable[i].frame * PAGE_SIZE;
                        j < procTable[i].frame * PAGE_SIZE + PAGE_SIZE;
                        j++) {

                    page_t* tmp = (page_t*)&mainMemory[j];
                    if (tmp->valid && tmp->frame == frame) {
                        return 1; // If that frame is allocated.
                    }
                }
            }
        }
    }

    return 0;
}

int find_free_frame() {
    for (int i = 0; i < PAGE_FRAMES; i++) {
        // Find an unallocated frame.
        if (!frame_allocated(i)) return i;
    }

    return -1;
}

page_t* find_page(int pid, int n) {
    if (!procTable[pid].valid) {
        return NULL;
    } else {
        return procTable[pid].frame * PAGE_SIZE + n;
    }
}

void init_table(int pid) {
    int frame = find_free_frame();

    if(frame >= 0) {
        // If there's a free frame, point the page table at it.
        procTable[pid].valid = 1;
        procTable[pid].write = 1;
        procTable[pid].frame = frame;

        // Initialize all the pages to invalid.
        page_t* tmp = (page_t*)&mainMemory[frame * PAGE_SIZE];
        for (int j = 0; j < PAGE_SIZE; j++) {
            (tmp + j)->valid = 0;
        }

        printf("Put page table for PID %d in physical frame %d\n", pid, frame);

    } else {
        // If we get here, then there was no space.
        printf("Unable to find a free physical frame for PID %d page table\n", pid);
    }
}

int map_page(int pid, int address, int write) {
    if (!procTable[pid].valid) {
        // If there's no page table, we need to make one.
        init_table(pid);

        // If there's still no page table, then we failed to make one.
        if (!procTable[pid].valid) {
            printf("Unable to find page table for PID %d\n", pid);
            return 0;
        }
    }

    
    int frame = find_free_frame();
    if (frame < 0) {
        printf("Unable to find a free physical frame for new page\n");
        return 0;
    }
        
    // Divide and round down the address to find the right page.
    page_t* page = find_page(pid, address / PAGE_SIZE);
    page->frame = frame;
    page->valid = 1;
    page->write = write;
    return 1;
}

int store(int pid, int address, unsigned char value) {
    // If there's no page table, then no one's mapped anything to it.
    if (!procTable[pid].valid) {
        printf("PID %d has no pages associated with it\n", pid);
        return 0;
    }

    page_t* page = find_page(pid, address / PAGE_SIZE);
    int offset = address % PAGE_SIZE;

    // Check to see if the page is valid.
    if(!page->valid) {
        printf("Address %d has no frame mapped for PID %d\n", address, pid);
        return 0;
    }
    
    // Check to see if writing is allowed.
    if (!page->write) {
        printf("Writing is forbidden at %d for PID %d\n", address, pid);
        return 0;   
    }
    
    // Finally do the writing.
    mainMemory[page->frame * PAGE_SIZE + offset] = value;
    return 1;
}

int load(int pid, int address, unsigned char* out) {
    // If there's no page table, the no one's mapped anything to it.
    if (!procTable[pid].valid) {
        printf("PID %d has no pages associated with it\n", pid);
        return 0;
    }

    page_t* page = find_page(pid, address / PAGE_SIZE);
    int offset = address % PAGE_SIZE;

    // Check to see if the page is valid.
    if(!page->valid) {
        printf("Address %d has no frame mapped for PID %d\n", address, pid);
        return 0;
    }

    //Finally do the reading.
    *out = mainMemory[page->frame * PAGE_SIZE + offset];
    return 1;
}