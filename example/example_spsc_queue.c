#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "spsc_queue.h"

struct user_node {
    int user_data;
    struct spscq_node node;
};


int main() 
{
    struct spsc_queue q;
    struct user_node *unode, *ufree;
    struct spscq_node  *pnode, *pfree;
    struct user_node* dummy_node = 
        (struct user_node*)malloc(sizeof(struct user_node));
    spsc_queue_init(&q, &dummy_node->node); 

    printf("qlen=%ld\n", spsc_queue_len(&q));
    pnode = spsc_dequeue(&q, &pfree);
    printf("pnode=%p, pfree=%p\n", pnode, pfree);
    if (pnode) {
        unode = container_of(pnode, struct user_node, node);
        printf("dequeue %d\n", unode->user_data);
    }
    if (pfree) {
        ufree = container_of(pfree, struct user_node, node);
        free(ufree);
    }

    unode = (struct user_node*)malloc(sizeof(struct user_node));
    unode->user_data = 1234;
    spsc_enqueue(&q, &unode->node);

    printf("qlen=%ld\n", spsc_queue_len(&q));
    pnode = spsc_dequeue(&q, &pfree);
    printf("pnode=%p, pfree=%p\n", pnode, pfree);
    if (pnode) {
        unode = container_of(pnode, struct user_node, node);
        printf("dequeue %d\n", unode->user_data);
    }
    if (pfree) {
        ufree = container_of(pfree, struct user_node, node);
        free(ufree);
    }

    spsc_queue_destroy(&q, free, struct user_node, node);
    return 0;
}
