#ifndef KEYPAD_H
#define KEYPAD_H

/*
 *  ----------------------------------------
 *  | R4 | R6 | R7 | R2 | - | C5 | C1 | C3 |
 *  ----------------------------------------
 */
#define KEYPAD_CTRL P5

void init_keypad();
uint16_t scan_keypad();

#endif
