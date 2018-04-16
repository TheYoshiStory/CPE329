#include "msp.h"
#include "keypad.h"

// initialize keypad input
void init_keypad()
{
    // set KEYPAD_CTRL columns as inputs and rows as output
    KEYPAD_CTRL->SEL1 &= ~(BIT7|BIT6|BIT5|BIT4|BIT2|BIT1|BIT0);
    KEYPAD_CTRL->SEL0 &= ~(BIT7|BIT6|BIT5|BIT4|BIT2|BIT1|BIT0);
    KEYPAD_CTRL->DIR &= ~(BIT2|BIT1|BIT0);
    KEYPAD_CTRL->DIR |= BIT7|BIT6|BIT5|BIT4;
    KEYPAD_CTRL->REN |= BIT2|BIT1|BIT0;
    KEYPAD_CTRL->OUT &= ~(BIT7|BIT6|BIT5|BIT4|BIT2|BIT1|BIT0);
}

// scans keypad for inputs
uint16_t scan_keypad()
{
    uint16_t data = 0;

    KEYPAD_CTRL->OUT |= BIT4;
    data |= KEYPAD_CTRL->IN & (BIT2|BIT1|BIT0);
    KEYPAD_CTRL->OUT &= ~(BIT4);

    KEYPAD_CTRL->OUT |= BIT5;
    data |= (KEYPAD_CTRL->IN & (BIT2|BIT1|BIT0)) << 3;
    KEYPAD_CTRL->OUT &= ~(BIT5);

    KEYPAD_CTRL->OUT |= BIT6;
    data |= (KEYPAD_CTRL->IN & (BIT2|BIT1|BIT0)) << 6;
    KEYPAD_CTRL->OUT &= ~(BIT6);

    KEYPAD_CTRL->OUT |= BIT7;
    data |= (KEYPAD_CTRL->IN & (BIT2|BIT1|BIT0)) << 9;
    KEYPAD_CTRL->OUT &= ~(BIT7);

    return(data);
}
