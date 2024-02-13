
.section .text
.global get_page_directory
.type get_page_directory, @function
get_page_directory:
    push %ebp
    movl %esp, %ebp

    movl %cr3, %eax

    popl %ebp
    ret