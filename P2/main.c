#include "msp.h"

#define CLK_FREQ 3000000
#define SIGNAL_PERIOD 20
#define SIGNAL_AMPLITUDE 2000
#define SIGNAL_OFFSET 1000

int timer_counter;
int signal_state;
int signal_counter;

void Drive_DAC(unsigned int level)
{
  unsigned int DAC_Word = 0;
  int i;

  DAC_Word = (0x1000) | (level & 0x0FFF);   // 0x1000 sets DAC for Write
                                            // to DAC, Gain = 2, /SHDN = 1
                                            // and put 12-bit level value
                                            // in low 12 bits.

  P4->OUT &= ~BIT1;                                   // Clear P4.1 (drive /CS low on DAC)
                                                      // Using a port output to do this for now

  EUSCI_B0->TXBUF = (unsigned char) (DAC_Word >> 8);  // Shift upper byte of DAC_Word
                                                      // 8-bits to right

  while (!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG));              // USCI_A0 TX buffer ready?

  EUSCI_B0->TXBUF = (unsigned char) (DAC_Word & 0x00FF);     // Transmit lower byte to DAC

  while (!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG));      // Poll the TX flag to wait for completion

  for(i = 200; i > 0; i--);                          // Delay 200 16 MHz SMCLK periods
                                                     //to ensure TX is complete by SIMO

  P4->OUT |= BIT1;                                   // Set P4.1   (drive /CS high on DAC)

  return;
}

// main program
void main()
{
     // Stop watchdog timer
     WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;

    // set DCO to 3MHz and use as MCLK and SMCLK
    CS->KEY = CS_KEY_VAL;
    CS->CTL0 = 0;
    CS->CTL0 = CS_CTL0_DCORSEL_1;
    CS->CTL1 = CS_CTL1_SELA_2 | CS_CTL1_SELS_3 | CS_CTL1_SELM_3;
    CS->KEY = 0;

    // configure 4.1 as GPIO output
    P4->SEL1 &= ~BIT1;
    P4->SEL0 &= ~BIT1;
    P4->DIR |= BIT1;

    // configure 6.1 as GPIO output
    P6->SEL1 &= ~BIT1;
    P6->SEL0 &= ~BIT1;
    P6->DIR |= BIT1;

    // set P1.6 and P1.5 to UCB0SIMO and UCB0CLK
    P1->SEL1 &= ~(BIT6|BIT5);
    P1->SEL0 |= BIT6|BIT5;

    // SPI Setup
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_SWRST;
    EUSCI_B0->CTLW0 = EUSCI_B_CTLW0_SWRST | EUSCI_B_CTLW0_MST | EUSCI_B_CTLW0_SYNC | EUSCI_B_CTLW0_CKPL | EUSCI_B_CTLW0_MSB;
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_SSEL__SMCLK;
    EUSCI_B0->BRW = 0x01;
    EUSCI_B0->CTLW0 &= ~EUSCI_B_CTLW0_SWRST;
    EUSCI_B0->IFG |= EUSCI_B_IFG_TXIFG;

    // initialize timer A0
    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE;
    TIMER_A0->CCR[0] += CLK_FREQ / 1000;
    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_MC__CONTINUOUS;
    SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;
    __enable_irq();
    NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31);

    timer_counter = 0;
    signal_state = SIGNAL_OFFSET;
    signal_counter = SIGNAL_AMPLITUDE / SIGNAL_PERIOD;

    while(1);
}

// timer A0 interrupt service routine
void TA0_0_IRQHandler()
{
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
    TIMER_A0->CCR[0] += CLK_FREQ / 1000;
    timer_counter++;

    signal_state += signal_counter;

    if(timer_counter == (SIGNAL_PERIOD / 2))
    {
        timer_counter = 0;
        signal_counter *= -1;
        P6->OUT ^= BIT1;
    }

    Drive_DAC(signal_state);
}
