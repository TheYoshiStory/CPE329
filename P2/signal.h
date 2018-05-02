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
    volatile unsigned int amplitude[SAMPLES];
    volatile unsigned short state;
}
signal;

void process_signal(signal *s);

#endif
