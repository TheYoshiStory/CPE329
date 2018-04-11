#include "msp.h"
#include "delay.h"
#include "led.h"
#include "lcd.h"

// initialization
void init()
{
    // stop watchdog timer
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;

    // initialize components
    init_dco();
    init_led();
    init_lcd();
}

// main program
void main()
{
    init();

    clear_lcd();
    //write_string_lcd("Ayusman Saha\nCal Poly CPE");
    //hold_lcd();

    while(1);
}
