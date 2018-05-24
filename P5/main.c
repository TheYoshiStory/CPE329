#include "msp.h"
#include "delay.h"
#include "led.h"
#include "battery.h"
#include "imu.h"
#include "rc.h"
#include "esc.h"

volatile channel ch[6];

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

    while(1)
    {
        TIMER_A0->CCR[1] = ch[2].pulse;
        TIMER_A0->CCR[2] = ch[2].pulse;
        TIMER_A0->CCR[3] = ch[2].pulse;
        TIMER_A0->CCR[4] = ch[2].pulse;
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
