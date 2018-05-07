#ifndef SIGNAL_H
#define SIGNAL_H

#define VDD 3300
#define GND 0
#define DUTY_CYCLE_MAX 90
#define DUTY_CYCLE_MIN 10
#define SAMPLES 1000
#define SAMPLE_RATE 100000
#define DAC_OFFSET 760

enum TYPE{SQUARE, SINE, RAMP};

typedef struct
{
    unsigned char type;
    unsigned short frequency;
    unsigned char duty_cycle;
    unsigned short amplitude[SAMPLES];
    unsigned short state;
}
signal;

void build_signal(volatile signal *s);

#endif
