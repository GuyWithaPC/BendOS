#ifndef _KERNEL_SERIAL_H
#define _KERNEL_SERIAL_H

#include <stdbool.h>

/// @brief Check whether the serial port is enabled for this architecture
bool has_serial(void);
/// @brief Initialize the serial port
/// @return 0 if the operation was successful,
///         1 if the serial port is faulty,
///         2 if serial ports are disabled for this architecture
int init_serial(void);
/// @brief Return true if there is data on the serial port
bool serial_received(void);
/// @brief Read the next byte from the serial port
/// @note this function will block until data arrives!
/// @return the next byte that comes in on the serial port
char read_serial(void);
/// @brief Return true if the serial port has no outgoing data
bool is_transmit_empty(void);
/// @brief Write a byte to the serial port
/// @note  This function will block if there is outgoing data!
/// @param a the byte to write
void write_serial(char a);

/// @brief Write a null-terminated string to the serial port
/// @note  if the string is large, this function may take a long time!
void serial_writestring(const char* s);
/// @brief Write a formatted string to the serial port
/// @note  if the string is large, this might take a while!
void serial_printf(const char* restrict format, ...);

#endif