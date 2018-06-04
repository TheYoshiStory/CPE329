#ifndef BATTERY_H
#define BATTERY_H

/*
 *  --------------------------------------
 *  | - | - | - | - | - | - | SPKR | A13 |
 *  --------------------------------------
 */
#define BATTERY_CTRL P4

#define BATTERY_THRESHOLD 10500
#define BATTERY_DIVIDER 4
#define VDD 3300
#define SCALE 16383

void init_battery();

#endif
