
#include <kernel/paging.h>
#include <stdint.h>

void linear_map(page_table table, uint32_t from, uint32_t size) {
    from = from & 0xfffff000;
    unsigned int i = 0;
    for (; size > 0; from += 4096, size -= 4096, i++) {
        set_pagetable_mapping(table, i, from);
    }
}