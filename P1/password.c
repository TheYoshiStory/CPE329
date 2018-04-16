#include "password.h"
#include "delay.h"
#include "lcd.h"

// updates current input
void update_password(password *pwd, char c)
{
    int val = char_to_int(c);
    int power = PASSWORD_SIZE - pwd->count - 1;
    int multiple = exponential(10, power);
    val = val * multiple;
    pwd->input += val;
    pwd->count++;
}

// checks current input with the key
int check_password(password *pwd)
{
    if (pwd->input == PASSWORD_KEY)
    {
        return 1;
    }
    else
    {
        pwd->failures++;
        lock_out(pwd->failures);
        return 0;
    }
}

int exponential(int base, int power)
{
    int i;
    int val = 1;
    for (i = 0; i < power; i++)
    {
        val *= base;
    }
    return val;
}

int char_to_int(char c)
{
    int val = (int)(c - 48);
    return val;
}

void lock_out(int multiplier)
{
    int i = 0;

    clear_lcd();
    write_string_lcd("Timed out for\n");
    write_char_lcd((char)(multiplier+48));
    write_string_lcd(" minute(s)...");

    for (i = multiplier; i > 0; i--)
    {
        delay_ms(60000);
    }

    clear_lcd();
}
