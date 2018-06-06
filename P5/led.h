#ifndef LED_H
#define LED_H

/*
 *  ---------------------------------
 *  | - | - | - | - | - | B | G | R |
 *  ---------------------------------
 */
#define LED_CTRL P2

#define LED_RED 0x01
#define LED_YELLOW 0x03
#define LED_GREEN 0x02
#define LED_CYAN 0x06
#define LED_BLUE 0x04
#define LED_MAGENTA 0x05
#define LED_WHITE 0x07

void init_led();

#endif
