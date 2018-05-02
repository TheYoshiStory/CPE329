#include "msp.h"
#include "delay.h"
#include "led.h"
#include "lcd.h"
#include "keypad.h"
#include "dac.h"
#include "signal.h"

signal s;

void update_display()
{
    clear_lcd();

    if(s.type == 0)
    {
        write_string_lcd("SQUARE WAVE\n");
        write_int_lcd(s.frequency/10);
        write_string_lcd("0Hz @ ");
        write_int_lcd(s.duty_cycle);
        write_string_lcd("%");
    }
    else if(s.type == 1)
    {
        write_string_lcd("SINE WAVE\n");
        write_int_lcd(s.frequency/10);
        write_string_lcd("0Hz");
    }
    else if(s.type == 2)
    {
        write_string_lcd("SAWTOOTH WAVE\n");
        write_int_lcd(s.frequency/10);
        write_string_lcd("0Hz");
    }
    else
    {
        write_string_lcd("UNKNOWN\n");
        write_int_lcd(s.frequency/10);
        write_string_lcd("0Hz @ ");
        write_int_lcd(s.duty_cycle);
        write_string_lcd("%");
    }

    hold_lcd();
}

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
    init_dac();
}

// main program
void main()
{
    uint16_t data;

    init();
    blue_led();

    s.type = 0;
    s.frequency = 600;
    s.duty_cycle = 50;
    s.amplitude = 0;
    s.state = 0;
    update_display();

    // initialize timer A0
    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE;
    TIMER_A0->CCR[0] += ((100 - s.duty_cycle) * CLK_FREQ) / (100 * s.frequency);
    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_MC__CONTINUOUS;
    SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;
    __enable_irq();
    NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31);

    while(1)
    {
        delay_ms(1000);
        data = scan_keypad();

        if(data & BIT0)
        {
            s.frequency = 100;
            update_display();
        }

        if(data & BIT1)
        {
            s.frequency = 200;
            update_display();
        }

        if(data & BIT2)
        {
            s.frequency = 300;
            update_display();
        }

        if(data & BIT3)
        {
            s.frequency = 400;
            update_display();
        }

        if(data & BIT4)
        {
            s.frequency = 500;
            update_display();
        }

        if(data & BIT5)
        {
            s.frequency = 600;
            update_display();
        }

        if(data & BIT6)
        {
            s.type = 0;
            s.duty_cycle = 50;
            update_display();
        }

        if(data & BIT7)
        {
            s.type = 1;
            update_display();
        }

        if(data & BIT8)
        {
            s.type = 2;
            update_display();
        }

        if(data & BIT9)
        {
            if(s.duty_cycle > 10)
            {
                s.duty_cycle -= 10;
            }

            update_display();
        }

        if(data & BITA)
        {
            s.duty_cycle = 50;
            update_display();
        }

        if(data & BITB)
        {
            if(s.duty_cycle < 90)
            {
                s.duty_cycle += 10;
            }

            update_display();
        }
    }
}

// timer A0 interrupt service routine
void TA0_0_IRQHandler()
{
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
    process_signal(&s,&TIMER_A0->CCR[0]);
    output_dac(s.amplitude);
}
