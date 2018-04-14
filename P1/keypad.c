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

void process_bus()
{
    char s[2];
    s[1] = '\0';

    if (bus & BIT0)
    {
        s[0] = '1';
        write_lcd(s);
    }
    if (bus & BIT1)
    {
        s[0] = '2';
        write_lcd(s);
    }
    if (bus & BIT2)
    {
        s[0] = '3';
        write_lcd(s);
    }
    if (bus & BIT3)
    {
        s[0] = '4';
        write_lcd(s);
    }
    if (bus & BIT4)
    {
        s[0] = '5';
        write_lcd(s);
    }
    if (bus & BIT5)
    {
        s[0] = '6';
        write_lcd(s);
    }
    if (bus & BIT6)
    {
        s[0] = '7';
        write_lcd(s);
    }
    if (bus & BIT7)
    {
        s[0] = '8';
        write_lcd(s);
    }
    if (bus & BIT8)
    {
        s[0] = '9';
        write_lcd(s);
    }
    if (bus & BIT9)
    {
        write_lcd("\n");
    }
    if (bus & BITA)
    {
        s[0] = '0';
        write_lcd(s);
    }
    if (bus & BITB)
    {
        clear_lcd();
    }
}

void probe_keypad_bus()
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
