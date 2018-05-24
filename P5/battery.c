#include "msp.h"
#include "delay.h"
#include "battery.h"

// initialize battery monitoring module
void init_battery()
{
    // configure BATTERY_CTRL for analog input and digital output
    BATTERY_CTRL->SEL1 &= BIT1;
    BATTERY_CTRL->SEL1 |= BIT0;
    BATTERY_CTRL->SEL0 &= BIT1;
    BATTERY_CTRL->SEL0 |= BIT0;
    BATTERY_CTRL->DIR |= BIT1;
    BATTERY_CTRL->OUT &= ~BIT1;

    // setup Timer32 for 1s interval timing
    TIMER32_2->CONTROL |= TIMER32_CONTROL_MODE | TIMER32_CONTROL_IE | TIMER32_CONTROL_MODE | TIMER32_CONTROL_SIZE;
    TIMER32_2->LOAD = CLK_FREQ;

    // enable 14-bit precision conversion for A13
    ADC14->CTL0 = ADC14_CTL0_SHP | ADC14_CTL0_SSEL__MCLK | ADC14_CTL0_SHT0_2 | ADC14_CTL0_ON;
    ADC14->CTL1 = ADC14_CTL1_RES_3;
    ADC14->MCTL[0] |= ADC14_MCTLN_INCH_13;
    ADC14->IER0 |= ADC14_IER0_IE0;
    ADC14->CTL0 |= ADC14_CTL0_ENC | ADC14_CTL0_SC;

    // enable ADC14 and Timer32 interrupts
    NVIC->ISER[0] = 1 << ((ADC14_IRQn) & 31);
    NVIC->ISER[0] = 1 << ((T32_INT2_IRQn) & 31);
}

// read battery voltage and enable timer if below BATTERY_THRESHOLD
void read_battery()
{
    if((ADC14->MEM[0] * BATTERY_DIVIDER * VDD / SCALE) < BATTERY_THRESHOLD)
    {
        // start Timer32
        TIMER32_2->CONTROL |= TIMER32_CONTROL_ENABLE;
    }
    else
    {
        // stop Timer32 and turn off buzzer
        TIMER32_2->CONTROL &= ~TIMER32_CONTROL_ENABLE;
        BATTERY_CTRL->OUT &= ~BIT1;
    }

    ADC14->CTL0 |= ADC14_CTL0_ENC | ADC14_CTL0_SC;
}

// sound buzzer to indicate low battery voltage
void alert_battery()
{
    // toggle buzzer and clear Timer32 interrupt
    BATTERY_CTRL->OUT ^= BIT1;
    TIMER32_2->INTCLR++;
}
