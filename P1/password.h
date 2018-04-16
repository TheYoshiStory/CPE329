#ifndef PASSWORD_H
#define PASSWORD_H

#include "msp.h"

#define PASSWORD_SIZE 4
#define PASSWORD_KEY 1234

typedef struct
{
    uint16_t input;
    uint8_t failures;
    char count;
}
password;

void update_password(password *pwd, char c);
int check_password(password *pwd);
int char_to_int(char c);
int exponential(int base, int power);
void lock_out(int multiplier);

#endif
