#ifndef DELAY_H_
#define DELAY_H_

#define CLK_FREQ 3000000
#define DELAY_MS CLK_FREQ / 1000
#define DELAY_US CLK_FREQ / 1700000

void delay_ms(int ms, int clk);
void delay_us(int us, int clk);

#endif
