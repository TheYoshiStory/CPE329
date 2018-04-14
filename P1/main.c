#include "msp.h"
#include "delay.h"
#include "led.h"
#include "lcd.h"
#include "keypad.h"

// initialization
void init()
{
    // stop watchdog timer
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;

    // initialize components
    init_dco();
    init_led();
    init_lcd();
    init_keypad();
}

// main program
void main()
{
    init();
    clear_lcd();
    blue_led();

    while(1)
    {
        probe_keypad_bus();
        process_bus();
        delay_ms(175);
    }
}
