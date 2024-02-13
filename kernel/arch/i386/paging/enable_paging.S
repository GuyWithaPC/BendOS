
.section .text
.global enable_paging
.type enable_paging, @function
enable_paging:
    push %ebp
    movl %esp, %ebp

    mov %cr0, %eax
    or $0x80000000, %eax
    mov %eax, %cr0

    mov %ebp, %esp
    popl %ebp
    ret