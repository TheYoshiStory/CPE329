#include "msp.h"
#include "password.h"

// updates current input
void update_password(password *pwd, char c)
{
    int i;
    int val;
    int pow;

    val = c - 48;
    pow = PASSWORD_SIZE - pwd->count - 1;

    for(i=0; i<pow; i++)
    {
        val *= 10;
    }

    pwd->input += val;
    pwd->count++;
}

// checks current input with the key
int check_password(password *pwd)
{
    if((pwd->count == PASSWORD_SIZE) && (pwd->input == PASSWORD_KEY))
    {
        pwd->valid = 1;
    }

    return(pwd->valid);
}
