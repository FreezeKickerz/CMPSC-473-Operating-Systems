/*
 * mm.c
 *
 * Name: Tyler Korz
 *
 * This implementation manages dynamic memory allocation by maintaining an explicit,
 * doubly-linked free list. Each memory block includes a header and footer that encode
 * the block's size and allocation status using bit flags. The allocator supports block
 * splitting to efficiently utilize memory and coalescing of adjacent free blocks to
 * minimize fragmentation. All blocks are aligned to 16 bytes, and standard routines
 * (malloc, free, realloc, and calloc) are provided along with heap consistency checking
 * (mm_checkheap) when debugging is enabled.
 *
 */
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>

#include "mm.h"
#include "memlib.h"

/*
 * If you want to enable your debugging output and heap checker code,
 * uncomment the following line. Be sure not to have debugging enabled
 * in your final submission.
 */
//#define DEBUG 

#ifdef DEBUG
// When debugging is enabled, the underlying functions get called
#define dbg_printf(...) printf(__VA_ARGS__)
#define dbg_assert(...) assert(__VA_ARGS__)
#else
// When debugging is disabled, no code gets generated
#define dbg_printf(...)
#define dbg_assert(...)
#endif // DEBUG

// do not change the following!
#ifdef DRIVER
// create aliases for driver tests
#define malloc mm_malloc
#define free mm_free
#define realloc mm_realloc
#define calloc mm_calloc
#define memset mm_memset
#define memcpy mm_memcpy
#endif // DRIVER

#define ALIGNMENT 16
// Global pointer
static void* search(size_t);
/* Block Structure */
typedef struct Block{
    size_t size_node;
    struct Block* next_node;
    struct Block* prev_node;
} Block;

//Free Block pointer
Block* free_block_pointer;

void coalesce(Block* pointer);

// Flag definitions
//allocated block flag (bit 0)
//previous block allocated flag (bit 1)

// rounds up to the nearest multiple of ALIGNMENT
static size_t align(size_t x)
{
    return ALIGNMENT * ((x+ALIGNMENT-1)/ALIGNMENT);
}

/********** Helper Functions **********/

// Removes a block from the free list.
static inline void remove_from_free_list(Block* block) {
    if (block->prev_node) block->prev_node->next_node = block->next_node; // Update previous block's next pointer
    else free_block_pointer = block->next_node;                          // Update free list head if block is first
    if (block->next_node) block->next_node->prev_node = block->prev_node; // Update next block's previous pointer
}
// Inserts a block at the beginning of the free list.
static inline void add_to_free_list(Block* block) {
    block->prev_node = NULL;                     // Set block's previous pointer to NULL
    block->next_node = free_block_pointer;       // Link block to current head
    if (free_block_pointer) free_block_pointer->prev_node = block; // Update current head's previous pointer
    free_block_pointer = block;                  // Update free list head
}

/*
 * mm_init: returns false on error, true on success.
 */


 bool mm_init(void)
 {
    void* heap_start = mm_sbrk(4096 + 16);       // Extend heap: 4096 bytes payload + 16 bytes for prologue/epilogue
    if (heap_start == (void*)(-1)) return false;   // Check for sbrk failure
    size_t* heap_end = mm_heap_hi() + 1;           // Pointer one past the end of the heap

    *((size_t*)heap_start) = 1;                  // Set prologue header
    Block* free_block = (Block*)((char*)heap_start + 8); // Create free block after prologue header
    free_block->size_node = 4096;                // Set free block size
    free_block->prev_node = NULL;                // Initialize previous pointer
    free_block->next_node = NULL;                // Initialize next pointer
    free_block->size_node ^= 2;                  // Toggle previous free flag
    free_block_pointer = free_block;             // Set free list pointer

    *(heap_end - 2) = 4096;                      // Set free block footer
    *(heap_end - 1) = 1;                         // Set epilogue header

    return true;
 }


/*
 * malloc
 */
