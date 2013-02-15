#ifndef _MPMC_QUEUE_HXX_
#define _MPMC_QUEUE_HXX_
#include "mpmc_queue.h"

template <typename T>
class MpmcQueue {
public:
    MpmcQueue() {
        mpmc_queue_init(&q);
    }
    ~MpmcQueue() {
        T *userData;
        while (!this->empty()){
            userData = (T *)this->dequeue();
            delete userData;
        }
        MCT_FREE(q.head.ptr);
    }
    bool empty() {
        return mpmc_queue_empty(&q);
    }
    long size() {
        return mpmc_queue_len(&q);
    }
    void enqueue (T *x) {
        mpmc_enqueue(&q, x);
    }
    T* dequeue() {
        return (T *)mpmc_dequeue(&q);
    }

private:
    struct mpmc_queue q;
};


#endif /*_MPMC_QUEUE_HXX_*/

