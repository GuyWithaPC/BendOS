.extern kernel_main

# Multiboot header in lower memory
.set MBALIGN,  1<<0
.set MEMINFO,  1<<1
.set FLAGS,    MBALIGN | MEMINFO
.set MAGIC,    0x1BADB002
.set CHECKSUM, -(MAGIC + FLAGS)

.set STACKSIZE, 32*1024                     # 32 KB stack

# multiboot header (for GRUB)
.section .multiboot.hdr
.align 4
MultiBootHeader:
    .long MAGIC
    .long FLAGS
    .long CHECKSUM


# entry point for linker
.section .text
.align 16
.global _start
.type _start, @function
_start:
    movl $stack_top, %esp # initialize stack
    push %eax # multiboot magic number
    push %ebx # multiboot entry
    
    # enter high-level kernel
    call _init
    call kernel_main

    # hang if kernel exits
    cli
_hang:
    hlt
    jmp _hang

# set up the stack above everything else
.section .bss
.align 32
stack_bottom:
    .skip STACKSIZE
stack_top: