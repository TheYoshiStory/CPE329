#include "msp.h"
#include "delay.h"

// set DCO frequency
void init_dco(int clk)
{
   if(CLK_FREQ == CLK_LVL_1)
   {
       CS->KEY = CS_KEY_VAL;
       CS->CTL0 = 0;
       CS->CTL0 = CS_CTL0_DCORSEL_0;
       CS->CTL1 = CS_CTL1_SELA_2 | CS_CTL1_SELS_2 | CS_CTL1_SELM_3;
       CS->KEY = 0;
   }
   else if(CLK_FREQ == CLK_LVL_2)
   {
       CS->KEY = CS_KEY_VAL;
       CS->CTL0 = 0;
       CS->CTL0 = CS_CTL0_DCORSEL_1;
       CS->CTL1 = CS_CTL1_SELA_2 | CS_CTL1_SELS_2 | CS_CTL1_SELM_3;
       CS->KEY = 0;
   }
   else if(CLK_FREQ == CLK_LVL_3)
   {
       CS->KEY = CS_KEY_VAL;
       CS->CTL0 = 0;
       CS->CTL0 = CS_CTL0_DCORSEL_2;
       CS->CTL1 = CS_CTL1_SELA_2 | CS_CTL1_SELS_2 | CS_CTL1_SELM_3;
       CS->KEY = 0;
   }
   else if(CLK_FREQ == CLK_LVL_4)
   {
       CS->KEY = CS_KEY_VAL;
       CS->CTL0 = 0;
       CS->CTL0 = CS_CTL0_DCORSEL_3;
       CS->CTL1 = CS_CTL1_SELA_2 | CS_CTL1_SELS_2 | CS_CTL1_SELM_3;
       CS->KEY = 0;
   }
   else if(CLK_FREQ == CLK_LVL_5)
   {
       CS->KEY = CS_KEY_VAL;
       CS->CTL0 = 0;
       CS->CTL0 = CS_CTL0_DCORSEL_4;
       CS->CTL1 = CS_CTL1_SELA_2 | CS_CTL1_SELS_2 | CS_CTL1_SELM_3;
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
}

// delay by microseconds
void delay_us(int us)
{
    int i;

    for(i=0; i<us; i++)
    {
        __delay_cycles(DELAY_US);
    }
}

// delay by milliseconds
void delay_ms(int ms)
{
    int i;

    for(i=0; i<ms; i++)
    {
        __delay_cycles(DELAY_MS);
    }
}
