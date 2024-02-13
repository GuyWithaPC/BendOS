#include <limits.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

static bool print(const char* data, size_t length) {
	const unsigned char* bytes = (const unsigned char*) data;
	for (size_t i = 0; i < length; i++)
		if (putchar(bytes[i]) == EOF)
			return false;
	return true;
}

static bool printstr(const char* string) {
	size_t len = strlen(string);
	for (size_t i = 0; i < len; i++)
		if (putchar(string[i]) == EOF)
			return false;
	return true;
}

static bool printchar(const char c) {
	if (putchar(c) == EOF)
		return false;
	return true;
}

const char hex[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
const char dec[10] = {'0','1','2','3','4','5','6','7','8','9'};
char dec_buffer[20]; // 20 is the max size a decimal number can be

int printf(const char* restrict format, ...) {
	va_list parameters;
	va_start(parameters, format);

	int written = 0;

	while (*format != '\0') {
		size_t maxrem = INT_MAX - written;

		if (format[0] != '%' || format[1] == '%') {
			if (format[0] == '%')
				format++;
			size_t amount = 1;
			while (format[amount] && format[amount] != '%')
				amount++;
			if (maxrem < amount) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
			if (!print(format, amount))
				return -1;
			format += amount;
			written += amount;
			continue;
		}

		const char* format_begun_at = format++;

		if (*format == 'c') {
			format++;
			char c = (char) va_arg(parameters, int /* char promotes to int */);
			if (!maxrem) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
			if (!print(&c, sizeof(c)))
				return -1;
			written++;
		} else if (*format == 's') {
			format++;
			const char* str = va_arg(parameters, const char*);
			size_t len = strlen(str);
			if (maxrem < len) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
			if (!print(str, len))
				return -1;
			written += len;
		} else if (*format == 'x') {
			format++;
			size_t value = va_arg(parameters, size_t);
			printstr("0x");
			for (int i = sizeof(size_t) * 8 - 4; i >= 0; i -= 4) {
				printchar(hex[(value & (0xF << i)) >> i]);
			}
		} else if (*format == 'd') {
			format++;
			size_t value = va_arg(parameters, size_t);
			size_t remainder;
			size_t buffer_pos = 0;
			while (true) {
				remainder = value % 10;
				dec_buffer[buffer_pos] = dec[remainder];
				buffer_pos++;
				value /= 10;
				if (value == 0) break;
			}
			buffer_pos--;
			while (true) {
				printchar(dec_buffer[buffer_pos]);
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
				dec_buffer[buffer_pos] = dec[remainder];
				buffer_pos++;
				value /= 10;
				if (value == 0) break;
			}
			buffer_pos--;
			while (true) {
				printchar(dec_buffer[buffer_pos]);
				if (buffer_pos == 0) break;
				buffer_pos--;
			};
		} else {
			format = format_begun_at;
			size_t len = strlen(format);
			if (maxrem < len) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
			if (!print(format, len))
				return -1;
			written += len;
			format += len;
		}
	}

	va_end(parameters);
	return written;
}