void* malloc(size_t size) {
    if (size == 0) return NULL;                  // Return NULL for zero size
    size_t required_block_size = align(size + 8);  // Compute block size (payload + header)
    if (required_block_size < 32) required_block_size = 32; // Enforce minimum block size

    Block* block = search(required_block_size);  // Search free list for a fitting block
    if (block) {
        size_t current_size = block->size_node & ~3; // Get actual block size
        size_t remaining_size = current_size - required_block_size; // Calculate remaining size

        if (remaining_size <= 32 || current_size == required_block_size) { // Use whole block if split not possible
            if (block->size_node & 2) {              // Check previous free flag
                block->size_node = current_size | 1; // Mark block as allocated
                block->size_node ^= 2;               // Toggle previous flag
            } else {
                block->size_node = current_size | 1; // Mark block as allocated
            }
            *((size_t*)block + (current_size / sizeof(size_t))) ^= 2; // Update next block's prev flag
            remove_from_free_list(block);          // Remove block from free list
            return (size_t*)block + 1;             // Return pointer to payload
        } else {
            size_t extra_size = current_size - required_block_size; // Calculate size for free remainder
            if (block->size_node & 2) {
                block->size_node = extra_size;    // Update free block size
                block->size_node ^= 2;             // Toggle previous flag
            } else {
                block->size_node = extra_size;    // Update free block size
            }
            *((size_t*)block + (extra_size / sizeof(size_t)) - 1) = extra_size; // Set footer for free remainder
            size_t* alloc_header = (size_t*)block + (extra_size / sizeof(size_t)); // Locate header for allocated block
            *alloc_header = required_block_size | 1; // Set allocated block header
            *((size_t*)block + (current_size / sizeof(size_t))) ^= 2; // Update next block's prev flag
            return alloc_header + 1;             // Return pointer to allocated payload
        }
    } else {
        void* new_block_ptr = mm_sbrk(required_block_size); // Extend heap if no free block found
        if (new_block_ptr == (void*)-1) return NULL; // Check for sbrk failure
        size_t* new_heap_end = mm_heap_hi() + 1;   // Get new heap end pointer
        Block* new_block_header = (Block*)((char*)new_block_ptr - 8); // Locate new block header
        if (new_block_header->size_node & 2) {
            new_block_header->size_node = required_block_size | 1; // Set allocated header with toggle
            new_block_header->size_node ^= 2;
        } else {
            new_block_header->size_node = required_block_size | 1; // Set allocated header
        }
        *(new_heap_end - 1) = 3;                   // Update epilogue header
        return new_block_ptr;                      // Return pointer to new block payload
    }
}


/*
 * free
 */ 
void free(void* ptr) {
    if (!ptr) return;                          // Do nothing for NULL pointer
    Block* block = (Block*)((char*)ptr - 8);     // Retrieve block header from payload pointer
    block->size_node &= ~1;                      // Mark block as free
    size_t block_size = block->size_node & ~3;     // Get block size
    *((size_t*)block + (block_size / sizeof(size_t)) - 1) = block_size; // Set free block footer
    *((size_t*)block + (block_size / sizeof(size_t))) &= ~2; // Clear next block's previous free flag
    add_to_free_list(block);                     // Add block to free list
    coalesce(block);                           // Coalesce adjacent free blocks
}

/*
 * realloc
 */
void* realloc(void* oldptr, size_t size) {
    if (!oldptr) return malloc(size);           // If oldptr is NULL, behave like malloc
    if (size == 0) { free(oldptr); return NULL; } // Free block if new size is zero

    void* newMemory = malloc(size);             // Allocate new block
    if (newMemory) {
        Block* old_block = (Block*)((char*)oldptr - 8); // Get old block header
        size_t old_block_size = old_block->size_node & ~3; // Get old block size
        size_t bytes_to_copy = (size > old_block_size) ? old_block_size : size; // Determine copy size
        memcpy(newMemory, oldptr, bytes_to_copy); // Copy data to new block
    }
    free(oldptr);                               // Free the old block
    return newMemory;                           // Return new block pointer
}

/*
 * calloc
 * This function is not tested by mdriver, and has been implemented for you.
 */
void* calloc(size_t nmemb, size_t size)
{
    void* ptr;
    size *= nmemb;
    ptr = malloc(size);
    if (ptr) {
        memset(ptr, 0, size);
    }
    return ptr;
}

//Searches for an open location that satisfies size
static void* search(size_t size){
    Block* look = free_block_pointer;

    //Keep searching through list until suitable block or end of list
    while(look){
        //Masks last 2 bits to get actual block size
        if (((look->size_node) & (~3)) >= size){
            return look;
        }
        look = look->next_node;
    }
    return NULL;
}

