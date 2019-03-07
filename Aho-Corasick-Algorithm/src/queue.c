#include "queue.h"

#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>

struct queue *init_queue()
{
    struct queue *q;

    q = (struct queue *)malloc(sizeof(*q));
    assert(q != NULL);

    q->head = q->tail = NULL;

    return q;
}

void enqueue(struct queue *q, int state)
{
    struct queue_node *newnode;

    assert(q != NULL);

    newnode = (struct queue_node *)malloc(sizeof *newnode);
    assert(NULL != newnode);
    newnode->state = state;
    newnode->next = NULL;

    // If queue is empty
    if (q->tail == NULL)
    {
        q->head = q->tail = newnode;
    }

    // If queue is not empty
    else
    {
        q->tail->next = newnode;
        q->tail = newnode;
    }
}

int is_empty_queue(struct queue *q)
{
    assert(q != NULL);

    return (q->head == NULL);
}

int dequeue(struct queue *q)
{
    int r;

    assert(q != NULL);

    // If queue is empty
    if (q->head == NULL)
    {
        fprintf(stderr, "Queue is empty. Nothing to dequeue\n");
        exit(2);
    }
    // If queue is not empty
    else
    {
        struct queue_node *temp;

        temp = q->head;
        r = q->head->state;
        q->head = q->head->next;
        if (q->head == NULL)
            q->tail = NULL;
        free(temp);
    }

    return r;
}

void free_queue(struct queue *q)
{
    assert(q != NULL);

    while (q->head != NULL)
    {
        struct queue_node *next = q->head->next;
        free(q->head);
        q->head = next;
    }

    free(q);
}
