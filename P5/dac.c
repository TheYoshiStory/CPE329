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

// output a value to the DAC
void output_dac(unsigned short level)
{
  uint32_t data = 0;

  // setup data buffer for write command
  data = 0x3000 | (level & 0x0FFF);

  // drive /CS low
  DAC_CTRL->OUT &= ~BIT7;

  // transmit upper byte of the data buffer
  EUSCI_B0->TXBUF = (unsigned char)(data >> 8);
  while(!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG));

  // transmit lower byte of the data buffer
  EUSCI_B0->TXBUF = (unsigned char)(data & 0x00FF);
  while (!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG));

  // drive /CS high
  DAC_CTRL->OUT |= BIT7;
}
