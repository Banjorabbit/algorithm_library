#pragma once
#include "fmt/core.h"    // print library
#include "gtest/gtest.h" // google test
#include <dlfcn.h>       // For dynamic loading

// test class that loads and closes the shared library libAlgorithmLibrary.so.
// It is templated to an algorithm, so an instance of this class should be created for each algorithm that is tested.
// The constructor takes a string, which is the name of the function in the shared library that returns a pointer to a new algorithm instance
//
// author: Kristian Timm Andersen

template <typename Talgo>
class TestFixture : public ::testing::Test
{
  public:
    TestFixture(std::string name) { createAlgoFunc = name; }

  protected:
    // Define a function pointer type for the function that creates the instances
    typedef Talgo *(*CreateAlgoFunctionPtr)();

    void SetUp() override
    {
        // Load the shared library dynamically
        handle = dlopen("./libAlgorithmLibrary.so", RTLD_LAZY); // Assuming libMyLibrary.so is in the current directory
        if (!handle) { FAIL() << "Failed to load the shared library: " << dlerror(); }
        else { fmt::print("Succesfully loaded shared library.\n"); }

        createAlgoFuncPtr = reinterpret_cast<CreateAlgoFunctionPtr>(dlsym(handle, createAlgoFunc.c_str())); // create function pointer to create algo instance

        algoPtr = std::shared_ptr<Talgo>(createAlgoFuncPtr()); // assign shared pointer to algo instance
        if (!algoPtr.get())
        {
            std::cerr << "Error: Unable to find function to create instance in the shared library.\n";
            dlclose(handle);
            FAIL() << dlerror();
        }
    }

    void TearDown() override
    {
        // Close the handle to the shared library
        if (handle) { dlclose(handle); }
    }

    std::shared_ptr<Talgo> algoPtr; // shared pointer to algorithm instance

  private:
    std::string createAlgoFunc;              // name of function in shared library that returns a pointer to algorithm instance
    CreateAlgoFunctionPtr createAlgoFuncPtr; // function pointer used during SetUp()
    void *handle = nullptr;                  // handle to the shared library
};
