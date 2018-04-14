#include "msp.h"
#include "keypad.h"
#include "queue.h"
#include "lcd.h"

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

void process_queue()
{
    char input = queue_pop();
    char s[2];
    s[1] = '\0';
    while (input != 0)
    {
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
        }
        input = queue_pop();
    }

}

void probe_keypad()
{
    KEYPAD_CTRL->OUT |= BIT4;

    if(KEYPAD_CTRL->IN & BIT0)
    {
        queue_push('1');
    }
    else if(KEYPAD_CTRL->IN & BIT1)
    {
        queue_push('2');
    }
    else if(KEYPAD_CTRL->IN & BIT2)
    {
        queue_push('3');
    }

    KEYPAD_CTRL->OUT &= ~(BIT4);
    KEYPAD_CTRL->OUT |= BIT5;

    if(KEYPAD_CTRL->IN & BIT0)
    {
        queue_push('4');
    }
    else if(KEYPAD_CTRL->IN & BIT1)
    {
        queue_push('5');
    }
    else if(KEYPAD_CTRL->IN & BIT2)
    {
        queue_push('6');
    }

    KEYPAD_CTRL->OUT &= ~(BIT5);
    KEYPAD_CTRL->OUT |= BIT6;

    if(KEYPAD_CTRL->IN & BIT0)
    {
        queue_push('7');
    }
    else if(KEYPAD_CTRL->IN & BIT1)
    {
        queue_push('8');
    }
    else if(KEYPAD_CTRL->IN & BIT2)
    {
        queue_push('9');
    }

    KEYPAD_CTRL->OUT &= ~(BIT6);
    KEYPAD_CTRL->OUT |= BIT7;

    if(KEYPAD_CTRL->IN & BIT0)
    {
        queue_push('*');
    }
    else if(KEYPAD_CTRL->IN & BIT1)
    {
        queue_push('0');
    }
    else if(KEYPAD_CTRL->IN & BIT2)
    {
        queue_push('#');
    }

    KEYPAD_CTRL->OUT &= ~(BIT7);
}

// scans keypad for user input
char scan_keypad()
{
    char input = 0;

    KEYPAD_CTRL->OUT |= BIT4;

    if(KEYPAD_CTRL->IN & BIT0)
    {
        input = '1';
    }
    else if(KEYPAD_CTRL->IN & BIT1)
    {
        input = '2';
    }
    else if(KEYPAD_CTRL->IN & BIT2)
    {
        input = '3';
    }

    KEYPAD_CTRL->OUT &= ~(BIT4);
    KEYPAD_CTRL->OUT |= BIT5;

    if(KEYPAD_CTRL->IN & BIT0)
    {
        input = '4';
    }
    else if(KEYPAD_CTRL->IN & BIT1)
    {
        input = '5';
    }
    else if(KEYPAD_CTRL->IN & BIT2)
    {
        input = '6';
    }

    KEYPAD_CTRL->OUT &= ~(BIT5);
    KEYPAD_CTRL->OUT |= BIT6;

    if(KEYPAD_CTRL->IN & BIT0)
    {
        input = '7';
    }
    else if(KEYPAD_CTRL->IN & BIT1)
    {
        input = '8';
    }
    else if(KEYPAD_CTRL->IN & BIT2)
    {
        input = '9';
    }

    KEYPAD_CTRL->OUT &= ~(BIT6);
    KEYPAD_CTRL->OUT |= BIT7;

    if(KEYPAD_CTRL->IN & BIT0)
    {
        input = '*';
    }
    else if(KEYPAD_CTRL->IN & BIT1)
    {
        input = '0';
    }
    else if(KEYPAD_CTRL->IN & BIT2)
    {
        input = '#';
    }

    KEYPAD_CTRL->OUT &= ~(BIT7);
    return(input);
}
