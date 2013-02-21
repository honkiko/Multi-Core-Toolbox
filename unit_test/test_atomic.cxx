#include "gtest/gtest.h"
#include "mct_adapt.h"
#define __STDC_LIMIT_MACROS
#include "stdint.h"

#define MCT_NULL ((void *)0)

struct FetchAndStorePara_t {
    unsigned long x;
    unsigned long y;
};

class FetchAndStoreTest : public ::testing::TestWithParam<FetchAndStorePara_t> {
  // You can implement all the usual fixture class members here.
  // To access the test parameter, call GetParam() from class
  // TestWithParam<T>.
};

TEST_P(FetchAndStoreTest, basic) {
    const FetchAndStorePara_t& para = GetParam();
    ptr_t _OLD_VALUE = (ptr_t)para.x;
    ptr_t _NEW_VALUE = (ptr_t)para.y;
    ptr_t x = _OLD_VALUE; 
    ptr_t v = _NEW_VALUE;
    ptr_t old;

    old = fetch_and_store(&x,v);
     
    EXPECT_EQ(_OLD_VALUE, old);
    EXPECT_EQ(_NEW_VALUE, v);
    EXPECT_EQ(_NEW_VALUE, x);
}

const FetchAndStorePara_t fetchAndStoreParas[] = {
    {1234, 5678},
    {1234, 1234},
    {0, 0},
    {0xffffffff, 0},
    {0xffffffff, 0xffffffff},
    {3412412, 0xffffffff},
};
INSTANTIATE_TEST_CASE_P(Atomic, FetchAndStoreTest,
                        ::testing::ValuesIn(fetchAndStoreParas));


//compare x with cmp, is equal, write x = v
struct CompAndSwapPara_t {
    unsigned long x;
    unsigned long cmp;
    unsigned long v;
};

class CompAndSwapTest : public ::testing::TestWithParam<CompAndSwapPara_t> {
  // You can implement all the usual fixture class members here.
  // To access the test parameter, call GetParam() from class
  // TestWithParam<T>.
};

TEST_P(CompAndSwapTest, basic) {
    const CompAndSwapPara_t& para = GetParam();
    ptr_t x = (ptr_t)para.x; 
    ptr_t cmp = (ptr_t)para.cmp;
    ptr_t v = (ptr_t)para.v;
    char ret;

    ret = compare_and_store(&x,cmp,v);
   
    EXPECT_EQ((ptr_t)para.cmp, cmp);
    EXPECT_EQ((ptr_t)para.v, v);

    if (para.x == para.cmp){ 
        EXPECT_EQ(1, ret);
        EXPECT_EQ((ptr_t)para.v, x);
    } else {
        EXPECT_EQ(0, ret);
        EXPECT_EQ((ptr_t)para.x, x);
    }
}

const CompAndSwapPara_t compAndSwapParas[] = {
    {1234, 5678, 555},
    {1234, 1234, 555},
    {0, 0, 555},
    {0, 0, 0},
    {UINTPTR_MAX, 0, 555},
    {UINTPTR_MAX, UINTPTR_MAX, 555},
    {UINTPTR_MAX, UINTPTR_MAX, UINTPTR_MAX},
    {3412412, UINTPTR_MAX, 555},
};
INSTANTIATE_TEST_CASE_P(Atomic, CompAndSwapTest,
                        ::testing::ValuesIn(compAndSwapParas));



