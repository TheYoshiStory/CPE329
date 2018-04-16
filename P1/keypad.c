#include "msp.h"
#include "keypad.h"
#include "lcd.h"

uint16_t bus = 0;

// initialize keypad input
void init_keypad()
{
    // set KEYPAD_CTRL columns as inputs and rows as output
    KEYPAD_CTRL->SEL1 &= ~(BIT7|BIT6|BIT5|BIT4|BIT2|BIT1|BIT0);
    KEYPAD_CTRL->SEL0 &= ~(BIT7|BIT6|BIT5|BIT4|BIT2|BIT1|BIT0);
    KEYPAD_CTRL->DIR &= ~(BIT2|BIT1|BIT0);
    KEYPAD_CTRL->DIR |= BIT7|BIT6|BIT5|BIT4;
    KEYPAD_CTRL->OUT &= ~(BIT7|BIT6|BIT5|BIT4);
    KEYPAD_CTRL->REN |= BIT2|BIT1|BIT0;
    KEYPAD_CTRL->OUT &= ~(BIT2|BIT1|BIT0);
}

// probes keypad data bus
void probe_keypad()
{
    bus = 0;

    KEYPAD_CTRL->OUT |= BIT4;

    bus |= (KEYPAD_CTRL->IN & BIT0);
    bus |= (KEYPAD_CTRL->IN & BIT1);
    bus |= (KEYPAD_CTRL->IN & BIT2);

    KEYPAD_CTRL->OUT &= ~(BIT4);
    KEYPAD_CTRL->OUT |= BIT5;

    bus |= (KEYPAD_CTRL->IN & BIT0) << 3;
    bus |= (KEYPAD_CTRL->IN & BIT1) << 3;
    bus |= (KEYPAD_CTRL->IN & BIT2) << 3;

    KEYPAD_CTRL->OUT &= ~(BIT5);
    KEYPAD_CTRL->OUT |= BIT6;

    bus |= (KEYPAD_CTRL->IN & BIT0) << 6;
    bus |= (KEYPAD_CTRL->IN & BIT1) << 6;
    bus |= (KEYPAD_CTRL->IN & BIT2) << 6;

    KEYPAD_CTRL->OUT &= ~(BIT6);
    KEYPAD_CTRL->OUT |= BIT7;

    bus |= (KEYPAD_CTRL->IN & BIT0) << 9;
    bus |= (KEYPAD_CTRL->IN & BIT1) << 9;
    bus |= (KEYPAD_CTRL->IN & BIT2) << 9;

    KEYPAD_CTRL->OUT &= ~(BIT7);
}

// processes keypad data bus
void process_keypad()
{
    if(bus & BIT0)
    {
        write_char_lcd('1');
    }

    if(bus & BIT1)
    {
        write_char_lcd('2');
    }

    if(bus & BIT2)
    {
        write_char_lcd('3');
    }

    if(bus & BIT3)
    {
        write_char_lcd('4');
    }

    if(bus & BIT4)
    {
        write_char_lcd('5');
    }

    if(bus & BIT5)
    {
        write_char_lcd('6');
    }

    if(bus & BIT6)
    {
        write_char_lcd('7');
    }

    if(bus & BIT7)
    {
        write_char_lcd('8');
    }

    if(bus & BIT8)
    {
        write_char_lcd('9');
    }

    if(bus & BIT9)
    {
        write_char_lcd('\n');
    }

    if(bus & BITA)
    {
        write_char_lcd('0');
    }

    if(bus & BITB)
    {
        clear_lcd();
    }
}
