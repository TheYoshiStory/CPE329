#include "delay.h"

// delay by milliseconds
void delay_ms(int ms, int clk)
{
    int i;

    for(i=0; i<ms; i++)
    {
        __delay_cycles(DELAY_MS);
    }
}


// delay by microseconds
void delay_us(int us, int clk)
{
    int i;

    for(i=0; i<us; i++)
    {
        __delay_cycles(DELAY_US);
    }
}
