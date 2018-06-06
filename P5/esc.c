#include "msp.h"
#include "delay.h"
#include "esc.h"

// initialize ESC PWM output pins
void init_esc()
{
    // configure ESC_CTRL as PWM outputs
    ESC_CTRL->SEL1 &= ~(BIT7|BIT6|BIT5|BIT4);
    ESC_CTRL->SEL0 |= BIT7|BIT6|BIT5|BIT4;
    ESC_CTRL->DIR |= BIT7|BIT6|BIT5|BIT4;

    // setup TimerA for reset-set output mode
    TIMER_A0->CTL |= TIMER_A_CTL_TASSEL_2 | TIMER_A_CTL_ID__8 | TIMER_A_CTL_MC__UP | TIMER_A_CTL_CLR;
    TIMER_A0->CCTL[1] |= TIMER_A_CCTLN_OUTMOD_7;
    TIMER_A0->CCTL[2] |= TIMER_A_CCTLN_OUTMOD_7;
    TIMER_A0->CCTL[3] |= TIMER_A_CCTLN_OUTMOD_7;
    TIMER_A0->CCTL[4] |= TIMER_A_CCTLN_OUTMOD_7;
    TIMER_A0->CCR[0] = CLK_FREQ / 8 / ESC_FREQ;
    TIMER_A0->CCR[RF+1] = ESC_MAX;
    TIMER_A0->CCR[LF+1] = ESC_MAX;
    TIMER_A0->CCR[LB+1] = ESC_MAX;
    TIMER_A0->CCR[RB+1] = ESC_MAX;
}
