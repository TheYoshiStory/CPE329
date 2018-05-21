#include "msp.h"
#include "delay.h"
#include "led.h"
#include "rc.h"
#include "esc.h"

volatile channel ch[6];

void main()
{
    // disable watchdog timer
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;

    // initialize components
    init_led();
    init_dco();
    //init_rc(ch);
    init_esc();

    /*
    // enable interrupts
    NVIC->ISER[1] = 1 << ((PORT3_IRQn) & 31);
    __enable_irq();
    */

    while(1);
}

/*
void PORT3_IRQHandler()
{
    process_rc(ch);
}
*/
