#ifndef SIGNAL_H
#define SIGNAL_H

#define VDD 3300
#define GND 0

typedef struct
{
    unsigned char type;
    unsigned short frequency;
    unsigned char duty_cycle;
    unsigned short amplitude;
    unsigned short state;
}
signal;

void process_signal(signal *s, volatile unsigned short *t);

#endif
