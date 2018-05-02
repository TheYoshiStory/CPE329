#include <math.h>
#include "delay.h"
#include "signal.h"

void process_signal(signal *s, volatile unsigned short *t)
{
    if(s->type == 0)
    {
        if(s->state)
        {
            s->state = 0;
            s->amplitude = GND;
            *t += ((100 - s->duty_cycle) * CLK_FREQ) / (100 * s->frequency);
        }
        else
        {
            s->state = 1;
            s->amplitude = VDD;
            *t += (s->duty_cycle * CLK_FREQ) / (100 * s->frequency);
        }
    }
    else if(s->type == 1)
    {

    }
    else if(s->type == 2)
    {

    }
    else
    {
        s->amplitude = 0;
    }
}
