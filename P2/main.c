#include "msp.h"
#include "delay.h"
#include "led.h"
#include "lcd.h"
#include "keypad.h"
#include "dac.h"
#include "signal.h"

volatile signal s;

// update signal specifications on the display
void update_display()
{
    clear_lcd();

    if(s.type == SQUARE)
    {
        write_string_lcd("SQUARE WAVE\n");
        write_int_lcd(s.frequency/10);
        write_string_lcd("0Hz @ ");
        write_int_lcd(s.duty_cycle);
        write_string_lcd("%");
    }
    else if(s.type == SINE)
    {
        write_string_lcd("SINE WAVE\n");
        write_int_lcd(s.frequency/10);
        write_string_lcd("0Hz");
    }
    else if(s.type == RAMP)
    {
        write_string_lcd("SAWTOOTH WAVE\n");
        write_int_lcd(s.frequency/10);
        write_string_lcd("0Hz");
    }

    hold_lcd();
}

// initialization
void init()
{
    // disable watchdog timer
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;

    // initialize components
    init_dco();
    init_led();
    init_lcd();
    init_keypad();
    init_dac();

    // initialize timer A0
    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE;
    TIMER_A0->CCR[0] = CLK_FREQ / SAMPLE_RATE;
    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_MC__CONTINUOUS;

    // enable interrupts
    __enable_irq();
    NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31);
}

// main program
void main()
{
    uint16_t data;

    // initialize system
    init();
    s.type = SQUARE;
    s.state = 0;
    s.frequency = 100;
    s.duty_cycle = 50;
    update_display();
    build_signal(&s);
    red_led();

    while(1)
    {
        // continuously output to the DAC and poll for keypad input
        output_dac(s.amplitude[s.state]);
        data = scan_keypad();

        // debounce if input is detected
        if(data)
        {
            delay_ms(100);
        }

        // change signal frequency to 100Hz
        if(data & BIT0)
        {
            s.frequency = 100;
            s.state = 0;
            update_display();
        }

        // change signal frequency to 200Hz
        if(data & BIT1)
        {
            s.frequency = 200;
            s.state = 0;
            update_display();
        }

        // change signal frequency to 300Hz
        if(data & BIT2)
        {
            s.frequency = 300;
            s.state = 0;
            update_display();
        }

        // change signal frequency to 400Hz
        if(data & BIT3)
        {
            s.frequency = 400;
            s.state = 0;
            update_display();
        }

        // change signal frequency to 500Hz
        if(data & BIT4)
        {
            s.frequency = 500;
            s.state = 0;
            update_display();
        }

        // change signal frequency to 600Hz
        if(data & BIT5)
        {
            s.frequency = 600;
            s.state = 0;
            update_display();
        }

        // change signal type to square
        if(data & BIT6)
        {
            s.type = SQUARE;
            s.duty_cycle = 50;
            reset_led();
            update_display();
            build_signal(&s);
            red_led();
        }

        // change signal type to sine
        if(data & BIT7)
        {
            s.type = SINE;
            reset_led();
            update_display();
            build_signal(&s);
            green_led();
        }

        // change signal type to ramp
        if(data & BIT8)
        {
            s.type = RAMP;
            reset_led();
            update_display();
            build_signal(&s);
            blue_led();
        }

        // decrease signal duty cycle by 10%
        if(data & BIT9)
        {
            if(s.type == SQUARE)
            {
                if(s.duty_cycle > DUTY_CYCLE_MIN)
                {
                    s.duty_cycle -= 10;
                    update_display();
                    build_signal(&s);
                }
            }
        }

        // reset signal duty cycle to 50%
        if(data & BITA)
        {
            if(s.type == SQUARE)
            {
                s.duty_cycle = 50;
                update_display();
                build_signal(&s);
            }
        }

        // increase signal duty cycle by 10%
        if(data & BITB)
        {
            if(s.type == SQUARE)
            {
                if(s.duty_cycle < DUTY_CYCLE_MAX)
                {
                    s.duty_cycle += 10;
                    update_display();
                    build_signal(&s);
                }
            }
        }
    }
}

// timer A0 interrupt service routine
void TA0_0_IRQHandler()
{
    // clear interrupt flag
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;

    // increment index based on frequency and modulo by table size
    s.state = (s.state + (s.frequency / 100)) % SAMPLES;

    // trigger interrupt every 10us
    TIMER_A0->CCR[0] += CLK_FREQ / SAMPLE_RATE;
}
