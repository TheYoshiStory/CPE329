#include "msp.h"
#include "timer.h"

// initialization
void init()
{
    // disable watching timer
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;

    // initialize components
    init_timer();
}

// main program
void main()
{
    init();

    while (1)
    {
        __sleep();
    }
}


// Timer A0 interrupt service routine
void TA0_0_IRQHandler()
{
    set_timer();
}
