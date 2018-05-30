#include "msp.h"
#include "delay.h"
#include "uart.h"

// initialize UART registers and enable RX interrupts
void init_uart()
{
    // setup UCA0TXD and UCA0RXD
    UART_CTRL->SEL1 &= ~(BIT3|BIT2);
    UART_CTRL->SEL0 |= BIT3|BIT2;

    // configure UART settings
    EUSCI_A0->CTLW0 |= 1;
    EUSCI_A0->MCTLW = 0;
    EUSCI_A0->CTLW0 = 0x0081;
    EUSCI_A0->BRW = CLK_FREQ / BAUD_RATE;
    EUSCI_A0->CTLW0 &= ~1;
    EUSCI_A0->IE |= BIT0;

    // enable UART interrupts
    NVIC->ISER[0] = 1 << ((EUSCIA0_IRQn) & 31);
}

// transmit a byte over UART
void tx_uart(unsigned char data)
{
    while(!(EUSCI_A0->IFG & 0x02));
    EUSCI_A0->TXBUF = data;
}

// receive a byte over UART
unsigned char rx_uart()
{
    if(EUSCI_A0->IV & 0x02)
    {
        return(EUSCI_A0->RXBUF);
    }
    else
    {
        return(0);
    }
}

// clear UART terminal
void clc_uart()
{
    tx_uart(27);
    tx_uart(91);
    tx_uart(50);
    tx_uart(74);
    tx_uart(27);
    tx_uart(91);
    tx_uart(72);
}
