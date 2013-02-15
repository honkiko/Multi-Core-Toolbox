#ifndef _MPMC_QUEUE_HXX_
#define _MPMC_QUEUE_HXX_
#include "mpmc_queue.h"

template <typename T>
class MpmcQueue {
public:
    MpmcQueue() {
        struct mpmcq_node *dummy_node = new mpmcq_node();    
        mpmc_queue_init(&q, dummy_node);
    }
    ~MpmcQueue() {
        T *userData;
        while (!this->empty()){
            userData = (T *)this->dequeue();
            delete userData;
        }
        delete q.head.ptr;
    }
    bool empty() {
        return mpmc_queue_empty(&q);
    }
    long size() {
        return mpmc_queue_len(&q);
    }
    void enqueue (T *x) {
        struct mpmcq_node* node = new mpmcq_node();
        node->user_data = (void *)x;
        mpmc_enqueue(&q, node);
    }
    T* dequeue() {
        T *userData;
        struct mpmcq_node *pfree;
        userData = (T *)mpmc_dequeue(&q, &pfree);
        if (pfree) {
            delete pfree;
        }
        return userData;
    }

private:
    struct mpmc_queue q;
};


#endif /*_MPMC_QUEUE_HXX_*/

