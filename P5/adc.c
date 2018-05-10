#include "msp.h"
#include "adc.h"

// initialize ADC14 module
void init_adc()
{
    ADC_CTRL->SEL0 |= BIT1;             //set to analog input
    ADC_CTRL->SEL1 |= BIT1;             //

    ADC14->CTL0 = ADC14_CTL0_SHT0_2 | ADC14_CTL0_SHP | ADC14_CTL0_ON;
    ADC14->CTL1 = ADC14_CTL1_RES_3;         // Use sampling timer, 12-bit conversion results

    ADC14->MCTL[0] |= ADC14_MCTLN_INCH_14;   // A14 ADC input select; Vref=AVCC
    ADC14->IER0 |= ADC14_IER0_IE0;          // Enable ADC conv complete interrupt

}

short read_adc_mv()
{
    int16_t data = ADC14->MEM[0];
    data = (data *3300) / 16384; //mV per bit times data;
    return data;
}
