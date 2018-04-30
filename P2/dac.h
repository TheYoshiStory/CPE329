#ifndef DAC_H
#define DAC_H

/*
 *  ------------------------------------------------
 *  | /CS | UCB0SIMO | UCB0CLK | - | - | - | - | - |
 *  ------------------------------------------------
 */
#define DAC_CTRL P1

void init_dac();
void output_dac(unsigned int level);

#endif
