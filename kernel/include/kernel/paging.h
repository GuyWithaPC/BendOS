#ifndef _KERNEL_PAGING_H
#define _KERNEL_PAGING_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <kernel/kpanic.h>

// fields on both
#define PRESENT      0x01
#define READWRITE    0x02
#define USRSUPR      0x04
#define WRITETHROUGH 0x08
#define CACHEDISABLE 0x10
#define ACCESSED     0x20
#define ADDRESS      0xFFFFF000

// field on only pagedir
#define PAGEDIR_PAGESIZE 0x80

// fields on only pagetable
#define PAGETABLE_DIRTY  0x40
#define PAGETABLE_PAT    0x80
#define PAGETABLE_GLOBAL 0x100

typedef uint32_t pd_entry;
typedef uint32_t pt_entry;

typedef pt_entry* page_table;
typedef pd_entry* page_directory;

/// @brief Get the page table a page directory entry points to
/// @param directory the page directory containing the desired entry
/// @param index the index of the page directory entry
/// @return the page table this page directory entry points to, or NULL if it's marked not present
/// @exception this will call a panic if index is greater than or equal to 1024.
static inline page_table get_pagedir_entry(page_directory directory, unsigned int index) {
    if (index >= 1024)
        kpanic("Tried to access a page directory entry out of bounds");
    if (!directory[index] & PRESENT)
        return NULL;
    return (page_table)(directory[index] & ADDRESS);
}

/// @brief Set the page table a page directory entry points to
/// @param directory the page directory to modify
/// @param index the index of the page directory entry to modify
/// @param table the page table which the page directory entry should point to
/// @note this will cause serious issues if your page table isn't page aligned!
/// @exception this will call a panic if index is greater than 1024.
static inline void set_pagedir_entry(page_directory directory, unsigned int index, page_table table) {
    if (index >= 1024)
        kpanic("Tried to modify a page directory entry out of bounds");
    directory[index] &= ~ADDRESS;
    directory[index] |= PRESENT;
    directory[index] |= (uint32_t)table;
}

/// @brief Delete a page directory entry
/// @param directory the page directory to delete the entry from
/// @param index the index of the entry to delete
/// @exception this will call a panic if index is greater than 1024.
static inline void delete_pagedir_entry(page_directory directory, unsigned int index) {
    if (index >= 1024)
        kpanic("Tried to modify a page directory entry out of bounds");
    directory[index] &= ~PRESENT;
}

/// @brief Get the mapping address of an entry in a page table
/// @param table the table to get the entry from
/// @param index the index of the entry
/// @param flag  set to 1 if the entry exists, 0 otherwise (can be NULL, then it's ignored)
/// @return the address the page table entry points to (as an integer)
/// @exception this will call a panic if index is greater than 1024.
static inline uint32_t get_pagetable_mapping(page_table table, unsigned int index, bool* flag) {
    if (index >= 1024)
        kpanic("Tried to access a page table entry out of bounds");
    if (flag)
        *flag = table[index] & PRESENT;
    return (table[index] & ADDRESS);
}

/// @brief Set the mapping address of an entry in a page table
/// @param table the table to modify
/// @param index the index of the table entry to modify
/// @param address the address to make the table entry point to
/// @note this will cause serious issues if the address isn't page-aligned!
/// @exception this will call a panic if index is greater than 1024.
static inline void set_pagetable_mapping(page_table table, unsigned int index, uint32_t address) {
    if (index >= 1024)
        kpanic("Tried to modify a page table entry out of bounds");
    table[index] &= ~ADDRESS;
    table[index] |= PRESENT;
    table[index] |= address;
}

/// @brief Delete a page table entry
/// @param table the page table to delete the entry from
/// @param index the index of the entry to delete
/// @exception this will call a panic if index is greater than 1024.
static inline void delete_pagetable_mapping(page_table table, unsigned int index) {
    if (index >= 1024)
        kpanic("Tried to modify a page table entry out of bounds");
    table[index] &= ~PRESENT;
}

extern void load_page_directory(page_directory);

extern page_directory get_page_directory(void);

extern void enable_paging(void);

void linear_map(page_table table, uint32_t from, uint32_t size);

#endif