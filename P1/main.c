#include "msp.h"
#include "delay.h"
#include "led.h"
#include "lcd.h"
#include "keypad.h"
#include "password.h"

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
    password pwd;
    uint16_t data;
    uint8_t timer;

    init();
    clear_lcd();
    write_string_lcd("LOCKED\nENTER KEY: ");
    pwd.input = 0;
    pwd.count = 0;
    pwd.valid = 0;
    blue_led();


    while(1)
    {
        delay_ms(200);
        data = scan_keypad();

        if(data & BIT9)
        {
            reset_led();
            blue_led();
            clear_lcd();
            write_string_lcd("LOCKED\nENTER KEY: ");
            pwd.count = 0;
            pwd.input = 0;
            pwd.valid = 0;
        }

        if(!pwd.valid)
        {
            if(data & BITA)
            {
                write_char_lcd('0');
                update_password(&pwd,'0');
            }

            if(data & BIT0)
            {
                write_char_lcd('1');
                update_password(&pwd,'1');
            }

            if(data & BIT1)
            {
                write_char_lcd('2');
                update_password(&pwd,'2');
            }

            if(data & BIT2)
            {
                write_char_lcd('3');
                update_password(&pwd,'3');
            }

            if(data & BIT3)
            {
                write_char_lcd('4');
                update_password(&pwd,'4');
            }

            if(data & BIT4)
            {
                write_char_lcd('5');
                update_password(&pwd,'5');
            }

            if(data & BIT5)
            {
                write_char_lcd('6');
                update_password(&pwd,'6');
            }

            if(data & BIT6)
            {
                write_char_lcd('7');
                update_password(&pwd,'7');
            }

            if(data & BIT7)
            {
                write_char_lcd('8');
                update_password(&pwd,'8');
            }

            if(data & BIT8)
            {
                write_char_lcd('9');
                update_password(&pwd,'9');
            }

            if(data & BITB)
            {
                if(check_password(&pwd))
                {
                    blue_led();
                    green_led();
                    clear_lcd();
                    write_string_lcd("HELLO WORLD");
                    hold_lcd();
                }
                else
                {
                    blue_led();
                    red_led();
                    timer = 60;

                    while(timer > 0)
                    {
                        clear_lcd();
                        write_string_lcd("TIMED OUT FOR\n");
                        write_int_lcd(timer);
                        write_string_lcd(" SECOND(S)");
                        hold_lcd();
                        delay_ms(1000);
                        timer--;
                    }

                    clear_lcd();
                    write_string_lcd("LOCKED\nENTER KEY: ");
                    pwd.input = 0;
                    pwd.count = 0;
                    red_led();
                    blue_led();
                }
            }
        }
    }
}
