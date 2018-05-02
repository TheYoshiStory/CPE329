#ifndef SIGNAL_H
#define SIGNAL_H

typedef struct
{
    unsigned char type;
    unsigned short frequency;
    unsigned char duty_cycle;
    unsigned short amplitude;
}
signal;

#endif
