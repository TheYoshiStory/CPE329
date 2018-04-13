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
    char input;
    char s[2];

    init();
    clear_lcd();
    s[1] = '\0';
    blue_led();

    while(1)
    {
        input = scan_keypad();

        if(input == '#')
        {
            clear_lcd();
        }
        else if(input == '*')
        {
            write_lcd("\n");
        }
        else if(input)
        {
            s[0] = input;
            write_lcd(s);
            delay_ms(200);
        }
    }
}
