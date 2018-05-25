#include "msp.h"
#include "delay.h"
#include "led.h"
#include "battery.h"
#include "imu.h"
#include "rc.h"
#include "esc.h"
#include <stdio.h>

volatile channel ch[6];
int TransmitFlag = 0;

// system initialization
void init()
{
    // disable watchdog timer
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;

    // initialize components
    init_dco();
    init_led();
    init_battery();
    init_imu();
    init_rc(ch);
    init_esc();

    // enable all interrupts
    __enable_irq();
}

// main program
void main()
{
    init();

    blue_led();

    write_imu(0x6B,0x01);
    delay_ms(5);
    P2->OUT |= read_imu(0x6B) & 0x07;

    while(1)
    {
        printf("Hello ");
        printf("World ");
        printf("%c", 0x1B);
        printf("%c", 0x5B);
        printf("%c", 0x32);
        printf("%c", 0x4A);

        TIMER_A0->CCR[1] = ch[2].pulse;
        TIMER_A0->CCR[2] = ch[2].pulse;
        TIMER_A0->CCR[3] = ch[2].pulse;
        TIMER_A0->CCR[4] = ch[2].pulse;
    }
}

void EUSCIB0_IRQHandler(void)
{
    if (EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG0)     // Check if transmit complete
    {
        EUSCI_B0->IFG &= ~ EUSCI_B_IFG_TXIFG0;  // Clear interrupt flag
        TransmitFlag = 1;                       // Set global flag
    }

    if (EUSCI_B0->IFG & EUSCI_B_IFG_RXIFG0)     // Check if receive complete
    {
        EUSCI_B0->IFG &= ~ EUSCI_B_IFG_RXIFG0;  // Clear interrupt flag
        TransmitFlag = 1;                       // Set global flag
    }
}

// P3 interrupt service routine
void PORT3_IRQHandler()
{
    process_rc(ch);
}

// ADC14 interrupt service routine
void ADC14_IRQHandler()
{
    read_battery();
}

// Timer32 interrupt service routine
void T32_INT2_IRQHandler()
{
    alert_battery();
}
