#include "gtest/gtest.h"
#include "mpmc_queue.hxx"

#define MCT_NULL ((void *)0)

/*
class BasicTest : public ::testing::Test {
    public:
        BasicTest() {}
        ~BasicTest() {}
};
*/

struct userStruct {
    userStruct(int n): x(n) {}
    int x;
};

TEST(MpmcBasicTest, initState) {
    MpmcQueue<userStruct> q; 
    EXPECT_TRUE(q.empty());
    EXPECT_EQ(0, q.size());
    EXPECT_EQ(MCT_NULL, q.dequeue());

    MpmcQueue<userStruct>* p = new MpmcQueue<userStruct>();
    EXPECT_EQ(p->empty(), true);
    EXPECT_EQ(p->size(), 0);
    delete p;
}

TEST(MpmcBasicTest, queue_1) {
    MpmcQueue<userStruct> q; 
    struct userStruct *pUserStruct;

    q.enqueue(new userStruct(1234));
    EXPECT_FALSE(q.empty());
    EXPECT_EQ(1, q.size());

    pUserStruct = q.dequeue();
    ASSERT_NE(MCT_NULL, pUserStruct);
    EXPECT_EQ(1234, pUserStruct->x);
    delete pUserStruct;

    EXPECT_TRUE(q.empty());
    EXPECT_EQ(0, q.size());

    //do it again
    q.enqueue(new userStruct(5678));
    EXPECT_FALSE(q.empty());
    EXPECT_EQ(1, q.size());

    pUserStruct = q.dequeue();
    ASSERT_NE(MCT_NULL, pUserStruct);
    EXPECT_EQ(5678, pUserStruct->x);
    delete pUserStruct;

    EXPECT_TRUE(q.empty());
    EXPECT_EQ(0, q.size());
    EXPECT_EQ(MCT_NULL, q.dequeue());

}

TEST(MpmcBasicTest, queue_2) {
    MpmcQueue<userStruct> q; 
    struct userStruct *pUserStruct;

    q.enqueue(new userStruct(1234));
    q.enqueue(new userStruct(5678));
    EXPECT_FALSE(q.empty());
    EXPECT_EQ(2, q.size());

    pUserStruct = q.dequeue();
    ASSERT_NE(MCT_NULL, pUserStruct);
    EXPECT_EQ(1234, pUserStruct->x);
    delete pUserStruct;
    EXPECT_FALSE(q.empty());
    EXPECT_EQ(1, q.size());

    pUserStruct = q.dequeue();
    ASSERT_NE(MCT_NULL, pUserStruct);
    EXPECT_EQ(5678, pUserStruct->x);
    delete pUserStruct;

    EXPECT_TRUE(q.empty());
    EXPECT_EQ(0, q.size());
}


TEST(MpmcBasicTest, dtor) {
    MpmcQueue<userStruct>* p = new MpmcQueue<userStruct> ();
    p->enqueue(new userStruct(1234));
    p->enqueue(new userStruct(5678));
    delete p;
}
