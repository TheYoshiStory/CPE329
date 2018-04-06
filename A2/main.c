#include "msp.h"
#include "delay.h"

#define BLINK 1

// set DCO frequency
void set_DCO(int clk)
{
   if(clk <= 1500000)
   {
       CS->KEY = CS_KEY_VAL;
       CS->CTL0 = 0;
       CS->CTL0 = CS_CTL0_DCORSEL_0;
       CS->CTL1 = CS_CTL1_SELA_2 | CS_CTL1_SELS_2 | CS_CTL1_SELM_3;
       CS->KEY = 0;
   }
   else if(clk <= 3000000)
   {
       CS->KEY = CS_KEY_VAL;
       CS->CTL0 = 0;
       CS->CTL0 = CS_CTL0_DCORSEL_1;
       CS->CTL1 = CS_CTL1_SELA_2 | CS_CTL1_SELS_2 | CS_CTL1_SELM_3;
       CS->KEY = 0;
   }
   else if(clk <= 6000000)
   {
       CS->KEY = CS_KEY_VAL;
       CS->CTL0 = 0;
       CS->CTL0 = CS_CTL0_DCORSEL_2;
       CS->CTL1 = CS_CTL1_SELA_2 | CS_CTL1_SELS_2 | CS_CTL1_SELM_3;
       CS->KEY = 0;
   }
   else if(clk <= 12000000)
   {
       CS->KEY = CS_KEY_VAL;
       CS->CTL0 = 0;
       CS->CTL0 = CS_CTL0_DCORSEL_3;
       CS->CTL1 = CS_CTL1_SELA_2 | CS_CTL1_SELS_2 | CS_CTL1_SELM_3;
       CS->KEY = 0;
   }
   else if(clk <= 24000000)
   {
       CS->KEY = CS_KEY_VAL;
       CS->CTL0 = 0;
       CS->CTL0 = CS_CTL0_DCORSEL_4;
       CS->CTL1 = CS_CTL1_SELA_2 | CS_CTL1_SELS_2 | CS_CTL1_SELM_3;
       CS->KEY = 0;
   }
   else
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


// main program
int main(void)
{
    // set system clock frequency
    set_DCO(CLK_FREQ);

    // set P2.0, P2.1, and P2.2 as outputs
    P2->SEL1 &= ~(BIT2|BIT1|BIT0);
    P2->SEL0 &= ~(BIT2|BIT1|BIT0);
    P2->DIR |= BIT2|BIT1|BIT0;


    // blink LED every 1s
    while(BLINK)
    {
        P2->OUT |= BIT2|BIT1|BIT0;
        delay_ms(1000,CLK_FREQ);
        P2->OUT &= ~(BIT2|BIT1|BIT0);
        delay_ms(1000,CLK_FREQ);
    }

    // generate 100us pulses
    P2->OUT |= BIT2|BIT1|BIT0;
    delay_us(100,CLK_FREQ);
    P2->OUT &= ~(BIT2|BIT1|BIT0);
    delay_us(100,CLK_FREQ);
    P2->OUT |= BIT2|BIT1|BIT0;
    delay_us(100,CLK_FREQ);
}
