#include "msp.h"
#include "delay.h"
#include "led.h"
#include "lcd.h"
#include "uart.h"
#include "adc.h"

char flag = 0;
static int16_t data = 0;

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

    clear_lcd();
    blue_led();
    tx_uart(0x41);

    ADC14->CTL0 |= ADC14_CTL0_ENC | ADC14_CTL0_SC;

    while(1)
    {
        if (flag)
        {
            flag = 0;

            tx_uart((data/1000) + 48);
            data -= (data/1000) * 1000;
            tx_uart((data/100) + 48);
            data -= (data/100) * 100;
            tx_uart((data/10) + 48);
            data -= (data/10) * 10;
            tx_uart((data/1 + 48));
            data -= (data/1) * 1;

            tx_uart(0xD);

            ADC14->CTL0 |= ADC14_CTL0_ENC | ADC14_CTL0_SC;
        }
    }
}

void ADC14_IRQHandler()
{
    data = read_adc_mv();
    flag = 1;
}


