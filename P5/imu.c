#include "msp.h"
#include "delay.h"
#include "imu.h"

void init_imu()
{
    // configure IMU_CTRL pins for SCL and SDA
    IMU_CTRL->SEL1 &= ~(BIT7|BIT6);
    IMU_CTRL->SEL0 |= BIT7|BIT6;

    // setup EUSCIB0 for I2C
    EUSCI_B0->CTLW0 |= EUSCI_A_CTLW0_SWRST;
    EUSCI_B0->CTLW0 = EUSCI_A_CTLW0_SWRST | EUSCI_B_CTLW0_MODE_3 | EUSCI_B_CTLW0_MST | EUSCI_B_CTLW0_SYNC | EUSCI_B_CTLW0_SSEL__SMCLK;
    EUSCI_B0->BRW = CLK_FREQ / IMU_FREQ;
    EUSCI_B0->I2CSA = IMU_ADDR;
    EUSCI_B0->CTLW0 &= ~EUSCI_A_CTLW0_SWRST;
}

// write a byte to the specified register
void write_imu(unsigned char reg, unsigned char data)
{
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TR;          // Set transmit mode (write)
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTT;       // I2C start condition

    while (!(EUSCI_B0->STATW & BIT4));            // Wait for slave address to transmit

    EUSCI_B0 ->TXBUF = reg;                       // Send the register number

    while (!(EUSCI_B0->STATW & BIT4));            // Wait for the transmit to complete

    EUSCI_B0 ->TXBUF = data;                      // Send the data

    while (!(EUSCI_B0->STATW & BIT4));            // Wait for the transmit to complete

    EUSCI_B0 ->CTLW0 |= EUSCI_B_CTLW0_TXSTP;      // I2C stop condition
}

// read a byte from the specified register
unsigned char read_imu(unsigned char reg)
{
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TR;      // Set transmit mode (write)
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTT;   // I2C start condition

    while (!(EUSCI_B0->STATW & BIT4));                // Wait for EEPROM address to transmit

    EUSCI_B0 -> TXBUF = reg;    // Send the register number

    while (!(EUSCI_B0->STATW & BIT4));            // Wait for the transmit to complete

    EUSCI_B0->CTLW0 &= ~EUSCI_B_CTLW0_TR;   // Set receive mode (read)
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTT; // I2C start condition (restart)

    // Wait for start to be transmitted
    while ((EUSCI_B0->CTLW0 & EUSCI_B_CTLW0_TXSTT));

    // set stop bit to trigger after first byte
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTP;

    while (!(EUSCI_B0->STATW & BIT4));            // Wait to receive a byte

    return EUSCI_B0->RXBUF;
}
