#include "msp.h"
#include "battery.h"

// initialize battery monitoring module
void init_battery()
{
    // configure BATTERY_CTRL for analog input
    BATTERY_CTRL->SEL1 |= BIT0;
    BATTERY_CTRL->SEL0 |= BIT0;

    // enable 14-bit precision conversion for A13
    ADC14->CTL0 = ADC14_CTL0_SHP | ADC14_CTL0_SSEL__MCLK | ADC14_CTL0_SHT0_2 | ADC14_CTL0_ON;
    ADC14->CTL1 = ADC14_CTL1_RES_3;
    ADC14->MCTL[0] |= ADC14_MCTLN_INCH_13;
    ADC14->IER0 |= ADC14_IER0_IE0;

    // enable ADC14 interrupt
    NVIC->ISER[0] = 1 << ((ADC14_IRQn) & 31);
    NVIC->IP[10] |= (0xC0 << NVIC_IPR10_PRI_40_OFS) & NVIC_IPR10_PRI_40_M;
}
