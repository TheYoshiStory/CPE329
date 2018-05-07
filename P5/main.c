#include "delay.h"
#include "dac.h"
#include "led.h"

#include "msp.h"

void UART0_init(void);

unsigned short flag = 0;
unsigned short data = 0;
unsigned char buffer[100];
unsigned char count = 0;

int main(void) {
    init_dco();
    init_dac();
    init_led();
    UART0_init();

    __enable_irq();
    NVIC->ISER[0] = 1 << ((EUSCIA0_IRQn) & 31);

    blue_led();

    while (1)
    {
        if (flag)
        {
            data = 0;
            data += buffer[0] * 1000;
            data += buffer[1] * 100;
            data += buffer[2] * 10;
            data += buffer[3] * 1;
            data += 760;

            output_dac(data);

            flag = 0;
            buffer[0] = 0;
            buffer[0] = 0;
            buffer[0] = 0;
            buffer[0] = 0;
            count = 0;
        }
    }
}

void UART0_init(void)
{
    EUSCI_A0->CTLW0 |= 1;       /* put in reset mode for config */
    EUSCI_A0->MCTLW = 0;        /* disable oversampling */
    EUSCI_A0->CTLW0 = 0x0081;   /* 1 stop bit, no parity, SMCLK, 8-bit data */
    EUSCI_A0->BRW = 26;         /* 3,000,000 / 115200 = 26 */
    P1->SEL0 |= 0x0C;           /* P1.3, P1.2 for UART */
    P1->SEL1 &= ~0x0C;
    EUSCI_A0->CTLW0 &= ~1;      /* take UART out of reset mode */
    EUSCI_A0->IE |= BIT0;       //Enable Receive interrupts
}

void EUSCIA0_IRQHandler()
{
    if (EUSCI_A0->IV & 0x02)
    {
        if (EUSCI_A0->RXBUF >= 0x30 && EUSCI_A0->RXBUF <= 0x39)
        {
            buffer[count] = EUSCI_A0->RXBUF - 48;
            count++;
        }
        else if (EUSCI_A0->RXBUF == 0x0D)
        {
            flag = 1;
        }
        while(!(EUSCI_A0->IFG & 0x02)) {}
        if (EUSCI_A0->RXBUF == 0x0D)
        {
            EUSCI_A0->TXBUF = 0x0A;
        }
        else
        {
            EUSCI_A0->TXBUF = EUSCI_A0->RXBUF;
        }
    }
}
