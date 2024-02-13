
.section .text
.global load_page_directory
.type load_page_directory, @function
load_page_directory:
    push %ebp
    movl %esp, %ebp

    movl 8(%ebp), %eax
    movl %eax, %cr3

    popl %ebp
    ret