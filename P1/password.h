#ifndef PASSWORD_H
#define PASSWORD_H

#define PASSWORD_SIZE 4

typedef struct
{
    char key[PASSWORD_SIZE];
    char input[PASSWORD_SIZE];
    char count;
}
password;

void update_password(password *pwd, char c);
int check_password(password *pwd);

#endif
