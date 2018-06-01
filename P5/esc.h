#ifndef ESC_H
#define ESC_H

/*
 *  ---------------------------------------------
 *  | ESC4 | ESC3 | ESC2 | ESC1 | - | - | - | - |
 *  ---------------------------------------------
 */
#define ESC_CTRL P2

#define RF 0
#define LF 1
#define LB 2
#define RB 3

void init_esc();

#endif
