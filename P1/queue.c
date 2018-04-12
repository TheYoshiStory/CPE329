#include "msp.h"
#include "queue.h"

// clears all elements from queue
void clear(queue *q)
{
    q->size = 0;
    q->head = q->arr;
}

// adds element to end if queue is not full
void add(queue *q, char data)
{
    if(q->size < MAX_SIZE)
    {
        q->size++;

        if((q->head+q->size) == &(q->arr[MAX_SIZE]))
        {
            q->arr[0] = data;
        }
        else
        {
            *(q->head+q->size) = data;
        }
    }
}

// removes first element if queue is not empty
char remove(queue *q)
{
    char data;

    if(q->size > 0)
    {
        q->size--;
        data = *(q->head);

        if(q->head == &(q->arr[MAX_SIZE-1]))
        {
            q->head = q->arr;
        }
        else
        {
            q->head++;
        }

        return(data);
    }
    else
    {
        return(0);
    }
}
