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
    //init_rc(ch);
    init_esc();
    init_battery();

    // enable all interrupts
    __enable_irq();
}

void main()
{
    init();
    blue_led();

    while(1)
    {
        TIMER_A0->CCR[1] = 3000;
        TIMER_A0->CCR[2] = 3000;
        TIMER_A0->CCR[3] = 3000;
        TIMER_A0->CCR[4] = 3000;
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
