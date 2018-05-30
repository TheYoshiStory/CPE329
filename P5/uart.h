#ifndef UART_H
#define UART_H

/*
 *  ---------------------------------------------
 *  | - | - | - | - | UCA0TXD | UCA0RXD | - | - |
 *  ---------------------------------------------
 */
#define UART_CTRL P1
#define BAUD_RATE 115200

void init_uart();
void tx_uart(unsigned char data);
unsigned char rx_uart();
void clear_terminal_uart();

#endif
