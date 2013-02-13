#include <list>
#include "generic_queue.hxx"
#include "spsc_queue.hxx"
#include "test_queue.hxx"
#include "gtest/gtest.h"
using namespace std;

INSTANTIATE_TYPED_TEST_CASE_P(Spsc, BasicTest, SpscQueue<int>);

