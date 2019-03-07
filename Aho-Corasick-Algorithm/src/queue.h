#ifndef _QUEUE
#define _QUEUE

struct queue_node
{
    int state;
    struct queue_node *next;
};

struct queue
{
    struct queue_node *head;
    struct queue_node *tail;
};

struct queue *init_queue(void);
void free_queue(struct queue *);
void enqueue(struct queue *, int);
int dequeue(struct queue *);
int is_empty_queue(struct queue *);

#endif
