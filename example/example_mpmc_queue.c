#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpmc_queue.h"

struct user_node {
    int x;
};

void dump_mpmc_queue(struct mpmc_queue *q)
{
    struct mpmcq_node *qnode;
    struct user_node *unode; 
    if (!q)
        return;

    printf("\n----------dump_queue-----------\n");
    printf("| qlen=%ld\n", mpmc_queue_len(q));

    if (!q->head.ptr)
        return;
    qnode = q->head.ptr->next.ptr;
    while(qnode) {
        unode = qnode->user_data;
        printf("| qnode@%p, unode@%p, data=%d\n", qnode, unode, unode->x);
        qnode = qnode->next.ptr;
    }
    printf("---------/dump_queue-----------\n\n");
}

void enqueue_int(struct mpmc_queue *q, int n)
{
    struct user_node *unode;
    struct mpmcq_node  *pnode;
    unode = (struct user_node*)malloc(sizeof(struct user_node));
    unode->x = n;
    mpmc_enqueue(q, unode);
    printf("enqueue %d\n", n);
}
void dequeue_int(struct mpmc_queue *q) {
    struct user_node *unode;
    unode = mpmc_dequeue(q);
    //printf("unode=%p, pfree=%p\n", unode, pfree);
    if (unode) {
        printf("dequeue %d\n", unode->x);
        /*after using unode, free it*/
        free(unode);
    } else {
        printf("dequeue NULL\n");
    }
}

int main() 
{
    struct mpmc_queue q;
    struct user_node *unode;
    struct mpmcq_node  *pnode, *pfree;
    mpmc_queue_init(&q); 

    dump_mpmc_queue(&q);
    dequeue_int(&q);
    dump_mpmc_queue(&q);

    enqueue_int(&q, 1234);
    enqueue_int(&q, 5678);
    enqueue_int(&q, 3333);
    dump_mpmc_queue(&q);
    //return 0;
    dequeue_int(&q);
    dequeue_int(&q);
    dequeue_int(&q);
    dump_mpmc_queue(&q);
    dequeue_int(&q);
    mpmc_queue_destroy(&q);

    return 0;
}
