#include "msp.h"
#include "rc.h"

// initialize RC receiver input
void init_rc()
{
    // configure RC_CTRL pins as GPIO input with interrupts
    RC_CTRL->SEL1 &= ~(BIT7|BIT6|BIT5|BIT3|BIT2|BIT0);
    RC_CTRL->SEL0 &= ~(BIT7|BIT6|BIT5|BIT3|BIT2|BIT0);
    RC_CTRL->DIR &= ~(BIT7|BIT6|BIT5|BIT3|BIT2|BIT0);
    RC_CTRL->IES &= ~(BIT7|BIT6|BIT5|BIT3|BIT2|BIT0);
    RC_CTRL->IE |= BIT7|BIT6|BIT5|BIT3|BIT2|BIT0;

    // enable Timer32 for pulse timing
    TIMER32_1->CONTROL |= TIMER32_CONTROL_ENABLE | TIMER32_CONTROL_SIZE | TIMER32_CONTROL_ONESHOT;
    TIMER32_1->CONTROL &= ~TIMER32_CONTROL_IE;

    // enable P3 interrupts
    NVIC->ISER[1] = 1 << ((PORT3_IRQn) & 31);
    NVIC->IP[13] |= (0x20 << NVIC_IPR13_PRI_53_OFS) & NVIC_IPR13_PRI_53_M;
}
