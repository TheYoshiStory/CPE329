#ifndef RC_H
#define RC_H

/*
 *  ---------------------------------------------
 *  | CH6 | CH5 | CH4 | - | CH3 | CH2 | - | CH1 |
 *  ---------------------------------------------
 */
#define RC_CTRL P3

#define RC_MAX 48000
#define RC_MID 36000
#define RC_MIN 24000

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
    int setpoint;
}
channel;

void init_rc();

#endif
