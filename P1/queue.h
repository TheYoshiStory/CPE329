#ifndef QUEUE_H
#define QUEUE_H

#define MAX_SIZE 64

typedef struct
{
    char size;
    char *head;
    char arr[MAX_SIZE]
}
queue;

void clear(queue *q);
void add(queue *q, char data);
char remove(queue *q);

#endif
