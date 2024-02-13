#include <kernel/serial.h>
#include <kernel/io.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdint.h>
#include <limits.h>
#include <stddef.h>
#include <string.h>

#define COM1 0x3f8

bool has_serial(void) {
    return true;
}

int init_serial(void) {
    if (!has_serial())
        return 2;
    outb(COM1 + 1, 0x00); // disable interrupts
    outb(COM1 + 3, 0x80); // enable DLAB (baud rate divisor)
    outb(COM1 + 0, 0x03); // set divisor to 3 (lo byte) 38400 baud
    outb(COM1 + 1, 0x00); //                  (hi byte)
    outb(COM1 + 3, 0x03); // 8 bits, no parity, one stop bit
    outb(COM1 + 2, 0xC7); // Enable FIFO, clear them, with 14-byte threshold
    outb(COM1 + 4, 0x0B); // enable IRQs, RTS/DSR set
    outb(COM1 + 4, 0x1E); // Set in loopback mode, test chip
    outb(COM1 + 0, 0xAE); // Test chip (send 0xAE and check if it returns same byte)

    if (inb(COM1 + 0) != 0xAE) {
        return 1;
    }

    // if it isn't faulty set it in normal operation mode
    // (IRQ enabled, OUT#1 and OUT#2 enabled)
    outb(COM1 + 4, 0x0F);
    return 0;
}

bool serial_received(void) {
    return inb(COM1 + 5) & 1;
}

char read_serial(void) {
    while (serial_received() == 0);

    return inb(COM1);
}

bool is_transmit_empty(void) {
    return inb(COM1 + 5) & 0x20;
}

void write_serial(char a) {
    while (is_transmit_empty() == 0);

    outb(COM1,a);
}

void serial_writestring(const char* s) {
    while (*s != '\0') {
        write_serial(*s);
        s++;
    }
}

void serial_writestringlen(const char* s, size_t len) {
    for (size_t i = 0; i < len; i++) {
        write_serial(s[i]);
    }
}

const char shex[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
const char sdec[10] = {'0','1','2','3','4','5','6','7','8','9'};
char sdec_buffer[20]; // 20 is the max size a decimal number can be

void serial_printf(const char* restrict format, ...) {
	va_list parameters;
	va_start(parameters, format);

	int written = 0;

	while (*format != '\0') {
		if (format[0] != '%' || format[1] == '%') {
			if (format[0] == '%')
				format++;
			size_t amount = 1;
			while (format[amount] && format[amount] != '%')
				amount++;
			serial_writestringlen(format, amount);
			format += amount;
			written += amount;
			continue;
		}

		const char* format_begun_at = format++;

		if (*format == 'c') {
			format++;
			char c = (char) va_arg(parameters, int /* char promotes to int */);
            write_serial(c);
			written++;
		} else if (*format == 's') {
			format++;
			const char* str = va_arg(parameters, const char*);
			size_t len = strlen(str);
            serial_writestring(str);
			written += len;
		} else if (*format == 'x') {
			format++;
			size_t value = va_arg(parameters, size_t);
			serial_writestring("0x");
			for (int i = sizeof(size_t) * 8 - 4; i >= 0; i -= 4) {
				write_serial(shex[(value & (0xF << i)) >> i]);
			}
		} else if (*format == 'd') {
			format++;
			size_t value = va_arg(parameters, size_t);
			size_t remainder;
			size_t buffer_pos = 0;
			while (true) {
				remainder = value % 10;
				sdec_buffer[buffer_pos] = sdec[remainder];
				buffer_pos++;
				value /= 10;
				if (value == 0) break;
			}
			buffer_pos--;
			while (true) {
				write_serial(sdec_buffer[buffer_pos]);
				if (buffer_pos == 0) break;
				buffer_pos--;
			};
		} else if (*format == 'l') {
			format++;
			uint64_t value = va_arg(parameters, uint64_t);
			size_t remainder;
			size_t buffer_pos = 0;
			while (true) {
				remainder = value % 10;
				sdec_buffer[buffer_pos] = sdec[remainder];
				buffer_pos++;
				value /= 10;
				if (value == 0) break;
			}
			buffer_pos--;
			while (true) {
				write_serial(sdec_buffer[buffer_pos]);
				if (buffer_pos == 0) break;
				buffer_pos--;
			};
		} else {
			format = format_begun_at;
			size_t len = strlen(format);
            serial_writestring(format);
			written += len;
			format += len;
		}
	}

	va_end(parameters);
}