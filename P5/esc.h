#ifndef ESC_H
#define ESC_H

/*
 *  -------------------------------------
 *  | RB | LB | LF | RF | - | - | - | - |
 *  -------------------------------------
 */
#define ESC_CTRL P2

#define ESC_FREQ 50
#define ESC_MAX 6000
#define ESC_MIN 3000
#define ESC_IDLE 3200

#define RF 0
#define LF 1
#define LB 2
#define RB 3

void init_esc();

#endif
