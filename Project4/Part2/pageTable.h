// Maximilian Thompson

#ifndef PAGE_TABLE_H
#define PAGE_TABLE_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_PROCESSES   4

#define PHYS_MEM_SIZE   64
#define VIRT_ADD_SPACE  64
#define STORE_FRAMES    64
#define PAGE_SIZE       16
#define PAGE_FRAMES     (PHYS_MEM_SIZE / PAGE_SIZE)

typedef struct page {
    uint8_t valid:1;   // Is the page valid?
    uint8_t write:1;   // Is the memory writeable?
    uint8_t frame:4;   // What is the id of the frame?
    uint8_t procc:2;   // What is the ID of the process.
} page_t;

typedef struct storedFrame {
    unsigned char data[PAGE_SIZE];  // The data stored in the frame.
    int pid;                        // The PID of the owner.
    int page;                       // The page number. (-1 for page table page.)
    int write;                      // Can the frame be written to?
} storedFrame_t;

/**
 * Check to see if a frame is currently assigned to a page.
 * @param frame    The frame to check.
 * @return page_t* A pointer to the page which owns the frame.
 */
page_t* frame_allocated(int frame);

/**
 * Find a frame currently assigned to no pages. If there are no free frames,
 * a frame is freed, and then that one is used.
 * @return int The id of the frame.
 */
int find_free_frame();

/**
 * Frees a frame round robin style.
 * @return int The frame that was freed.
 */
int free_frame();

/**
 * Find the page with id n belonging to the process provided. NULL is returned
 * when the process does not have a page table.
 * @param pid      The process looking for a page.
 * @param n        The page number.
 * @return page_t* A point to the page.
 */
page_t* find_page(int pid, int n);

/**
 * Check the backing store for a page. If it is found, load it into memory and
 * return the page. If it is not found, NULL is returned.
 * @param pid      The process the page belongs to.
 * @param pageNum  The page number for the process.
 * @return page_t* The loaded page.
 */
page_t* check_backing_store(int pid, int pageNum);

/**
 * Allocate a page table for a process.
 * @param pid The process to allocate a table for.
 */
void init_table(int pid);

/**
 * Map the page the address would fall in for the process given to an unused
 * physical frame. Set the write protections while doing so.
 * @param pid     The process the page belongs to.
 * @param address The address to map.
 * @param write   The write protections.
 * @return int    Success or failure.
 */
int map_page(int pid, int address, int write);

/**
 * Store the value at the address provided in the page allocated for that
 * process.
 * @param pid     The process the data belongs to.
 * @param address The address to write to.
 * @param value   The value to write.
 * @return int    Success or failure.
 */
int store(int pid, int address, unsigned char value);

/**
 * Load the value stored at the address provided in the page allocated for that
 * process.
 * @param pid     The process the data belongs to.
 * @param address The address to read from.
 * @param out     Where to write the data read.
 * @return int    Success or failure.
 */
int load(int pid, int address, unsigned char* out);

#endif
