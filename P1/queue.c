#include "msp.h"
#include "queue.h"

char num_pad_queue[QUEUE_SIZE];
char count;

void queue_init()
{
    char i;

    for (i=0; i<QUEUE_SIZE; i++)
    {
        num_pad_queue[i] = 0;
    }

    count = 0;
}

void queue_push(char num)
{
    if(count < QUEUE_SIZE)
    {
        num_pad_queue[count] = num;
        ++count;
    }
}

char queue_pop()
{
    if(count > 0)
    {
        char num = num_pad_queue[0];
        --count;

        char i;
        for(i=0; i<QUEUE_SIZE-1; i++)
        {
            num_pad_queue[i] = num_pad_queue[i+1];
        }

        return num;
    }
    else
    {
        return 0;
    }
}
