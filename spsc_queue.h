#ifndef _MCT_SPSC_QUEUE_H
#define _MCT_SPSC_QUEUE_H
/*
 * SPSC_QUEUE: 
 * ==========
 * Single Producer, Single Consumer lockless queue.
 * 
 * What's special? 
 * ---------------
 * Producer and consumer can concurrently/parallely work 
 * on the queue without protection by locks.
 * 
 * If you have multiple producers and consumers, turn to 
 * MPSC_QUEUE in the same project.
 * 
 *
 * Implementation options and concerns
 * -----------------------------------
 * 1) spscq_node could be embeded in user defined strutures
 * 2) spscq_node could contain a pointer to user structures
 * The choice have impact to performance and limitations
 * Option 1 have better performance due to less malloc blocks
 * (less calls to memory allocation routines which is expensive)
 * and less pointer dereference(better cache usage).
 * Option 1 have limitation to free the killed dead node when 
 * you have multiple consumers protected by external locks. 
 * Because the user structure may still be in use by other 
 * consumer. With option 2, this is not a problem because the 
 * node does not contain user structure directly and could be 
 * freed without destroy user structure in use.
 * 
 * So it's designed to only for single-producer-single-consumer
 * scenario. If you have single-producer-multiple-consumer or
 * multiple-producer-multiple-consumer, you should use mpmc_queue
 * which is also shipped in this MCT project. And of cause option
 * 2 is taken by mpmc_queue.
 *
 * Another concern is dynamic memory management. All such staff 
 * is stripped out from my implementation. So there's no dependency
 * on specific memory management mechanism and flexable for any 
 * customized memory management algorithm and API. That's why the 
 * "node_to_free" pointer is exposed to user, other than freed 
 * inside the dequeue function.
 *
 *
 * Credit and Copyleft
 * -------------------
 *
 * Invented and published by Maged M. Michael and Michael L. 
 * Scott in the article "Simple, Fast, and Practical Non-Blocking 
 * and Blocking Concurrent Queue Algorithms", PODC96.
 *
 * Implemented and tested by:	
 *		Hong Zhiguo <honkiko@gmail.com/honkiko@163.com>
 *      2012.01.18 
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
 
#include "mct_adapt.h"

/*******************************************************
* Producer(enqueue)         | Consumer(dequeue)
*                           |
* node->next = NULL;        |
*                           | dead_head = q->head;
*                           | new_head = dead_head->next;
* q->tail->next = node;     |	
* smp_wmb();                |
*                           | if (!new_head){
*                           |    return NULL; }
*                           |
*                           | q->head = new_head;
*                           | *node_to_free = dead_head;
*                           |
*					        | return new_head;
*                           |
* q->tail = node;           |
*                           |
********************************************************/

struct spscq_node {
    struct spscq_node *next;
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
} __attribute__((__aligned__(MCT_CACHELINE_BYTES)));

/** @node: could be dummy node or a real first node */
static inline int  
spsc_queue_init(struct spsc_queue *q, struct spscq_node *node)
{
    if (!q || !node){
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

#ifdef __cplusplus
static inline bool 
#else
static inline int 
#endif 
spsc_queue_empty(struct spsc_queue *q)
{
    if (!q){
        return -1;
    }
    smp_rmb();
    return q->head->next == NULL;
}




static inline void 
spsc_enqueue(struct spsc_queue *q, struct spscq_node *node)
{
    if (!q){
        return;
    }

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

    if (!q){
        return NULL;
    }
    
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

/*
 * Why not have spsc_queue_destroy as inline function?
 * 1) We don't know user defined structure which includes spsc_node
 * 2) To avoid any dependency on specific memory management routins
 *    spsc_queue don's calls any mm routine inside APIs.
 *    the mm routines are full exposed to user's choise.
 */
#define spsc_queue_destroy(q, free_node, user_type, node_member) \
    ({                                       \
        struct spscq_node *qnode;            \
        user_type *unode;                    \
        while(!spsc_queue_empty(q)) {        \
            spsc_dequeue((q), &qnode);       \
            if (qnode) {                     \
                unode = container_of(qnode,  \
                    user_type, node_member); \
                free_node(unode);}           \
        }                                    \
        unode = container_of((q)->head,      \
                    user_type, node_member); \
        free_node(unode);                    \
    })

#ifdef HAVE_Q_LEN
static inline long spsc_queue_len(struct spsc_queue *q)
{
    if (!q){
        return 0;
    }
    return atomic_read(&q->len);
}
#endif

#endif
