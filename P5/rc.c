#include "msp.h"
#include "rc.h"

// initialize RC receiver input
void init_rc(volatile channel *ch)
{
    // configure RC_CTRL pins as GPIO input with interrupts
    RC_CTRL->SEL1 &= ~(BIT7|BIT6|BIT5|BIT3|BIT2|BIT0);
    RC_CTRL->SEL0 &= ~(BIT7|BIT6|BIT5|BIT3|BIT2|BIT0);
    RC_CTRL->DIR &= ~(BIT7|BIT6|BIT5|BIT3|BIT2|BIT0);
    RC_CTRL->IES &= ~(BIT7|BIT6|BIT5|BIT3|BIT2|BIT0);
    RC_CTRL->IE |= BIT7|BIT6|BIT5|BIT3|BIT2|BIT0;

    // initialize channel states to low
    ch[0].state = 0;
    ch[1].state = 0;
    ch[2].state = 0;
    ch[3].state = 0;
    ch[4].state = 0;
    ch[5].state = 0;

    // enable Timer32 for pulse timing
    TIMER32_1->CONTROL |= TIMER32_CONTROL_ENABLE | TIMER32_CONTROL_SIZE | TIMER32_CONTROL_ONESHOT;
    TIMER32_1->CONTROL &= ~TIMER32_CONTROL_IE;

    // enable P3 interrupts
    NVIC->ISER[1] = 1 << ((PORT3_IRQn) & 31);
}

// time pulses from RC receiver input
void process_rc(volatile channel *ch)
{
    // channel 1
    if(RC_CTRL->IFG & BIT0)
    {
        if(!ch[0].state && (RC_CTRL->IN & BIT0))
        {
            TIMER32_1->LOAD = UINT32_MAX;
            ch[0].time = TIMER32_1->VALUE;
            ch[0].state = 1;
            RC_CTRL->IES |= BIT0;
        }
        else if(ch[0].state && !(RC_CTRL->IN & BIT0))
        {
            ch[0].pulse = (ch[0].time - TIMER32_1->VALUE);
            ch[0].state = 0;
            RC_CTRL->IES &= ~BIT0;
        }

        RC_CTRL->IFG &= ~BIT0;
    }

    // channel 2
    if(RC_CTRL->IFG & BIT2)
    {
        if(!ch[1].state && (RC_CTRL->IN & BIT2))
        {
            ch[1].time = TIMER32_1->VALUE;
            ch[1].state = 1;
            RC_CTRL->IES |= BIT2;
        }
        else if(ch[1].state && !(RC_CTRL->IN & BIT2))
        {
            ch[1].pulse = (ch[1].time - TIMER32_1->VALUE);
            ch[1].state = 0;
            RC_CTRL->IES &= ~BIT2;
        }

        RC_CTRL->IFG &= ~BIT2;
    }

    // channel 3
    if(RC_CTRL->IFG & BIT3)
    {
        if(!ch[2].state && (RC_CTRL->IN & BIT3))
        {
            ch[2].time = TIMER32_1->VALUE;
            ch[2].state = 1;
            RC_CTRL->IES |= BIT3;
        }
        else if(ch[2].state && !(RC_CTRL->IN & BIT3))
        {
            ch[2].pulse = (ch[2].time - TIMER32_1->VALUE);
            ch[2].state = 0;
            RC_CTRL->IES &= ~BIT3;
        }

        RC_CTRL->IFG &= ~BIT3;
    }

    // channel 4
    if(RC_CTRL->IFG & BIT5)
    {
        if(!ch[3].state && (RC_CTRL->IN & BIT5))
        {
            ch[3].time = TIMER32_1->VALUE;
            ch[3].state = 1;
            RC_CTRL->IES |= BIT5;
        }
        else if(ch[3].state && !(RC_CTRL->IN & BIT5))
        {
            ch[3].pulse = (ch[3].time - TIMER32_1->VALUE);
            ch[3].state = 0;
            RC_CTRL->IES &= ~BIT5;
        }

        RC_CTRL->IFG &= ~BIT5;
    }

    // channel 5
    if(RC_CTRL->IFG & BIT6)
    {
        if(!ch[4].state && (RC_CTRL->IN & BIT6))
        {
            ch[4].time = TIMER32_1->VALUE;
            ch[4].state = 1;
            RC_CTRL->IES |= BIT6;
        }
        else if(ch[4].state && !(RC_CTRL->IN & BIT6))
        {
            ch[4].pulse = (ch[4].time - TIMER32_1->VALUE);
            ch[4].state = 0;
            RC_CTRL->IES &= ~BIT6;
        }

        RC_CTRL->IFG &= ~BIT6;
    }

    // channel 6
    if(RC_CTRL->IFG & BIT7)
    {
        if(!ch[5].state && (RC_CTRL->IN & BIT7))
        {
            ch[5].time = TIMER32_1->VALUE;
            ch[5].state = 1;
            RC_CTRL->IES |= BIT7;
        }
        else if(ch[5].state && !(RC_CTRL->IN & BIT7))
        {
            ch[5].pulse = (ch[5].time - TIMER32_1->VALUE);
            ch[5].state = 0;
            RC_CTRL->IES &= ~BIT7;
        }

        RC_CTRL->IFG &= ~BIT7;
    }
}
