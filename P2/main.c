#include "msp.h"
#include "delay.h"
#include "led.h"
#include "lcd.h"
#include "keypad.h"
#include "dac.h"
#include "signal.h"

int timer_counter;
int signal_state;
int signal_counter;

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
    init();

    blue_led();
    clear_lcd();
    write_string_lcd("hello world!\n");

    // initialize timer A0
    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE;
    TIMER_A0->CCR[0] += CLK_FREQ / 1000;
    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_MC__CONTINUOUS;
    SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;
    __enable_irq();
    NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31);

    timer_counter = 0;
    signal_state = SIGNAL_OFFSET;
    signal_counter = SIGNAL_AMPLITUDE / SIGNAL_PERIOD;

    while(1);
}

// timer A0 interrupt service routine
void TA0_0_IRQHandler()
{
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
    TIMER_A0->CCR[0] += CLK_FREQ / 1000;
    timer_counter++;

    signal_state += signal_counter;

    if(timer_counter == (SIGNAL_PERIOD / 2))
    {
        timer_counter = 0;
        signal_counter *= -1;
        P6->OUT ^= BIT1;
    }

    output_dac(signal_state);
}
