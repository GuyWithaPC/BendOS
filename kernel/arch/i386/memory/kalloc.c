
#include <kernel/mem.h>
#include <kernel/serial.h>
#include <stddef.h>

// how many 4KB blocks have been allocated
// right now this is just a watermark allocator, there is no free
// TODO: add freeing and free zones
static volatile size_t used_blocks;

void* next_aligned(void* addr) {
    size_t addr_val = (size_t)addr;
    if ((addr_val & 0xFFFFF000) < addr_val) {
        return (void*)((addr_val & 0xFFFFF000) + 0x1000);
    }
    return (void*)(addr_val & 0xFFFFF000);
}

void enable_phys_alloc(void) {
    used_blocks = 0;
}

// get the block number which this address would be in
int map_to_block(void* addr) {
    size_t ktop = (size_t)(next_aligned(KERNEL_END));
    size_t addr_aligned = (size_t)(next_aligned(addr));
    return (addr_aligned - ktop) / 0x1000;
}

static void add_blocks(size_t blocks) {
    used_blocks += blocks;
}

void* kphys_alloc(size_t blocks) {
    //terminal_initialize();
    void* address = next_aligned(KERNEL_END) + used_blocks * 0x1000;
    add_blocks(blocks);
    return address;
}