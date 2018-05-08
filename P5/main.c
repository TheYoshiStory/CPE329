#include "msp.h"
#include "delay.h"
#include "led.h"
#include "lcd.h"
#include "uart.h"

unsigned char data;

// main program
int main(void)
{
    // initialize components
    init_dco();
    init_led();
    init_lcd();
    init_uart();

    // enable interrupts
    __enable_irq();
    NVIC->ISER[0] = 1 << ((EUSCIA0_IRQn) & 31);

    clear_lcd();
    blue_led();

    while(1);
}

// UART interrupt service routine
void EUSCIA0_IRQHandler()
{
    data = rx_uart();

    // write input to LCD and console
    if(data)
    {
        if(data == 0x0D)
        {
            write_char_lcd('\n');
        }
        else if(data == 0x08)
        {
            clear_lcd();
        }
        else
        {
            write_char_lcd(data);
        }

        tx_uart(data);
    }
}
