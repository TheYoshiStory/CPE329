#include "msp.h"
#include "delay.h"
#include "led.h"
#include "lcd.h"
#include "uart.h"
#include "adc.h"

static short data;
char flag;

// main program
int main(void)
{
    // disable watchdog timer
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;

    // initialize components
    init_dco();
    init_led();
    init_lcd();
    init_uart();
    init_adc();

    // enable interrupts
    __enable_irq();
    NVIC->ISER[0] = 1 << ((ADC14_IRQn) & 31);

    blue_led();
    ADC14->CTL0 |= ADC14_CTL0_ENC | ADC14_CTL0_SC;
    flag = 0;

    while(1)
    {
        if(flag)
        {
            // transmit thousands digit
            tx_uart(data /1000  + 48);
            data -= data / 1000 * 1000;

            // transmit hundreds digit
            tx_uart(data / 100 + 48);
            data -= data / 100 * 100;

            // transmit tens digit
            tx_uart(data / 10 + 48);
            data -= data / 10 * 10;

            // transmit ones digit
            tx_uart(data + 48);
            tx_uart(0xD);

            // start ADC sampling
            ADC14->CTL0 |= ADC14_CTL0_ENC | ADC14_CTL0_SC;
            flag = 0;
        }
    }
}

// ADC14 interrupt service routine
void ADC14_IRQHandler()
{
    data = read_adc();
    flag = 1;
}
