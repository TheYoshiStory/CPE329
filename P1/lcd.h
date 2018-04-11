#ifndef LCD_H
#define LCD_H

#define LCD_CTRL P4

void init_lcd();
void clear_lcd();
void home_lcd();
void write_char_lcd(char c);
void write_string_lcd(char *s);
void hold_lcd();

#endif
