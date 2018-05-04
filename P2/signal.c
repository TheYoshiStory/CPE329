#include <math.h>
#include "signal.h"

// calculate amplitude values for a single period and store them in a table
void build_signal(volatile signal *s)
{
    int i;

    for(i = 0; i < SAMPLES; i++)
    {
        if(s->type == SQUARE)
        {
            if(i < (SAMPLES * s->duty_cycle / 100))
            {
                s->amplitude[i] = VDD;
            }
            else
            {
                s->amplitude[i] = GND;
            }
        }
        else if(s->type == SINE)
        {
            s->amplitude[i] = SINE_AMPLITUDE * sin(2 * M_PI * i / SAMPLES) + SINE_OFFSET;
        }
        else if(s->type == RAMP)
        {
            s->amplitude[i] = RAMP_SLOPE * i / SAMPLES;
        }
    }

    s->state = 0;
}
