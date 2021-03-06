#include "msp.h"
#include "delay.h"
#include "imu.h"

// initialize IMU module
void init_imu()
{
    // wait for IMU to power up
    delay_ms(100);

    // setup SCL and SDA lines for I2C
    IMU_CTRL->SEL1 &= ~(BIT7|BIT6);
    IMU_CTRL->SEL0 |= BIT7|BIT6;

    // configure USCI_B0 for I2C
    EUSCI_B0->CTLW0 |= EUSCI_A_CTLW0_SWRST;
    EUSCI_B0->CTLW0 = EUSCI_B_CTLW0_MST | EUSCI_B_CTLW0_MODE_3 | EUSCI_B_CTLW0_SYNC | EUSCI_B_CTLW0_SSEL__SMCLK | EUSCI_A_CTLW0_SWRST;
    EUSCI_B0->BRW = CLK_FREQ / IMU_FREQ;
    EUSCI_B0->I2CSA = IMU_ADDR;
    EUSCI_B0->CTLW0 &= ~EUSCI_A_CTLW0_SWRST;
    EUSCI_B0->IE |= EUSCI_A_IE_TXIE | EUSCI_A_IE_RXIE;

    // enable I2C interrupt
    NVIC->ISER[0] = 1 << ((EUSCIB0_IRQn) & 31);
    NVIC->IP[9] |= (0x00 << NVIC_IPR9_PRI_36_OFS) & NVIC_IPR9_PRI_36_M;

    // configure TimerA for constant interval timing
    TIMER_A1->CTL |= TIMER_A_CTL_TASSEL_2 | TIMER_A_CTL_ID__4 | TIMER_A_CTL_MC__STOP | TIMER_A_CTL_CLR | TIMER_A_CTL_IE;
    TIMER_A1->CCTL[0] = TIMER_A_CCTLN_CCIE;
    TIMER_A1->CCR[0] = CLK_FREQ / 4 / IMU_RATE;

    // enable TimerA interrupt
    NVIC->ISER[0] = 1 << ((TA1_0_IRQn) & 31);
    NVIC->IP[6] |= (0x40 << NVIC_IPR6_PRI_26_OFS) & NVIC_IPR6_PRI_26_M;
}
