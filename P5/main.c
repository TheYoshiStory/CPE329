#include "msp.h"
#include "delay.h"
#include "led.h"

#define CH1 P3

uint8_t state = 0;
uint32_t ch1_time = 0;

void main()
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;

    init_led();
    init_dco();

    CH1->SEL0 &= ~(BIT7|BIT5);
    CH1->SEL1 &= ~(BIT7|BIT5);
    CH1->DIR &= ~BIT5;
    CH1->DIR |= BIT7;

    TIMER32_1->CONTROL |= (BIT7|BIT1|BIT0);     //Enable Timer, 32-bit counter, Oneshot mode
    TIMER32_1->CONTROL &= ~BIT5;                //Interrupt Disabled

    CH1->IES &= ~BIT5;
    CH1->IE |= BIT5;

    __enable_irq();
    NVIC->ISER[1] = 1 << ((PORT3_IRQn) & 31);

    blue_led();

    while(1)
    {
        if (state == 1)
        {
            CH1->OUT |= BIT7;
            //__delay_cycles(ch1_time);
            CH1->OUT &= BIT7;
        }
    }
}

void PORT3_IRQHandler()
{
    green_led();

    if ((state == 0) && (CH1->IN & BIT5))
    {
        TIMER32_1->LOAD = 0xFFFFFFFF;               //UINT32_MAX
        state = 1;
        CH1->IES |= BIT5;
    }
    else if ((state == 1) && !(CH1->IN & BIT5))
    {
        state = 0;
        CH1->IES &= ~BIT5;
        ch1_time = 0xFFFFFFFF - TIMER32_1->VALUE;
    }
    CH1->IFG &= ~BIT5;
}
