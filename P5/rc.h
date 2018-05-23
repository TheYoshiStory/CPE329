#ifndef RC_H
#define RC_H

/*
 *  ---------------------------------------------
 *  | CH6 | CH5 | CH4 | - | CH3 | CH2 | - | CH1 |
 *  ---------------------------------------------
 */
#define RC_CTRL P3

typedef struct
{
    unsigned char state;
    unsigned int time;
    unsigned int output;
}
channel;

void init_rc(volatile channel *ch);
void process_rc(volatile channel *ch);

#endif
