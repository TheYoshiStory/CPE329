#ifndef UART_H
#define UART_H

#define BAUD_RATE 115200

void init_uart();
void tx_uart(unsigned char data);
unsigned char rx_uart();

#endif
