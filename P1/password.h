#ifndef PASSWORD_H
#define PASSWORD_H

#define PASSWORD_SIZE 4
#define PASSWORD_KEY 1234

typedef struct
{
    uint16_t input;
    uint8_t count;
    uint8_t valid;
}
password;

void update_password(password *pwd, char c);
int check_password(password *pwd);

#endif
