#include "msp.h"
#include "delay.h"
#include "lcd.h"

// send 8-bit data as two 4-bit packets
void send_data(char d)
{
    // send upper 4 bits
    LCD_CTRL->OUT &= ~(BIT3|BIT2|BIT1|BIT0);
    LCD_CTRL->OUT |= (d>>4) & (BIT3|BIT2|BIT1|BIT0);
    LCD_CTRL->OUT |= BIT7;
    delay_us(1);
    LCD_CTRL->OUT &= ~BIT7;

    // send lower 4 bits
    LCD_CTRL->OUT &= ~(BIT3|BIT2|BIT1|BIT0);
    LCD_CTRL->OUT |= d & (BIT3|BIT2|BIT1|BIT0);
    LCD_CTRL->OUT |= BIT7;
    delay_us(1);
    LCD_CTRL->OUT &= ~BIT7;
}

// initialize LCD display
void init_lcd()
{
    // set LCD_CTRL as output
    LCD_CTRL->SEL1 &= ~(BIT7|BIT6|BIT5|BIT3|BIT2|BIT1|BIT0);
    LCD_CTRL->SEL0 &= ~(BIT7|BIT6|BIT5|BIT3|BIT2|BIT1|BIT0);
    LCD_CTRL->DIR |= BIT7|BIT6|BIT5|BIT3|BIT2|BIT1|BIT0;

    // "Function Set" command
    LCD_CTRL->OUT &= ~(BIT6|BIT5);
    send_data(BIT5|BIT3|BIT2);
    delay_us(50);
}

// clear LCD
void clear_lcd()
{
    // "Clear Display" command
    LCD_CTRL->OUT &= ~(BIT6|BIT5);
    send_data(BIT0);
    delay_ms(2);

    // "Display ON/OFF Control" command
    LCD_CTRL->OUT &= ~(BIT6|BIT5);;
    send_data(BIT3|BIT2|BIT1|BIT0);
    delay_us(50);
}

// move cursor to top left of LCD
void home_lcd()
{
    // "Return Home" command
    LCD_CTRL->OUT &= ~(BIT6|BIT5);
    send_data(BIT1);
    delay_ms(2);
}

// write a character on the LCD
void write_char_lcd(char c)
{
    if(c == '\n')
    {
        // "Set DDRAM Address" command
        LCD_CTRL->OUT &= ~(BIT6|BIT5);
        send_data(BIT7|BIT6);
        delay_us(50);
    }
    else
    {
        // "Write Data to Address" command
        LCD_CTRL->OUT &= ~BIT6;
        LCD_CTRL->OUT |= BIT5;
        send_data(c);
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

// write an integer between -99 and 99 on the LCD
void write_int_lcd(int i)
{
    char j;

    if(i < 0)
    {
        write_char_lcd('-');
        i *= -1;
    }
    else
    {
        write_char_lcd('+');
    }

    j = i / 10;
    i -= j * 10;
    write_char_lcd(j+48);
    write_char_lcd(i+48);
}

// turns off cursor to make LCD static
void hold_lcd()
{
    // "Display ON/OFF Control" command
    LCD_CTRL->OUT &= ~(BIT6|BIT5);
    send_data(BIT3|BIT2);
    delay_us(50);
}

// shifts LCD right if input is nonzero or left if input is zero
void shift_lcd(char r)
{
    if(r)
    {
        // "Cursor or Display Shift" command
        LCD_CTRL->OUT &= ~(BIT6|BIT5);
        send_data(BIT4|BIT3|BIT2);
        delay_us(50);
    }
    else
    {
        // "Cursor or Display Shift" command
        LCD_CTRL->OUT &= ~(BIT6|BIT5);
        send_data(BIT4|BIT3);
        delay_us(50);
    }
}
