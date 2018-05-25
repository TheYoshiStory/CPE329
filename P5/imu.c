#include "msp.h"
#include "delay.h"
#include "imu.h"

void init_imu()
{
    P1->SEL0 |= BIT6 | BIT7;                // Set I2C pins of eUSCI_B0

    // Enable eUSCIB0 interrupt in NVIC module
    NVIC->ISER[0] = 1 << ((EUSCIB0_IRQn) & 31);

    // Configure USCI_B0 for I2C mode
    EUSCI_B0->CTLW0 |= EUSCI_A_CTLW0_SWRST;   // Software reset enabled
    EUSCI_B0->CTLW0 = EUSCI_A_CTLW0_SWRST |   // Remain eUSCI in reset mode
            EUSCI_B_CTLW0_MODE_3 |            // I2C mode
            EUSCI_B_CTLW0_MST |               // Master mode
            EUSCI_B_CTLW0_SYNC |              // Sync mode
            EUSCI_B_CTLW0_SSEL__SMCLK;        // SMCLK

    EUSCI_B0->BRW = CLK_FREQ / IMU_FREQ;
    EUSCI_B0->I2CSA = IMU_ADDR;          // Slave address
    EUSCI_B0->CTLW0 &= ~EUSCI_A_CTLW0_SWRST;  // Release eUSCI from reset

    EUSCI_B0->IE |= EUSCI_A_IE_RXIE |         // Enable receive interrupt
                    EUSCI_A_IE_TXIE;
}

// write a byte to the specified register
void write_imu(unsigned char reg, unsigned char data)
{
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TR;          // Set transmit mode (write)
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTT;       // I2C start condition

    while (!TransmitFlag);                // Wait for EEPROM address to transmit
    TransmitFlag = 0;

    EUSCI_B0 -> TXBUF = reg;    // Send the high byte of the memory address

    while (!TransmitFlag);            // Wait for the transmit to complete
    TransmitFlag = 0;

    EUSCI_B0 -> TXBUF = data;    // Send the high byte of the memory address

    while (!TransmitFlag);            // Wait for the transmit to complete
    TransmitFlag = 0;

    EUSCI_B0 -> CTLW0 |= EUSCI_B_CTLW0_TXSTP;   // I2C stop condition
}

// read a byte from the specified register
unsigned char read_imu(unsigned char reg)
{
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TR;      // Set transmit mode (write)
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTT;   // I2C start condition

    while (!TransmitFlag);                // Wait for EEPROM address to transmit
    TransmitFlag = 0;

    EUSCI_B0 -> TXBUF = reg;    // Send the high byte of the memory address

    while (!TransmitFlag);            // Wait for the transmit to complete
    TransmitFlag = 0;

    EUSCI_B0->CTLW0 &= ~EUSCI_B_CTLW0_TR;   // Set receive mode (read)
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTT; // I2C start condition (restart)

    // Wait for start to be transmitted
    while ((EUSCI_B0->CTLW0 & EUSCI_B_CTLW0_TXSTT));

    // set stop bit to trigger after first byte
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTP;

    while (!TransmitFlag);            // Wait to receive a byte
    TransmitFlag = 0;

    return(EUSCI_B0->RXBUF);
}
