#include "msp.h"
#include "led.h"

// initialize RGB LED
void init_led()
{
    // set lower 3 bits of LED_CTRL as output
    LED_CTRL->SEL1 &= ~(BIT2|BIT1|BIT0);
    LED_CTRL->SEL0 &= ~(BIT2|BIT1|BIT0);
    LED_CTRL->DIR |= BIT2|BIT1|BIT0;
    LED_CTRL->OUT &= ~(BIT2|BIT1|BIT0);
}

// toggle red LED
void red_led()
{
    LED_CTRL->OUT ^= BIT0;
}

// toggle green LED
void green_led()
{
    LED_CTRL->OUT ^= BIT1;
}

// toggle blue LED
void blue_led()
{
    LED_CTRL->OUT ^= BIT2;
}
