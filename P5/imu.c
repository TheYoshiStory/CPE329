#include "msp.h"
#include "delay.h"
#include "imu.h"

// initialize IMU module
void init_imu(volatile sensor *s)
{
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

    // configure TimerA for 1ms interval timing
    TIMER_A1->CTL |= TIMER_A_CTL_TASSEL_2 | TIMER_A_CTL_ID__4 | TIMER_A_CTL_MC__STOP |TIMER_A_CTL_CLR | TIMER_A_CTL_IE;
    TIMER_A1->CCTL[0] = TIMER_A_CCTLN_CCIE;
    TIMER_A1->CCR[0] = CLK_FREQ / 4 / IMU_RATE;
    TIMER_A1->CTL |= TIMER_A_CTL_MC__STOP;

    // enable I2C and TimerA interrupts
    NVIC->ISER[0] = 1 << ((EUSCIB0_IRQn) & 31);
    NVIC->ISER[0] = 1 << ((TA1_0_IRQn) & 31);
}

// write a byte to the specified register
void write_imu(unsigned char reg, unsigned char data, volatile sensor *s)
{
    // set transmit mode and send START condition
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TR;
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTT;
    while(!s->i2c_flag);
    s->i2c_flag = 0;

    // send register address
    EUSCI_B0->TXBUF = reg;
    while(!s->i2c_flag);
    s->i2c_flag = 0;

    // send register data
    EUSCI_B0->TXBUF = data;
    while(!s->i2c_flag);
    s->i2c_flag = 0;

    // send STOP condition
    EUSCI_B0 -> CTLW0 |= EUSCI_B_CTLW0_TXSTP;
}

// read a byte from the specified register
unsigned char read_imu(unsigned char reg, volatile sensor *s)
{
    // set transmit mode and send START condition
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TR;
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTT;
    while(!s->i2c_flag);
    s->i2c_flag = 0;

    // send register address
    EUSCI_B0->TXBUF = reg;
    while(!s->i2c_flag);
    s->i2c_flag = 0;

    // set receive mode and send RESTART condition
    EUSCI_B0->CTLW0 &= ~EUSCI_B_CTLW0_TR;
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTT;
    while((EUSCI_B0->CTLW0 & EUSCI_B_CTLW0_TXSTT));

    // wait to receive data
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTP;
    while(!s->i2c_flag);
    s->i2c_flag = 0;

    return(EUSCI_B0->RXBUF & 0xFF);
}

// set flag for I2C byte transmit and receive
void i2c_imu(volatile sensor *s)
{
    // check if byte was successfully received
    if(EUSCI_B0->IFG & EUSCI_B_IFG_RXIFG0)
    {
        s->i2c_flag = 1;
        EUSCI_B0->IFG &= ~EUSCI_B_IFG_RXIFG0;
    }

    // check if byte was successfully transmitted
    if(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG0)
    {
        s->i2c_flag = 1;
        EUSCI_B0->IFG &= ~EUSCI_B_IFG_TXIFG0;
    }
}

// run main program at IMU_RATE
void sample_imu(volatile sensor *s)
{
    // set sample flag and clear interrupt flag
    s->sample_flag = 1;
    TIMER_A1->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
}
