#include <stdio.h>

#include <kernel/tty.h>
#include <kernel/paging.h>
#include <kernel/kpanic.h>
#include <kernel/serial.h>
#include <kernel/mem.h>

#include <string.h>
#include <multiboot.h>

void kernel_main(multiboot_info_t* mbd, unsigned int magic) {
	terminal_initialize();
	printf("Hello, kernel World!\n");

	// do serial port stuff
	printf("Testing serial port...\n");
	if (init_serial() == 0) {
		serial_writestring("Hello Serial World!\nThis is easier to look at than QEMU.\n");
	} else {
		kpanic("Serial port failed to initialize.");
	}
	printf("Serial port works!\n");

	// do memory map stuff
	printf("checking memory map...\n");
	serial_printf("\nMemory Map:\n");
	if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
		kpanic("invalid magic number on memory map!");
	}
	if (!(mbd->flags >> 6 & 0x1)) {
		kpanic("invalid memory map provided by GRUB!");
	}
	size_t total_memory_size = 0;
	size_t available_memory_size = 0;
	for (unsigned int i = 0; i < mbd->mmap_length; i += sizeof(multiboot_memory_map_t)) {
		multiboot_memory_map_t* mmmt = (multiboot_memory_map_t*) (mbd->mmap_addr + i);
		total_memory_size += mmmt->len_low;
		serial_printf("Start Addr: %x | Length: %x | Size: %x\n", mmmt->addr_low, mmmt->len_low, mmmt->size);
		serial_printf("Type: %s\n", MULTIBOOT_MEMORY_TYPES[mmmt->type - 1]);
		if (mmmt->type == 1) {
			available_memory_size += mmmt->len_low;
		}
	}
	serial_printf("%d of %d bytes available\n", available_memory_size, total_memory_size);
	printf("Successfully loaded memory map!\n");
	printf("%d of %d KB available\n", available_memory_size / 1024, total_memory_size / 1024);

	// enable paging
	printf("Enabling paging...\n");
	serial_printf("\nPaging:\n");
	enable_phys_alloc();
	// start by allocating and emptying a block for page directory
	page_directory main_pagedir = kphys_alloc(1);
	memset(main_pagedir, 0, 0x1000);
	// then we can allocate and empty blocks for page tables
	page_table kernel_pagetable = kphys_alloc(1);
	memset(kernel_pagetable, 0, 0x1000);
	serial_printf("allocated empty page directory at address %x\n", main_pagedir);
	serial_printf("allocated empty page table at address %x\n", kernel_pagetable);
	// loop the page directory back on itself (this will make it easier to find it if we do wonky paging stuff)
	set_pagedir_entry(main_pagedir, 1023, (page_table)main_pagedir);
	// identity map the page table and add it to the page directory
	linear_map(kernel_pagetable, 0, 0x400000);
	set_pagedir_entry(main_pagedir, 0, kernel_pagetable);
	serial_printf("The first 4 entries of the page directory are:\n");
	for (unsigned int i = 0; i < 4; i++) {
		page_table table = get_pagedir_entry(main_pagedir, i);
		serial_printf("Address: %x | Present: %s\n", table, table ? "TRUE" : "FALSE");
	}
	serial_printf("The first 4 entries of the page table are:\n");
	for (unsigned int i = 0; i < 4; i++) {
		bool exists = false;
		uint32_t addr = get_pagetable_mapping(kernel_pagetable, i, &exists);
		serial_printf("Address: %x | Present: %s\n", addr, exists ? "TRUE" : "FALSE");
	}
	load_page_directory(main_pagedir);
	serial_printf("System set to use the page directory at address %x\n", get_page_directory());
	enable_paging();
	serial_printf("Paging enabled.");
	printf("Paging enabled!");
}
