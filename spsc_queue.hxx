#ifndef _SPSC_QUEUE_HXX_
#define _SPSC_QUEUE_HXX_
#include "spsc_queue.h"

// T must have copy ctor
template <typename T>
struct _spscqUserNode: public spscq_node {
    T userData;
    _spscqUserNode() {}
    _spscqUserNode(const T& t):userData(t) {}
};

// T must have copy ctor
template <typename T>
class SpscQueue {
public:
    typedef _spscqUserNode<T> userNode;
    
    SpscQueue() {
        userNode* dummy_node = new userNode();    
        spsc_queue_init(&q, dummy_node);
    }
    ~SpscQueue() {
        while (!this->empty()){
            this->dequeue();
        }
        delete (userNode*)(q.head);
    }
    bool empty() {
        return spsc_queue_empty(&q);
    }
    long size() {
        return spsc_queue_len(&q);
    }
    void enqueue (const T& x) {
        userNode* unode = new userNode(x);
        spsc_enqueue(&q, unode);
    }
    T* dequeue() {
        struct spscq_node *pnode;
        struct spscq_node *pfree;
        pnode = spsc_dequeue(&q, &pfree);
        if (pfree) {
            delete pfree;
        }
        if (!pnode) {
            return NULL;
        }
        userNode* unode = (userNode *)pnode;
        return &unode->userData;
    }


private:
    struct spsc_queue q;
};

#endif /*_SPSC_QUEUE_HXX_*/

