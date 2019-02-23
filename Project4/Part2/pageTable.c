// Maximilian Thompson

#include "pageTable.h"

extern unsigned char mainMemory[PHYS_MEM_SIZE];
storedFrame_t backingStore[STORE_FRAMES];
extern page_t procTable[MAX_PROCESSES];

page_t* frame_allocated(int frame) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (procTable[i].valid) {
            if (procTable[i].frame == frame) {
                return &procTable[i]; // If that frame is a page table.
            } else {
                for (int j = procTable[i].frame * PAGE_SIZE;
                        j < procTable[i].frame * PAGE_SIZE + PAGE_SIZE;
                        j++) {

                    page_t* tmp = (page_t*)&mainMemory[j];
                    if (tmp->valid && tmp->frame == frame) {
                        return tmp; // If that frame is allocated.
                    }
                }
            }
        }
    }

    return NULL; // There was no frame.
}

int find_free_frame(int pid) {
    for (int i = 0; i < PAGE_FRAMES; i++) {
        // Find an unallocated frame.
        if (!frame_allocated(i)) return i;
    }

    return free_frame(pid);
}

int get_page_num(const page_t* page) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (page == &procTable[i]) {
            return -1; // That page is a hardware thing.
        } else if (procTable[i].valid) {     
            for (int j = 1; j < PAGE_SIZE; j++) {
                // Compare the addresses.
                if (page == (page_t*)&mainMemory[procTable[i].frame * PAGE_SIZE + j]) {
                    return j; // If that page is an entry.
                }
            }           
        }
    }

    return -2; // THIS WOULD BE REALLY BAD
}

int free_frame(int pid) {
    // Get the page of the frame we're operating on.
    static int robin = 0;
    page_t* page = frame_allocated(robin);

    // Skip the frame if it contains the table for the provided pid.
    if (page == &procTable[pid]) {
        printf("Skipped moving physical frame %d to the backing store\n", robin);
        robin++;
        if (robin >= PAGE_FRAMES) robin = 0;
        return free_frame(pid);
    }

    // If it's another processes table, we have to free everything.
    if (page >= &procTable[0] && page < &procTable[MAX_PROCESSES]) {
        printf("Unloading contents of page table for PID %d\n", page->procc);
        for (int i = 0; i < PAGE_SIZE; i++) {
            if (((page_t*)&mainMemory[page->frame * PAGE_SIZE + i])->valid)
                free_specific_frame((page_t*)&mainMemory[page->frame * PAGE_SIZE + i]);
        }
    }

    free_specific_frame(page);

    int ret = robin;
    robin++;
    if (robin >= PAGE_FRAMES) robin = 0;
    return ret;
}

void free_specific_frame(page_t* page) {
    // Get the page of the frame we're operating on.
    page->valid = 0; // Mark invalid.
    int frame = page->frame;

    // Find an unused portion of the backing store.
    int i = 0;
    for (; i < STORE_FRAMES; i++) {
        if (backingStore[i].pid < 0) {
            backingStore[i].pid = page->procc; // Claim the frame.
            backingStore[i].write = page->write;
            backingStore[i].page = get_page_num(page);

            // Copy the data in.
            for (int j = 0; j < PAGE_SIZE; j++) {
                backingStore[i].data[j] = mainMemory[page->frame * PAGE_SIZE + j];
            }

            
            break;
        }
    }

    // This frame is now free!
    printf("Moved physical frame %d to backing store slot %d\n", frame, i);
}

page_t* find_page(int pid, int n) {
    if (!procTable[pid].valid) {
        // If the table isn't loaded, we need to load it.
        init_table(pid);
    } 
    
    return (page_t*)&mainMemory[procTable[pid].frame * PAGE_SIZE + n];
}

int check_backing_store(int pid, int pageNum) {
    for (int i = 0; i < STORE_FRAMES; i++) {
        if (backingStore[i].pid == pid && backingStore[i].page == pageNum) {
            // We found it!!!

            int frame = find_free_frame(pid); // We need a frame to stick this in.

            // Copy the data over.
            for (int j = 0; j < PAGE_SIZE; j++) {
                mainMemory[frame * PAGE_SIZE + j] = backingStore[i].data[j];
            }

            // Insert the page into the appropriate table.
            if (pageNum == -1) {
                // It's a hardware page.
                procTable[pid].frame = frame;
                procTable[pid].procc = pid;
                procTable[pid].valid = 1;
                procTable[pid].write = backingStore[i].write;
            
            } else {
                // It's not a hardware page.
                page_t* tmp = find_page(pid, pageNum);
                tmp->frame = frame;
                tmp->procc = pid;
                tmp->valid = 1;
                tmp->write = backingStore[i].write;
            }

            if (pageNum == -1) {
                printf("Loaded page table for PID %d from the backing store into physical frame %d\n", pid, frame);
            } else {
                printf("Loaded page %d for PID %d from the backing store into physical frame %d\n", pageNum, pid, frame);
            }
            backingStore[i].pid = -1; // Free the slot.
            return 1;
        }
    }

    return 0;
}

