#include "msp.h"
#include "delay.h"
#include "lcd.h"

// initialize LCD display
void init_lcd()
{
    // set lower 3 bits of LCD_CTRL as output
    LCD_CTRL->SEL1 &= ~(BIT2|BIT1|BIT0);
    LCD_CTRL->SEL0 &= ~(BIT2|BIT1|BIT0);
    LCD_CTRL->DIR |= BIT2|BIT1|BIT0;

    // set all bits of LCD_DATA as output
    LCD_DATA->SEL1 &= ~(BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);
    LCD_DATA->SEL0 &= ~(BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);
    LCD_DATA->DIR |= BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0;

    // "Function Set" command
    LCD_DATA->OUT = BIT5|BIT4|BIT3|BIT2;
    LCD_CTRL->OUT &= ~(BIT1|BIT0);
    LCD_CTRL->OUT |= BIT2;
    delay_us(1);
    LCD_CTRL->OUT &= ~BIT2;
    delay_us(50);
}

// clear LCD
void clear_lcd()
{
    // "Clear Display" command
    LCD_DATA->OUT = BIT0;
    LCD_CTRL->OUT &= ~(BIT1|BIT0);
    LCD_CTRL->OUT |= BIT2;
    delay_us(1);
    LCD_CTRL->OUT &= ~BIT2;
    delay_ms(2);

    // "Display ON/OFF Control" command
    LCD_DATA->OUT = BIT3|BIT2|BIT1|BIT0;
    LCD_CTRL->OUT &= ~(BIT1|BIT0);
    LCD_CTRL->OUT |= BIT2;
    delay_us(1);
    LCD_CTRL->OUT &= ~BIT2;
    delay_us(50);
}

// move cursor to top left of LCD
void home_lcd()
{
    // "Return Home" command
    LCD_DATA->OUT = BIT1;
    LCD_CTRL->OUT &= ~(BIT1|BIT0);
    LCD_CTRL->OUT |= BIT2;
    delay_us(1);
    LCD_CTRL->OUT &= ~BIT2;
    delay_ms(2);
}

// write a newline on the LCD
void write_nl_lcd()
{
    // "Set DDRAM Address" command
    LCD_DATA->OUT = BIT7|BIT6;
    LCD_CTRL->OUT &= ~(BIT1|BIT0);
    LCD_CTRL->OUT |= BIT2;
    delay_us(1);
    LCD_CTRL->OUT &= ~BIT2;
    delay_us(50);
}

// write a character on the LCD
void write_char_lcd(char c)
{
    if(c == '\n')
    {
        write_nl_lcd();
    }
    else
    {
        // "Write Data to Address" command
        LCD_DATA->OUT = c;
        LCD_CTRL->OUT &= ~BIT1;
        LCD_CTRL->OUT |= BIT2|BIT0;
        delay_us(1);
        LCD_CTRL->OUT &= ~BIT2;
        delay_us(50);
    }
}

// write a string on the LCD
void write_string_lcd(char *s)
{
    int i = 0;

    while(s[i] != '\0')
    {
        write_char_lcd(s[i]);
        i++;
    }
}

// turns off cursor to make LCD static
void hold_lcd()
{
    // "Display ON/OFF Control" command
    LCD_DATA->OUT = BIT3|BIT2;
    LCD_CTRL->OUT &= ~(BIT1|BIT0);
    LCD_CTRL->OUT |= BIT2;
    delay_us(1);
    LCD_CTRL->OUT &= ~BIT2;
    delay_us(50);
}

// toggles LCD on and off after holding display
void toggle_lcd()
{
    // "Display ON/OFF Control" command
    LCD_DATA->OUT ^= BIT2;
    LCD_CTRL->OUT &= ~(BIT1|BIT0);
    LCD_CTRL->OUT |= BIT2;
    delay_us(1);
    LCD_CTRL->OUT &= ~BIT2;
    delay_us(50);
}
