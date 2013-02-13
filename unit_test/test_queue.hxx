#ifndef __TEST_QUEUE_H_
#define __TEST_QUEUE_H_

#include "gtest.h"
#include "generic_queue.hxx"

typedef int userType;
#ifdef NULL
#undef NULL
#define NULL (void *)0
#endif

template <typename T>
class BasicTest : public ::testing::Test {
    public:
        BasicTest() {}
        ~BasicTest() {}
        T queueImpl;
};
TYPED_TEST_CASE_P(BasicTest);

TYPED_TEST_P(BasicTest, InitState) {
    // Inside a test, refer to the special name TypeParam to get the type
    // parameter.  Since we are inside a derived class template, C++ requires
    // us to visit the members of FooTest via 'this'.
    //create a new Queue.
    //Queue<int>* q = &this->queueImpl;
    TypeParam theQueueImpl; 
    Queue<int>& q = theQueueImpl;
    EXPECT_TRUE(q.empty());
    EXPECT_EQ(0, q.size());
    EXPECT_EQ(NULL, q.dequeue());

    Queue<int>* p = new TypeParam ();
    EXPECT_EQ(p->empty(), true);
    EXPECT_EQ(p->size(), 0);
    delete p;
}

TYPED_TEST_P(BasicTest, queue_1) {
    TypeParam theQueueImpl; 
    Queue<int>& q = theQueueImpl;

    q.enqueue(1234);
    EXPECT_FALSE(q.empty());
    EXPECT_EQ(1, q.size());

    int *n = q.dequeue();
    ASSERT_NE(NULL, n);
    EXPECT_EQ(1234, *n);

    EXPECT_TRUE(q.empty());
    EXPECT_EQ(0, q.size());

    //do it again
    q.enqueue(5678);
    EXPECT_FALSE(q.empty());
    EXPECT_EQ(1, q.size());

    n = q.dequeue();
    ASSERT_NE(NULL, n);
    EXPECT_EQ(5678, *n);

    EXPECT_TRUE(q.empty());
    EXPECT_EQ(0, q.size());
    EXPECT_EQ(NULL, q.dequeue());

}

TYPED_TEST_P(BasicTest, queue_2) {
    Queue<int>& q = this->queueImpl; 

    q.enqueue(1234);
    q.enqueue(5678);
    EXPECT_FALSE(q.empty());
    EXPECT_EQ(2, q.size());
    int *n = q.dequeue();
    ASSERT_NE(NULL, n);
    EXPECT_EQ(1234, *n);
    EXPECT_FALSE(q.empty());
    EXPECT_EQ(1, q.size());

    n = q.dequeue();
    ASSERT_NE(NULL, n);
    EXPECT_EQ(5678, *n);

    EXPECT_TRUE(q.empty());
    EXPECT_EQ(0, q.size());
}


TYPED_TEST_P(BasicTest, dtor) {
    Queue<int>* p = new TypeParam ();
    p->enqueue(1234);
    p->enqueue(5678);
    delete p;
}
REGISTER_TYPED_TEST_CASE_P(BasicTest,
        InitState,
        queue_1,
        queue_2,
        dtor);

/*
 * INSTANTIATE_TYPED_TEST_CASE_P(prefix, BasicTest, theQueueImplClass);
 */

#endif //__TEST_QUEUE_H_
