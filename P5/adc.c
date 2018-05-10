#include "msp.h"
#include "adc.h"

// initialize ADC14 module
void init_adc()
{
    // setup analog pin A14
    ADC_CTRL->SEL0 |= BIT1;
    ADC_CTRL->SEL1 |= BIT1;

    // use 14-bit precision and enable interrupts
    ADC14->CTL0 = ADC14_CTL0_SHT0_2 | ADC14_CTL0_SHP | ADC14_CTL0_ON;
    ADC14->CTL1 = ADC14_CTL1_RES_3;
    ADC14->MCTL[0] |= ADC14_MCTLN_INCH_14;
    ADC14->IER0 |= ADC14_IER0_IE0;
}

// read analog value from buffer and convert to mV
short read_adc()
{
    return(ADC14->MEM[0] * VDD / SCALE);
}
