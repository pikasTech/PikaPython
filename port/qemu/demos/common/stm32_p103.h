#ifndef __STM32_P103_H
#define __STM32_P103_H

#include "stm32f10x.h"

/* This library contains routines for interfacing with the STM32 P103 board. */

/* Initialize the LED (the board only has one). */
void init_led(void);

/* Initialize the button (the board only has one). */
void init_button(void);

/* Configures the RS232 serial port using the following settings:
 *   9600 Baud
 *   8 bits + 1 stop bit
 *   No parity bit
 *   No hardware flow control
 * Note that the USART2 is not enabled in this routine.  It is left disabled in
 * case any additional configuration is needed.
 */
void init_rs232(void);

void enable_rs232_interrupts(void);

void enable_rs232(void);

/* Functions for sending numbers through the UART */
void send_byte(uint8_t b);
void send_number(unsigned long sample, int radix);

// Missing prototype declarations
void enable_button_interrupts(void);
void rs232_print_str(const char *str); 
char hex_to_char(unsigned hex_number);

#endif /* __STM32_P103_H */
