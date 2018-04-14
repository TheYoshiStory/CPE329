#include "msp.h"
#include "queue.h"
#include "delay.h"

char num_pad_queue[QUEUE_SIZE];
char count;

void init_queue()
{
    count = 0;
}

void queue_push(char num)
{
    if (count < QUEUE_SIZE)
    {
        num_pad_queue[count] = num;
        ++count;
    }
}

char queue_pop()
{
    if (count > 0)
    {
        char num = num_pad_queue[0];
        --count;
        return num;
    }
    else
    {
        return 0;
    }
}

char get_count()
{
    return count;
}
