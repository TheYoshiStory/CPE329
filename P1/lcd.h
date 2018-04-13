#ifndef LCD_H
#define LCD_H

/*
 *  --------------------------------------------
 *  | E | R/W | RS | - | DB7 | DB6 | DB5 | DB4 |
 *  --------------------------------------------
 */
#define LCD_CTRL P4

void init_lcd();
void clear_lcd();
void home_lcd();
void write_lcd(char *s);
void hold_lcd();
void shift_lcd(char r);

#endif
