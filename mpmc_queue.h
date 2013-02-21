#ifndef _MCT_MPMC_QUEUE_H_
#define _MCT_MPMC_QUEUE_H_
/*
 * MPMC_QUEUE: 
 * ==========
 * Multiple Producers, Multiple Consumers lock-free queue.
 * 
 * What's special? 
 * ---------------
 * Two advantages over queue protected by lock (like the 
 * skb_queue_xxx APIs in Linux kernel, which is the core 
 * utility of Linux kernel networking subsystem):
 * 1) Producer and comsumer works parallely
 * 2) One producer never get blocked by other producer. 
 *    Same for comsumers. This means lock-free. (for precise
 *    definition of lock-free/wait-free, please refer to 
 *    "The Art of Multiprocessor Programming" by Herlihy and
 *    Shavit. 
 *
 * Implementation options and concerns
 * -----------------------------------
 * To overcome ABA problem, DCAS(Double Compare and Swap) is
 * used for x86. For RISC processors with LL/SC primitives, 
 * the ABA problem is naturally avoided.

 *
 * Credit and Copyleft
 * -------------------
 * Invented and published by Maged M. Michael and Michael L. 
 * Scott in the article "Simple, Fast, and Practical Non-Blocking 
 * and Blocking Concurrent Queue Algorithms", PODC96.
 *
 * Implemented and tested by:	
 *		Hong Zhiguo <honkiko@gmail.com>
 *      2012.01.18 
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "mct_adapt.h"

struct mpmcq_node;

struct versioned_pointer {
    struct mpmcq_node *ptr;
    unsigned long version;
} __attribute__ (( __aligned__( 16 ) ));

/* for comparation of 2 versioned_pointer */
#define vp_equal(x,y)                           \
    ( ((x).ptr == ACCESS_ONCE((y).ptr))         \
      &&                                        \
      ((x).version == ACCESS_ONCE((y).version)) \
    )

#define vp_assign(x,y)                          \
    ({                                          \
       (x).ptr = ACCESS_ONCE((y).ptr);          \
       (x).version = ACCESS_ONCE((y).version);  \
    })

struct mpmcq_node {
    struct versioned_pointer next;
    void *user_data;
};

struct mpmc_queue {
    struct versioned_pointer head;
#ifndef CONFIG_SAME_CPU
    char pading[MCT_CACHELINE_BYTES - sizeof(struct versioned_pointer)];
#endif
    struct versioned_pointer tail;
#ifdef HAVE_Q_LEN
#ifndef CONFIG_SAME_CPU
    char pading2[MCT_CACHELINE_BYTES - sizeof(struct versioned_pointer)];
#endif
    atomic_t len;
#endif
};


static inline int
mpmc_queue_init(struct mpmc_queue *q) 
{
    struct mpmcq_node *node;
    if (!q){
        return -1;
    }
    node = (struct mpmcq_node *)MCT_MALLOC(sizeof(struct mpmcq_node));
    if (!node){
        return -1;
    }
    node->next.ptr = NULL;
    node->next.version = 0;
    q->head.ptr = node;
    q->head.version = 0;
    q->tail.ptr = node;
    q->tail.version = 0;
#ifdef HAVE_Q_LEN
    atomic_set(&q->len, 0);
#endif
    return 0;
}

static inline int 
mpmc_queue_empty(struct mpmc_queue *q)
{
    if (!q){
        return 1;
    }
    smp_rmb();
    return q->head.ptr->next.ptr == NULL;
}

static inline int 
mpmc_enqueue(struct mpmc_queue *q, void *data)
{
    struct mpmcq_node *node;
    unsigned char swapped;
    struct versioned_pointer tail;
    struct versioned_pointer next;
    struct versioned_pointer pnode; /*pointer to node*/
    if (!q){
        return -1;
    }
    node = (struct mpmcq_node *)MCT_MALLOC(sizeof(struct mpmcq_node));
    if (!node){
        return -1;
    }
    node->user_data = data;
    node->next.ptr = NULL;
    node->next.version = 0;
    while(1) {
        //tail = ACCESS_ONCE(q->tail);
        vp_assign(tail,q->tail);
        //next = tail.ptr->next;
        vp_assign(next, tail.ptr->next);
        barrier();
        if ( vp_equal(tail, q->tail)){
            if (next.ptr == NULL){
                pnode.ptr = node; 
                pnode.version = next.version + 1; 
                swapped = double_cmp_and_swap(&tail.ptr->next, &next, &pnode);
                if (swapped) {
                    break;
                }
            } else {
                pnode.ptr = next.ptr;
                pnode.version = tail.version + 1;
                double_cmp_and_swap(&q->tail, &tail, &pnode);
            }
        }
    } /*while*/
    pnode.ptr = node;
    pnode.version = tail.version + 1;
    double_cmp_and_swap(&q->tail, &tail, &pnode); 
#ifdef HAVE_Q_LEN
    atomic_inc(&q->len);
#endif
    return 0;
}


/**different than spsc_queue: user_data is returned, instead of node */
static inline void * 
mpmc_dequeue(struct mpmc_queue *q)
{
    unsigned char swapped;
    void * p_user_data;
    struct versioned_pointer head;
    struct versioned_pointer tail;
    struct versioned_pointer next;
    struct versioned_pointer pnode;

    if (!q){
        return NULL;
    }

    while (1) {
        //head = ACCESS_ONCE(q->head); 
        vp_assign(head, q->head);
        //tail = ACCESS_ONCE(q->tail); 
        vp_assign(tail, q->tail);
        //next = head.ptr->next;
        vp_assign(next, head.ptr->next);
        if (! vp_equal(head, q->head)) {
            continue;
        }

        if (head.ptr == tail.ptr) {
            if (next.ptr == NULL) {
                return NULL;
            }
            pnode.ptr = next.ptr;
            pnode.version = tail.version + 1;
            double_cmp_and_swap(&q->tail, &tail, &pnode);
        } else {
            p_user_data = next.ptr->user_data;
            pnode.ptr = next.ptr;
            pnode.version = head.version + 1;
            swapped = double_cmp_and_swap(&q->head, &head, &pnode); 
            if (swapped) {
                break;
            }
        } /*if (head.ptr == tail.ptr)*/
    } /*while (1)*/ 
    
#ifdef HAVE_Q_LEN
    atomic_dec(&q->len);
#endif
    MCT_FREE(head.ptr);
    return p_user_data;
}

/*mpmp_queue_destroy must be called exclusively*/
static inline void
mpmc_queue_destroy(struct mpmc_queue *q)
{
    void *data;
    if (!q){
        return;
    }
    while (!mpmc_queue_empty(q)) {
        data = mpmc_dequeue(q);
        if (data){
            MCT_FREE(data);
        }
    }
    /*free the dead node*/
    MCT_FREE(q->head.ptr); 
}


#ifdef HAVE_Q_LEN
static inline long 
mpmc_queue_len(struct mpmc_queue *q)
{
    if (!q){
        return 0;
    }
    return atomic_read(&q->len);
}
#endif



#endif

