#ifndef LED_H
#define LED_H

/*
 *  ---------------------------------
 *  | - | - | - | - | - | B | G | R |
 *  ---------------------------------
 */
#define LED_CTRL P2

void init_led();
void reset_led();
void red_led();
void green_led();
void blue_led();

#endif
