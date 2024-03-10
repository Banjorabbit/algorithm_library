#include "gtest/gtest.h"
#include "framework/unit_test.h"
#include "framework/read_write_type.h"


// --------------------------------------------- TEST CASES ---------------------------------------------

// run get() in one thread and set() in main thread and check we don't crash
// pass criteria: test runs without crashing and all subsequent reads are larger or equal to previous read
TEST(ReadWriteType, RunTwoThreads)
{
    // initialize data
    int i = -1;
    ReadWriteType<int> data(i);

    // run write loop in separate thread
    std::thread t1([&data](){
        for (auto i = 0; i < 1000000; i++)
        {
            data.set(i);
        }
    });

    // loop reads
    int iterFail = -1;
    int value = -1;
    int valueOld = -1;
    for (int i = 0; i < 150000; i++) 
    {
        value = data.get();
        if (value < valueOld) // test pass criteria
        {
            iterFail = i;
            break;
        }
        valueOld = value;
        std::this_thread::yield();
    }
    int numElements = data.getNumElements();
    fmt::print("Value: {}\n", value);
    fmt::print("Number of elements in data: {}\n", numElements);

    // join thread
    t1.join();

    if (iterFail == -1) { fmt::print("Test succeeded\n"); }
    else { fmt::print("Test failed in iteration: {}\n", iterFail); }

    EXPECT_TRUE(iterFail = -1);
}
