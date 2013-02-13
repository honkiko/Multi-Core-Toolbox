#ifndef _SPSC_QUEUE_H
#define _SPSC_QUEUE_H
/*
 * SPSC_QUEUE: Single Producer, Single Consumer FIFO queue.
 * 
 * What's special? 
 * It's a concurrent SPSC FIFO queue, means producer and 
 * consumer can simultaneously work on the queue.
 * 
 * If you have multiple producer and consumer, simplely use
 * a producer-exclusive lock and consumer-exclusive lock to
 * serialize producers and consumers. Or turn to MPSC_QUEUE
 * 
 * It's based on the article by Maged M. Michael and 
 * Michael L. Scott: "Simple, Fast, and Practical Non-Blocking 
 * and Blocking Concurrent Queue Algorithms", PODC96.

 *
 * Implemented and tested by:	
 *		Hong Zhiguo <honkiko@gmail.com>
 *      2012.01.18 
 *
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License
 *	as published by the Free Software Foundation; either version
 *	2 of the License, or (at your option) any later version.
 */
 
#include "mct_adapt.h"

/*
	Producer(enqueue)		Consumer(dequeue)

	node->next = NULL;		
                            dead_head = q->head;
                            new_head = dead_head->next;
	q->tail->next = node;		
	smp_wmb();
                            if (!new_head){
                                return NULL; }

                            q->head = new_head;
                            *node_to_free = dead_head;
                           
					return new_head;


	q->tail = node;

*/

/** each node has 2 state: active or zombie
 * */
struct spscq_node {
    struct spscq_node *next;
    //spsc_node_state_t state;
};

struct spsc_queue {
    struct spscq_node *head;
#ifndef CONFIG_SAME_CPU
    char pading[MCT_CACHELINE_BYTES - sizeof(void *)];
#endif
    struct spscq_node *tail;
#ifdef HAVE_Q_LEN
    #ifndef CONFIG_SAME_CPU
    char pading2[MCT_CACHELINE_BYTES - sizeof(void *)];
    #endif
    atomic_t len;
#endif
};

/** @node: could be dummy node or a real first node */
static inline int 
spsc_queue_init(struct spsc_queue *q, struct spscq_node *node)
{
    if (!node){
        return -1;
    }
    node->next = NULL;
    q->head = node;
    q->tail = node;
#ifdef HAVE_Q_LEN
    atomic_set(&q->len, 0);
#endif
    return 0;
}

static inline bool 
spsc_queue_empty(struct spsc_queue *q)
{
    smp_rmb();
    return q->head->next == NULL;
}




static inline void 
spsc_enqueue(struct spsc_queue *q, struct spscq_node *node)
{
    node->next = NULL;
    
    smp_wmb();
    
    q->tail->next = node;
    
    q->tail = node;
#ifdef HAVE_Q_LEN
    atomic_inc(&q->len);
#endif
}

/** */
static inline struct spscq_node * 
spsc_dequeue(struct spsc_queue *q, struct spscq_node **node_to_free)
{
    struct spscq_node *dead_head, *new_head;
    
    dead_head = q->head;
    
    smp_rmb();
    
    new_head = dead_head->next;
    
    if (!new_head){
        *node_to_free = NULL;
        return NULL;
    }
    
#ifdef HAVE_Q_LEN
    atomic_dec(&q->len);
#endif

    q->head = new_head;
    
    *node_to_free = dead_head;
    
    return new_head;
}


#ifdef HAVE_Q_LEN
static inline long spsc_queue_len(struct spsc_queue *q)
{
    return atomic_read(&q->len);
}
#endif

#endif
