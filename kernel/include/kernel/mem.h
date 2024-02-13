#ifndef _KERNEL_MEM_H
#define _KERNEL_MEM_H

#include <stddef.h>

extern int _kernel_start;
extern int _kernel_end;


#define KERNEL_START (void*)&_kernel_start
#define KERNEL_END   (void*)&_kernel_end

void enable_phys_alloc(void);

/// @brief Allocate a contiguous, 4KB aligned section of `blocks` 4KB blocks.
/// @param blocks the number of 4KB blocks to allocate
/// @return a pointer to the start of the allocated space
void* kphys_alloc(size_t blocks);

/// @brief Get the next page-aligned address following the given address
void* next_aligned(void* addr);

#endif