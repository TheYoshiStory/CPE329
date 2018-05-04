#ifndef SIGNAL_H
#define SIGNAL_H

#define VDD 3300
#define GND 0
#define SAMPLES 1000

typedef struct
{
    unsigned char type;
    unsigned short frequency;
    unsigned char duty_cycle;
    unsigned short amplitude[SAMPLES];
    unsigned short state;
}
signal;

void process_signal(volatile signal *s);

#endif
