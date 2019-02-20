// Maximilian Thompson

#ifndef PAGE_TABLE_H
#define PAGE_TABLE_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_PROCESSES   4

#define PHYS_MEM_SIZE   64
#define PAGE_SIZE       16
#define PAGE_FRAMES     (PHYS_MEM_SIZE / PAGE_SIZE)

typedef struct page {
    uint8_t valid:1;   // Is the page valid?
    uint8_t write:1;   // Is the memory writeable?
    uint8_t frame:6;   // What is the id of the frame?
} page_t;

/**
 * Check to see if a frame is currently assigned to a page.
 * @param frame The frame to check.
 * @return int  True if the frame is assigned.
 */
int frame_allocated(int frame);

/**
 * Find a frame currently assigned to no pages. If there are no free frames,
 * -1 is returned.
 * @return int The id of the frame.
 */
int find_free_frame();

/**
 * Find the page with id n belonging to the process provided. NULL is returned
 * when the process does not have a page table.
 * @param pid      The process looking for a page.
 * @param n        The page number.
 * @return page_t* A point to the page.
 */
page_t* find_page(int pid, int n);

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
