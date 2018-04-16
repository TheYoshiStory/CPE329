#include "password.h"

// updates current input
void update_password(password *pwd, char c)
{
    pwd->input[pwd->count] = c;
    pwd->count++;
}

// checks current input with the key
int check_password(password *pwd)
{
    int i;

    for(i=0; i<PASSWORD_SIZE; i++)
    {
        if(pwd->input[i] != pwd->key[i])
        {
            return(0);
        }
    }

    return(1);
}
