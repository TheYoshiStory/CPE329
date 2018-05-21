#include "msp.h"
#include "delay.h"
#include "led.h"
#include "rc.h"
#include "esc.h"
#include "adc.h"

volatile channel ch[6];

void main()
{
    // disable watchdog timer
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;

    // initialize components
    init_led();
    init_dco();
    //init_rc(ch);
    //init_esc();

    // enable interrupts
    //NVIC->ISER[1] = 1 << ((PORT3_IRQn) & 31);
    //__enable_irq();

    P4->SEL0 &= ~BIT1;
    P4->SEL1 &= ~BIT1;
    P4->DIR |= BIT1;
    P4->OUT &= ~BIT1;

    TIMER32_2->CONTROL |= TIMER32_CONTROL_ENABLE | TIMER32_CONTROL_SIZE | TIMER32_CONTROL_MODE; //Periodic mode 32-bit timer with interrupts
    TIMER32_2->CONTROL &= ~TIMER32_CONTROL_IE;
    TIMER32_2->LOAD = CLK_FREQ;

    // setup analog pin A13
    P4->SEL0 |= BIT0;
    P4->SEL1 |= BIT0;

    // use 14-bit precision and enable interrupts
    ADC14->CTL0 = ADC14_CTL0_SHT0_2 | ADC14_CTL0_SHP | ADC14_CTL0_ON;
    ADC14->CTL1 = ADC14_CTL1_RES_3;
    ADC14->MCTL[0] |= ADC14_MCTLN_INCH_13;
    ADC14->IER0 |= ADC14_IER0_IE0;

    ADC14->CTL0 |= ADC14_CTL0_ENC | ADC14_CTL0_SC;

    NVIC->ISER[0] = 1 << ((ADC14_IRQn) & 31);
    NVIC->ISER[0] = 1 << ((T32_INT2_IRQn) & 31);
    __enable_irq();

    blue_led();

    while(1);
}


void T32_INT2_IRQHandler()
{
    P4->OUT ^= BIT1;
    TIMER32_2->INTCLR &= 0;     //Clears interrupt flag
}

void ADC14_IRQHandler()
{
    short adc_val = ADC14->MEM[0] * VDD / SCALE;
    if (adc_val < 1000)
    {
        TIMER32_2->CONTROL |= TIMER32_CONTROL_IE;
    }
    else
    {
        TIMER32_2->CONTROL &= ~TIMER32_CONTROL_IE;

    }
    ADC14->CTL0 |= ADC14_CTL0_ENC | ADC14_CTL0_SC;
}

/*
void PORT3_IRQHandler()
{
    process_rc(ch);
}
*/
