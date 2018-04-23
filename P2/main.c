#include "msp.h"

#define TIMER_CTRL P6
#define CLK_FREQ 24000000
#define DUTY_CYCLE 25
#define SIGNAL_FREQ 25000

// main program
void main()
{
    // enable MCLK on P4.3
    P4->SEL1 &= ~BIT3;
    P4->SEL0 |= BIT3;
    P4->DIR |= BIT3;

    // configure lower 2 bits as GPIO output
    TIMER_CTRL->SEL1 &= ~(BIT1|BIT0);
    TIMER_CTRL->SEL0 &= ~(BIT1|BIT0);
    TIMER_CTRL->DIR |= BIT1|BIT0;
    TIMER_CTRL->OUT &= ~(BIT1|BIT0);

    // set DCO to 24MHz and use as MCLK and SMCLK
    CS->KEY = CS_KEY_VAL;
    CS->CTL0 = 0;
    CS->CTL0 = CS_CTL0_DCORSEL_4;
    CS->CTL1 = CS_CTL1_SELA_2 | CS_CTL1_SELS_3 | CS_CTL1_SELM_3;
    CS->KEY = 0;

    // initialize timer A0
    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE;
    TIMER_A0->CCR[0] = (CLK_FREQ * DUTY_CYCLE) / (SIGNAL_FREQ * 100);
    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_MC__CONTINUOUS;
    SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;
    __enable_irq();
    NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31);

    while (1)
    {
        __sleep();
        __no_operation();
    }
}


// timer A0 interrupt service routine
void TA0_0_IRQHandler()
{
    TIMER_CTRL->OUT |= BIT1;
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;

    if(TIMER_CTRL->OUT & BIT0)
    {
        TIMER_CTRL->OUT &= ~BIT0;
        TIMER_A0->CCR[0] += (CLK_FREQ * (100 - DUTY_CYCLE)) / (SIGNAL_FREQ * 100);
    }
    else
    {
        TIMER_CTRL->OUT |= BIT0;
        TIMER_A0->CCR[0] += (CLK_FREQ * DUTY_CYCLE) / (SIGNAL_FREQ * 100);
    }

    TIMER_CTRL->OUT &= ~BIT1;
}
