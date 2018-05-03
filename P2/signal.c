#include <math.h>
#include "signal.h"

void process_signal(signal *s)
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
            s->amplitude[i] = (VDD / 2) * sin(2 * M_PI * SAMPLES * i) + (VDD / 2);
        }
        else if(s->type == 2)
        {
            s->amplitude[i] = i * VDD / SAMPLES;
        }
    }
}