void init_table(int pid) {
    // Exit if we can load it from the backing store.
    if (check_backing_store(pid, -1)) return;

    // Otherwise make it happen.
    int frame = find_free_frame(pid);

    if(frame >= 0) {
        // If there's a free frame, point the page table at it.
        procTable[pid].valid = 1;
        procTable[pid].procc = pid;
        procTable[pid].write = 1;
        procTable[pid].frame = frame;

        // Initialize all the pages to invalid.
        page_t* tmp = (page_t*)&mainMemory[frame * PAGE_SIZE];
        for (int j = 0; j < PAGE_SIZE; j++) {
            (tmp + j)->valid = 0;
            (tmp + j)->procc = pid;
        }

        printf("Put page table for PID %d in physical frame %d\n", pid, frame);

    } else {
        // If we get here, then there was no space.
        printf("Unable to find a free physical frame for PID %d page table\n", pid);
    }
}

int map_page(int pid, int address, int write) {

    // Divide and round down the address to find the right page.
    page_t* page = find_page(pid, address / PAGE_SIZE);
    
    // Check to see if the page is already allocated.
    if (page->valid) {
        printf("Updating write permissions on page %d\n", address / PAGE_SIZE);
        page->write = write;
        return 0;
    }

    // Check the backing store, and map if it isn't there.
    if (!check_backing_store(pid, address / PAGE_SIZE)) {
        // Find a free frame.
        int frame = find_free_frame(pid);
        if (frame < 0) {
            printf("Unable to find a free physical frame for new page\n");
            return 0;
        }
            
        // Set up the page.
        page->frame = frame;
        page->valid = 1;
        page->write = write;
        printf("Mapped virtual address %d (page %d) to physical frame %d\n", address, address / PAGE_SIZE, frame);
    }

    if (page->write != write) {
        printf("Updating write permissions on page %d\n", address / PAGE_SIZE);
        page->write = write;
    }
    return 1;
}

int store(int pid, int address, unsigned char value) {
    // If there's no page table, then no one's mapped anything to it.
    if (!procTable[pid].valid && !check_backing_store(pid, -1)) {
        printf("PID %d has no pages associated with it\n", pid);
        return 0;
    }

    page_t* page = find_page(pid, address / PAGE_SIZE);
    int offset = address % PAGE_SIZE;

    // Check to see if the page is valid.
    if(!page->valid && !check_backing_store(pid, address / PAGE_SIZE)) {
        printf("Virtual address %d has no frame mapped for PID %d\n", address, pid);
        return 0;
    }
    
    // Check to see if writing is allowed.
    if (!page->write) {
        printf("Writing is forbidden at virtual address %d for PID %d\n", address, pid);
        return 0;   
    }
    
    // Finally do the writing.
    mainMemory[page->frame * PAGE_SIZE + offset] = value;
    printf("Stored value %d at virtual address %d (physical address %d)\n", (int)value, address, page->frame * PAGE_SIZE + offset);
    return 1;
}

int load(int pid, int address, unsigned char* out) {
    // If there's no page table, the no one's mapped anything to it.
    if (!procTable[pid].valid && !check_backing_store(pid, -1)) {
        printf("PID %d has no pages associated with it\n", pid);
        return 0;
    }

    page_t* page = find_page(pid, address / PAGE_SIZE);
    int offset = address % PAGE_SIZE;

    // Check to see if the page is valid.
    if(!page->valid && !check_backing_store(pid, address / PAGE_SIZE)) {
        printf("Virtual address %d has no frame mapped for PID %d\n", address, pid);
        return 0;
    }

    //Finally do the reading.
    *out = mainMemory[page->frame * PAGE_SIZE + offset];
    printf("Read value %d at virtual address %d (physical address %d)\n", (int)*out, address, page->frame * PAGE_SIZE + offset);
    return 1;
}