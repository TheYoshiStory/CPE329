#ifndef RC_H
#define RC_H

/*
 *  ---------------------------------------------
 *  | CH6 | CH5 | CH4 | - | CH3 | CH2 | - | CH1 |
 *  ---------------------------------------------
 */
#define RC_CTRL P3

/*
 *  ----------------------
 *  | CH1 - ROLL         |
 *  | CH2 - PITCH        |
 *  | CH3 - THROTTLE     |
 *  | CH4 - YAW          |
 *  | CH5 - RIGHT SWITCH |
 *  | CH6 - LEFT SWITCH  |
 *  ----------------------
 */
typedef struct
{
    unsigned char state;
    unsigned int time;
    unsigned short pulse;
}
channel;

void init_rc(volatile channel *ch);
void process_rc(volatile channel *ch);

#endif
