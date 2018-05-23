#include "msp.h"
#include "delay.h"
#include "led.h"
#include "rc.h"
#include "esc.h"
#include "battery.h"

volatile channel ch[6];

void init()
{
    // disable watchdog timer
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;

    // initialize components
    init_led();
    init_dco();
    init_rc(ch);
    init_esc();
    init_battery();

    // enable all interrupts
    __enable_irq();
}

void writeMPU(uint8_t reg, uint8_t data)
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

uint8_t readMPU(uint8_t reg)
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


void main()
{
    init();

    P1->SEL0 |= BIT6 | BIT7;                // Set I2C pins of eUSCI_B0

    // Configure USCI_B0 for I2C mode
    EUSCI_B0->CTLW0 |= EUSCI_A_CTLW0_SWRST;   // Software reset enabled
    EUSCI_B0->CTLW0 = EUSCI_A_CTLW0_SWRST |   // Remain eUSCI in reset mode
            EUSCI_B_CTLW0_MODE_3 |            // I2C mode
            EUSCI_B_CTLW0_MST |               // Master mode
            EUSCI_B_CTLW0_SYNC |              // Sync mode
            EUSCI_B_CTLW0_SSEL__SMCLK;        // SMCLK

    EUSCI_B0->BRW = 240;                       // baudrate = SMCLK / 30 = 100kHz
    EUSCI_B0->I2CSA = 0x68;                    // Slave address
    EUSCI_B0->CTLW0 &= ~EUSCI_A_CTLW0_SWRST;  // Release eUSCI from reset

    writeMPU(0x6B, 0x01);
    delay_ms(5);
    P2->OUT = readMPU(0x6B) & 0x7;
    blue_led();

    while(1);

    while(1)
    {
        TIMER_A0->CCR[1] = ch[2].output;
        TIMER_A0->CCR[2] = ch[2].output;
        TIMER_A0->CCR[3] = ch[2].output;
        TIMER_A0->CCR[4] = ch[2].output;
    }
}

void PORT3_IRQHandler()
{
    process_rc(ch);
}

void ADC14_IRQHandler()
{
    read_battery();
}

void T32_INT2_IRQHandler()
{
    alert_battery();
}
