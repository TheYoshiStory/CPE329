#include "msp.h"
#include "dac.h"

// initialize external DAC
void init_dac()
{
    // setup DAC_CTRL MOSI, SCLK, and /CS
    DAC_CTRL->SEL1 &= ~(BIT7|BIT6|BIT5);
    DAC_CTRL->SEL0 |= BIT6|BIT5;
    DAC_CTRL->SEL0 &= ~BIT7;
    DAC_CTRL->DIR |= BIT7;
    DAC_CTRL->OUT |= BIT7;

    // setup SPI interface
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_SWRST;
    EUSCI_B0->CTLW0 = EUSCI_B_CTLW0_SWRST | EUSCI_B_CTLW0_MST | EUSCI_B_CTLW0_SYNC | EUSCI_B_CTLW0_CKPL | EUSCI_B_CTLW0_MSB;
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_SSEL__SMCLK;
    EUSCI_B0->BRW = 0x01;
    EUSCI_B0->CTLW0 &= ~EUSCI_B_CTLW0_SWRST;
    EUSCI_B0->IFG |= EUSCI_B_IFG_TXIFG;
}

// output value to DAC
void output_dac(unsigned int level)
{
  unsigned int DAC_Word = 0;
  int i;

  DAC_Word = (0x1000) | (level & 0x0FFF);   // 0x1000 sets DAC for Write
                                            // to DAC, Gain = 2, /SHDN = 1
                                            // and put 12-bit level value
                                            // in low 12 bits.

  DAC_CTRL->OUT &= ~BIT7;
                                                      // Using a port output to do this for now

  EUSCI_B0->TXBUF = (unsigned char) (DAC_Word >> 8);  // Shift upper byte of DAC_Word
                                                      // 8-bits to right

  while (!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG));              // USCI_A0 TX buffer ready?

  EUSCI_B0->TXBUF = (unsigned char) (DAC_Word & 0x00FF);     // Transmit lower byte to DAC

  while (!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG));      // Poll the TX flag to wait for completion

  for(i = 200; i > 0; i--);                          // Delay 200 16 MHz SMCLK periods
                                                     //to ensure TX is complete by SIMO

  DAC_CTRL->OUT |= BIT7;

}
