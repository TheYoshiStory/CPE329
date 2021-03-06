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
                s->amplitude[i] = VDD + DAC_OFFSET;
            }
            else
            {
                s->amplitude[i] = GND;
            }
        }
        else if(s->type == SINE)
        {
            s->amplitude[i] = ((VDD + DAC_OFFSET) / 2) * sin(2 * M_PI * i / SAMPLES) + ((VDD + DAC_OFFSET) / 2);
        }
        else if(s->type == RAMP)
        {
            s->amplitude[i] = (VDD + DAC_OFFSET) * i / SAMPLES;
        }
    }

    s->state = 0;
}
