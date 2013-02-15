#include "gtest/gtest.h"
#include "spsc_queue.hxx"

#define MCT_NULL ((void *)0)

/*
class BasicTest : public ::testing::Test {
    public:
        BasicTest() {}
        ~BasicTest() {}
};
*/


TEST(SpscBasicTest, initState) {
    SpscQueue<int> q; 
    EXPECT_TRUE(q.empty());
    EXPECT_EQ(0, q.size());
    EXPECT_EQ(MCT_NULL, q.dequeue());

    SpscQueue<int>* p = new SpscQueue<int>();
    EXPECT_EQ(p->empty(), true);
    EXPECT_EQ(p->size(), 0);
    delete p;
}

TEST(BasicTest, queue_1) {
    SpscQueue<int> q; 

    q.enqueue(1234);
    EXPECT_FALSE(q.empty());
    EXPECT_EQ(1, q.size());

    int *n = q.dequeue();
    ASSERT_NE(MCT_NULL, n);
    EXPECT_EQ(1234, *n);

    EXPECT_TRUE(q.empty());
    EXPECT_EQ(0, q.size());

    //do it again
    q.enqueue(5678);
    EXPECT_FALSE(q.empty());
    EXPECT_EQ(1, q.size());

    n = q.dequeue();
    ASSERT_NE(MCT_NULL, n);
    EXPECT_EQ(5678, *n);

    EXPECT_TRUE(q.empty());
    EXPECT_EQ(0, q.size());
    EXPECT_EQ(MCT_NULL, q.dequeue());

}

TEST(BasicTest, queue_2) {
    SpscQueue<int> q; 

    q.enqueue(1234);
    q.enqueue(5678);
    EXPECT_FALSE(q.empty());
    EXPECT_EQ(2, q.size());
    int *n = q.dequeue();
    ASSERT_NE(MCT_NULL, n);
    EXPECT_EQ(1234, *n);
    EXPECT_FALSE(q.empty());
    EXPECT_EQ(1, q.size());

    n = q.dequeue();
    ASSERT_NE(MCT_NULL, n);
    EXPECT_EQ(5678, *n);

    EXPECT_TRUE(q.empty());
    EXPECT_EQ(0, q.size());
}


TEST(BasicTest, dtor) {
    SpscQueue<int>* p = new SpscQueue<int> ();
    p->enqueue(1234);
    p->enqueue(5678);
    delete p;
}
