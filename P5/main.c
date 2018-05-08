#include "msp.h"
#include "delay.h"
#include "led.h"
#include "uart.h"

// main program
int main(void)
{
    // initialize components
    init_dco();
    init_led();
    init_uart();
    blue_led();

    // enable interrupts
    __enable_irq();
    NVIC->ISER[0] = 1 << ((EUSCIA0_IRQn) & 31);

    while(1);
}

// UART interrupt service routine
void EUSCIA0_IRQHandler()
{
    // echo input back to console
    tx_uart(rx_uart());
}