void coalesce(Block* block) {
    size_t current_size = block->size_node & ~3; // Get current block size
    size_t merged_size = current_size;           // Initialize merged size
    bool prev_free = ((block->size_node & 2) == 0); // Check if previous block is free
    Block* merged_block = block;                 // Start with current block

    size_t* prev_footer = (size_t*)block - 1;      // Locate previous block's footer
    size_t prev_size = *prev_footer & ~3;          // Get previous block size
    size_t* prev_header = (size_t*)block - (prev_size / sizeof(size_t)); // Locate previous block header
    if (prev_free && (prev_header >= (size_t*)mm_heap_lo() + 1) &&
        (prev_header < (size_t*)mm_heap_hi() - 7)) { // If previous block is free and in bounds
        Block* prev_block = (Block*)prev_header; // Cast to Block pointer
        remove_from_free_list(prev_block);       // Remove previous block from free list
        merged_size += prev_block->size_node & ~3; // Add its size to merged size
        merged_block = prev_block;               // Set merged block to previous block
    }

    size_t* next_header = (size_t*)block + (current_size / sizeof(size_t)); // Locate next block header
    if ((next_header >= (size_t*)mm_heap_lo() + 1) &&
        (next_header < (size_t*)mm_heap_hi() - 7) &&
        ((*next_header & 1) == 0)) {             // If next block is free and in bounds
        Block* next_block = (Block*)next_header; // Cast to Block pointer
        remove_from_free_list(next_block);       // Remove next block from free list
        merged_size += next_block->size_node & ~3; // Add its size to merged size
    }

    if (merged_size != current_size) {           // If coalescing occurred
        remove_from_free_list(block);            // Remove current block from free list
        merged_block->size_node = merged_size;     // Update header with merged size
        merged_block->size_node ^= 2;              // Toggle previous free flag as needed
        *((size_t*)merged_block + (merged_size / sizeof(size_t)) - 1) = merged_size; // Update merged footer
        add_to_free_list(merged_block);            // Add merged block to free list
    }
}

/*
 * Returns whether the pointer is in the heap.
 * May be useful for debugging.
 */
static bool in_heap(const void* p)
{
    return p <= mm_heap_hi() && p >= mm_heap_lo();
}

/*
 * Returns whether the pointer is aligned.
 * May be useful for debugging.
 */
static bool aligned(const void* p)
{
    size_t ip = (size_t) p;
    return align(ip) == ip;
}

/*
 * mm_checkheap
 * You call the function via mm_checkheap(__LINE__)
 * The line number can be used to print the line number of the calling
 * function where there was an invalid heap.
 */
bool mm_checkheap(int line_number)
{
#ifdef DEBUG
    // Define valid heap bounds (skip prologue and near epilogue)
    size_t* heap_low_bound  = (size_t*)mm_heap_lo() + 1;                    
    size_t* heap_high_bound = (size_t*)((char*)mm_heap_hi() - 7);            

    // Check header and footer invariants for each free block in the free list
    for (Block* checker = free_block_pointer; checker != NULL; 
         checker = checker->next_node) {
        size_t* header_loc = (size_t*)checker;                              // Block header pointer
        size_t header_size = *header_loc & ~3;                                // Extract size from header
        size_t* footer_loc = header_loc + (header_size / sizeof(size_t)) - 1;   // Block footer pointer

        if (header_loc < heap_low_bound || header_loc >= heap_high_bound)     // Verify header is in bounds
            dbg_printf("header oob");
        if (footer_loc < heap_low_bound || footer_loc >= heap_high_bound)     // Verify footer is in bounds
            dbg_printf("footer oob");
    }

    // Scan the entire heap to ensure all free blocks are in the free list
    for (size_t* begin = heap_low_bound; begin < heap_high_bound; ) {
        Block* blk = (Block*)begin;                                           // Interpret pointer as block
        if ((blk->size_node & 1) == 0) {                                        // If block is free (allocated bit clear)
            bool in_free_list = false;                                        
            for (Block* free_iter = free_block_pointer; free_iter != NULL; 
                 free_iter = free_iter->next_node) {                           // Check each free block in the free list
                if (begin == (size_t*)free_iter) {                              // Found block in free list
                    in_free_list = true;
                    break;
                }
            }
            if (!in_free_list)                                                // Warn if free block is missing from free list
                dbg_printf("Free block not in free list at: %p\n", begin);
        }
        // Advance to next block using the current block's size (ignoring flag bits)
        size_t block_size = blk->size_node & ~3;                              
        begin += block_size / sizeof(size_t);
    }
#endif // DEBUG
    return true;
}