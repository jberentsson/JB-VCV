#include "gtest/gtest.h"
#include "counter.cpp"

namespace {
    TEST(CounterTest, InitialValue) {
        Counter c;
        c.main(8);
        EXPECT_EQ(c.index(), 0);
    }

    TEST(CounterTest, Increment) {
        Counter c;
        c.main(8);
        c.increment();
        EXPECT_EQ(c.index(), 1);
    }

    TEST(CounterTest, Increment2) {
        Counter c;
        c.main(8);
        EXPECT_EQ(c.index(), 0);
        c.increment();
        EXPECT_EQ(c.index(), 1);
        c.increment();
        EXPECT_EQ(c.index(), 2);
        c.increment();
        EXPECT_EQ(c.index(), 3);
        c.increment();
        EXPECT_EQ(c.index(), 4);
        c.increment();
        EXPECT_EQ(c.index(), 5);
        c.increment();
        EXPECT_EQ(c.index(), 6);
        c.increment();
        EXPECT_EQ(c.index(), 7);
        c.increment();
        EXPECT_EQ(c.index(), 0);
    }

    TEST(CounterTest, Decrement) {
        Counter c;
        c.main(8);
        c.decrement();
        EXPECT_EQ(c.index(), 7);
    }

    TEST(CounterTest, Decrement2) {
        Counter c;
        c.main(8);
        c.increment();
        c.decrement();
        EXPECT_EQ(c.index(), 0);
        c.increment();
        c.decrement();
        c.increment();
        c.decrement();
        EXPECT_EQ(c.index(), 0);
        c.increment();
        c.decrement();
        c.increment();
        c.decrement();
        c.increment();
        c.decrement();
        c.increment();
        c.decrement();
        EXPECT_EQ(c.index(), 0);
        c.increment();
        c.decrement();
        c.increment();
        c.decrement();
        c.increment();
        c.decrement();
        c.increment();
        c.decrement();
        c.increment();
        c.decrement();
        c.increment();
        c.decrement();
        c.increment();
        c.decrement();
        c.increment();
        c.decrement();
        EXPECT_EQ(c.index(), 0);
    }

    TEST(CounterTest, NotNegative) {
        Counter c;
        c.main(8);
        c.decrement();
        EXPECT_EQ(c.index(), 7);
        c.decrement();
        EXPECT_EQ(c.index(), 6);
        c.decrement();
        EXPECT_EQ(c.index(), 5);
        c.decrement();
        EXPECT_EQ(c.index(), 4);
        c.decrement();
        EXPECT_EQ(c.index(), 3);
        c.decrement();
        EXPECT_EQ(c.index(), 2);
        c.decrement();
        EXPECT_EQ(c.index(), 1);
        c.decrement();
        EXPECT_EQ(c.index(), 0);
        c.decrement();
        EXPECT_EQ(c.index(), 7);
        c.decrement();
        EXPECT_EQ(c.index(), 6);
        c.decrement();
        EXPECT_EQ(c.index(), 5);
        c.decrement();
        EXPECT_EQ(c.index(), 4);
        c.decrement();
        EXPECT_EQ(c.index(), 3);
        c.decrement();
        EXPECT_EQ(c.index(), 2);
        c.decrement();
        EXPECT_EQ(c.index(), 1);
        c.decrement();
        EXPECT_EQ(c.index(), 0);
    }
}