#ifndef ADC_H
#define ADC_H

/*
 *  -----------------------------------
 *  | - | - | - | - | - | - | A14 | - |
 *  -----------------------------------
 */
#define ADC_CTRL P6

#define VDD 3300
#define SCALE 16384

void init_adc();
short read_adc();

#endif
