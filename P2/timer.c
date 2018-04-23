#include "msp.h"
#include "timer.h"

// setup timer A0
void init_timer()
{
    TIMER_CTRL->DIR |= BIT1|BIT0;
    TIMER_CTRL->OUT &= ~(BIT1|BIT0);

   if(CLK_FREQ == CLK_LVL_1)
   {
       CS->KEY = CS_KEY_VAL;
       CS->CTL0 = 0;
       CS->CTL0 = CS_CTL0_DCORSEL_0;
       CS->CTL1 = CS_CTL1_SELA_2 | CS_CTL1_SELS_3 | CS_CTL1_SELM_3;
       CS->KEY = 0;
   }
   else if(CLK_FREQ == CLK_LVL_2)
   {
       CS->KEY = CS_KEY_VAL;
       CS->CTL0 = 0;
       CS->CTL0 = CS_CTL0_DCORSEL_1;
       CS->CTL1 = CS_CTL1_SELA_2 | CS_CTL1_SELS_3 | CS_CTL1_SELM_3;
       CS->KEY = 0;
   }
   else if(CLK_FREQ == CLK_LVL_3)
   {
       CS->KEY = CS_KEY_VAL;
       CS->CTL0 = 0;
       CS->CTL0 = CS_CTL0_DCORSEL_2;
       CS->CTL1 = CS_CTL1_SELA_2 | CS_CTL1_SELS_3 | CS_CTL1_SELM_3;
       CS->KEY = 0;
   }
   else if(CLK_FREQ == CLK_LVL_4)
   {
       CS->KEY = CS_KEY_VAL;
       CS->CTL0 = 0;
       CS->CTL0 = CS_CTL0_DCORSEL_3;
       CS->CTL1 = CS_CTL1_SELA_2 | CS_CTL1_SELS_3 | CS_CTL1_SELM_3;
       CS->KEY = 0;
   }
   else if(CLK_FREQ == CLK_LVL_5)
   {
       CS->KEY = CS_KEY_VAL;
       CS->CTL0 = 0;
       CS->CTL0 = CS_CTL0_DCORSEL_4;
       CS->CTL1 = CS_CTL1_SELA_2 | CS_CTL1_SELS_3 | CS_CTL1_SELM_3;
       CS->KEY = 0;
   }
   else if(CLK_FREQ == CLK_LVL_6)
   {
       while((PCM->CTL1 & PCM_CTL1_PMR_BUSY));
       PCM->CTL0 = PCM_CTL0_KEY_VAL | PCM_CTL0_AMR_1;
       while((PCM->CTL1 & PCM_CTL1_PMR_BUSY));

       FLCTL->BANK0_RDCTL = (FLCTL->BANK0_RDCTL & ~(FLCTL_BANK0_RDCTL_WAIT_MASK)) | FLCTL_BANK0_RDCTL_WAIT_1;
       FLCTL->BANK1_RDCTL = (FLCTL->BANK0_RDCTL & ~(FLCTL_BANK1_RDCTL_WAIT_MASK)) | FLCTL_BANK1_RDCTL_WAIT_1;

       CS->KEY = CS_KEY_VAL;
       CS->CTL0 = 0;
       CS->CTL0 = CS_CTL0_DCORSEL_5;
       CS->CTL1 = CS->CTL1 & ~(CS_CTL1_SELM_MASK | CS_CTL1_DIVM_MASK) | CS_CTL1_SELM_3;
       CS->KEY = 0;
   }

   TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE;
   TIMER_A0->CCR[0] = (CLK_FREQ / SIGNAL_FREQ) * (DUTY_CYCLE / 100);
   TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_MC__CONTINUOUS;
   SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;
   __enable_irq();
   NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31);
}


// set timer A0
void set_timer()
{
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;

    if(TIMER_CTRL->OUT & BIT0)
    {
        TIMER_CTRL->OUT &= ~BIT0;
        TIMER_A0->CCR[0] += (CLK_FREQ / SIGNAL_FREQ) * (DUTY_CYCLE / 100);;
    }
    else
    {
        TIMER_CTRL->OUT |= BIT0;
        TIMER_A0->CCR[0] += (CLK_FREQ / SIGNAL_FREQ) * ((100 - DUTY_CYCLE) / 100);;
    }
}
