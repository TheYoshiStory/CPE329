#ifndef DELAY_H
#define DELAY_H

#define CLK_LVL_1  1500000
#define CLK_LVL_2  3000000
#define CLK_LVL_3  6000000
#define CLK_LVL_4 12000000
#define CLK_LVL_5 24000000
#define CLK_LVL_6 48000000

#define CLK_FREQ CLK_LVL_2
#define DELAY_US CLK_FREQ / 1700000
#define DELAY_MS CLK_FREQ / 1000

void init_dco();
void delay_us(int us);
void delay_ms(int ms);

#endif
