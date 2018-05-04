#include <math.h>
#include "signal.h"

void process_signal(volatile signal *s)
{
    int i;

    for(i = 0; i < SAMPLES; i++)
    {
        if(s->type == 0)
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
        else if(s->type == 1)
        {
            s->amplitude[i] = (1000 * sin(2.0 * M_PI * i / SAMPLES)) + (1000);
        }
        else if(s->type == 2)
        {
            s->amplitude[i] = i * VDD * 0.6 / SAMPLES;
        }
    }
}
