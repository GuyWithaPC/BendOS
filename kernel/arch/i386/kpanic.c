
#include "vga.h"
#include <stddef.h>

static uint16_t* const VGA_MEMORY = (uint16_t*) 0xB8000;
static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

void printstr(char* str, uint8_t color, uint16_t* buffer, size_t* row, size_t* col) {
    while (*str != '\0') {
        if (*str == '\n') {
            if (++(*row) == VGA_HEIGHT) {
                // return from here, we're not going to scroll for now
                return;
            }
            *col = 0;
            return;
        }
        buffer[(*row)*VGA_WIDTH+(*col)] = vga_entry(*str, color);
        str++;
        if (++(*col) == VGA_WIDTH) {
            *col = 0;
            if (++(*row) == VGA_HEIGHT) {
                // return from here, just cut off text
                return;
            }
        }
    }
}

void kpanic(char* context) {
    uint16_t* terminal_buffer = VGA_MEMORY;
    uint8_t color = vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_RED);
    for (size_t clear = 0; clear < VGA_WIDTH*VGA_HEIGHT; clear++) {
        terminal_buffer[clear] = vga_entry(' ', color);
    }
    size_t row = 0;
    size_t col = 0;
    printstr("you fucked up\n", color, terminal_buffer, &row, &col);
    printstr("context: ", color, terminal_buffer, &row, &col);
    printstr(context, color, terminal_buffer, &row, &col);
}