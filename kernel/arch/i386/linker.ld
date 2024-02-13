/* The bootloader will look at this image and start execution at the symbol
   designated at the entry point. */
ENTRY(_start)
OUTPUT_FORMAT(elf32-i386)

/* Tell where the various sections of the object files will be put in the final
   kernel image. */
SECTIONS
{

	/* start boot entry at 1M (a normal place for bootloaders to check) */
	. = 1M;

	_kernel_start = .;

	.text BLOCK(4K) : ALIGN(4K)
	{
		*(.multiboot.hdr)
		*(.text)
	}

	.rodata BLOCK(4K) : ALIGN(4K)
	{
		*(.rodata)
	}

	.data BLOCK(4K) : ALIGN(4K)
	{
		*(.data)
	}

	/* Read-write data (initialized) */
	.bss BLOCK(4K) : ALIGN(4K)
	{
		*(COMMON)
		*(.bss)
	}

	.bpd BLOCK(4K) : ALIGN(4K)
	{
		*(.bpd)
		*(.bpd.tables.*)
	}

	_kernel_end = .;

	/DISCARD/ : {
		*(.comment*);
	}
}
