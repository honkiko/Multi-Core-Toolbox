#ifndef __GENERIC_QUEUE__
#define __GENERIC_QUEUE__
#include "stdlib.h"

template <typename T>
class Queue {
public: 
    virtual ~Queue() {}
    virtual bool empty() = 0; 
    virtual long size() = 0;
    virtual void enqueue (const T& x) = 0;
    /*Null means failure*/
    virtual T* dequeue() = 0; 
};

#endif
