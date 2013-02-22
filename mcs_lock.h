#ifndef _MCT_MCS_LOCK_H_
#define _MCT_MCS_LOCK_H_

#include "mct_adapt.h"
#ifdef __KERNEL__
#include "asm/processor.h"  /*cpu_relax()*/
#endif

struct _mcslock_node {
    struct _mcslock_node *volatile next; /*next one waiting the lock*/
    volatile char wait;                  /*should wait or not*/
} __attribute__((__aligned__(MCT_CACHELINE_BYTES))) mcslock_node;


typedef struct {
    _mcslock_node *volatile tail;        /*queue tail*/
} mcslock_t;



static inline void mcs_lock_init(mcslock_t *lock)
{
   lock->tail = NULL; 
}

/**
 * Here's a trick: we alloc the node on stack
 * to get both good performance and simplicity
 */
static inline void mcs_lock(mcslock_t *lock, struct _mcslock_node *mynode)
{
    struct _mcslock_node *pre;
    mynode->next = NULL;
    mynode->wait = 1;
   
    /*atomic: {pre=lock->tail; tail=node;} */ 
    pre = fetch_and_store(&lock->tail, mynode);
    if (pre == NULL) {
        /* It's free, and taken by me */
        return;              
    }
    
    /* it's hold by others. queue up and spin on the node of myself */
    pre->next = mynode;
    smp_wmb();
    while (mynode->wait) {
        /* nop or some power saving instruction like "pause" on x86*/
        cpu_relax();   
    }
}

static inline void mcs_unlock(mcslock_t *lock, struct _mcslock_node *mynode)
{
    if (mynode->next == NULL) { 
        /* 
         * I'm the last one temporally, now race with other new requester:
         * They may try to fetch_and_store(&lock->tail, their_node),
         * while I'll try to "set lock->tail=me, if I'm still the last one"
         */ 
        if (compare_and_store(&lock->tail, mynode, NULL)) {
            return;
        }
        /* we failed the race, someone else will wait afer me soon
         * wait util he complete mcs_lock: "pre->next = mynode;"
         */ 
        while ( mynode->next == NULL) {
            /* usally we don't need cpu_relax because the follower need only 
             * several instructions to put himself on my "next" to free myself
             * But in case the follower got interrupted or TLB miss or page fault ... 
             */
            smp_rmb();
            cpu_relax();  
        }
    } /*if (mynode->next == NULL)*/

    /* here I'm sure someone is waiting after me, pass the lock 
       to him and return*/
    mynode->next->wait = 0;
    return;
}

static inline int mcs_trylock(mcslock_t *lock, struct _mcslock_node *mynode)
{
    if (compare_and_store(&lock->tail, NULL, mynode)) 
        return 1;
    else
        return 0; 
}

static inline int mcs_is_locked(mcslock_t *lock)
{
    return lock->tail != NULL;
}

#endif /*_MCT_MCS_LOCK_H_*/
