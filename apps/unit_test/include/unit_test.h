#pragma once
#include "fmt/core.h"
#include "framework/framework.h"
#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>

namespace InterfaceTests // this namespace contains interface tests and should be used in any unit test of an algorithm
{

template <typename Talgo>
bool resetTest()
{
    Talgo algo;
    auto size = algo.getDynamicSize();
    algo.reset();
    auto sizeOut = algo.getDynamicSize();
    if (size == sizeOut) { return true; }
    else
    {
        fmt::print("resetTest failed.\nSize before reset: {0} \nSize after reset: {1} \n", size, sizeOut);
        return false;
    }
}

template <typename Talgo>
bool getSetTest()
{
    Talgo algo;
    auto size = algo.getDynamicSize();

    auto p = algo.getParameters();
    algo.setParameters(p);
    auto pAll = algo.getParametersTree();
    algo.setParametersTree(pAll);
    auto sizeOut = algo.getDynamicSize();
    if (size != sizeOut)
    {
        fmt::print("getSetTest failed. Parameter test failed.\n");
        return false;
    }

    auto c = algo.getCoefficients();
    algo.setCoefficients(c);
    auto cTree = algo.getCoefficientsTree();
    algo.setCoefficientsTree(cTree);
    auto sizeOut2 = algo.getDynamicSize();
    if (sizeOut != sizeOut2)
    {
        fmt::print("getSetTest failed. Coefficient test failed since dynamic memory changed after setting coefficients. Before: {}. After: {}\n", sizeOut, sizeOut2);
        return false;
    }
    if (size != sizeOut2)
    {
        fmt::print("getSetTest failed. Coefficient test failed.\n");
        return false;
    }

    auto s = algo.getSetup();
    algo.setSetup(s);
    sizeOut = algo.getDynamicSize();
    auto sTree = algo.getSetupTree();
    algo.setSetupTree(sTree);
    sizeOut2 = algo.getDynamicSize();
    if (sizeOut != sizeOut2)
    {
        fmt::print("getSetTest failed. Setup test failed since dynamic memory changed after setting setup. Before: {}. After: {}\n", sizeOut, sizeOut2);
        return false;
    }
    if (size != sizeOut2)
    {
        fmt::print("getSetTest failed. Setup test failed.\n");
        return false;
    }

    return true;
}

template <typename T, typename = void>
struct hasPublicProcessOn : std::false_type
{};

template <typename T>
struct hasPublicProcessOn<T, decltype(void(&T::processAlgorithm))> : std::true_type
{};

template <typename Talgo>
bool assertInterfaceTest()
{
    // The following tests are compile-time tests.
    static_assert(!hasPublicProcessOn<Talgo>(), "processAlgorithm is declared as public method.");

    constexpr bool flagReset = std::is_same<decltype(&Talgo::reset), decltype(&Talgo::BaseAlgorithm::reset)>::value;
    static_assert(flagReset, "reset() is declared in derived algorithm and hiding reset() in base class.");

    constexpr bool flagGetDynamicSize = std::is_same<decltype(&Talgo::getDynamicSize), decltype(&Talgo::BaseAlgorithm::getDynamicSize)>::value;
    static_assert(flagGetDynamicSize, "getDynamicSize() is declared in derived algorithm and hiding getDynamicSize() in base class.");

    constexpr bool flagGetStaticSize = std::is_same<decltype(&Talgo::getStaticSize), decltype(&Talgo::BaseAlgorithm::getStaticSize)>::value;
    static_assert(flagGetStaticSize, "getStaticSize() is declared in derived algorithm and hiding getStaticSize() in base class.");

    constexpr bool flagGetCoefficients = std::is_same<decltype(&Talgo::getCoefficients), decltype(&Talgo::BaseAlgorithm::getCoefficients)>::value;
    static_assert(flagGetCoefficients, "getCoefficients() is declared in derived algorithm and hiding getCoefficients() in base class.");

    constexpr bool flagGetParameters = std::is_same<decltype(&Talgo::getParameters), decltype(&Talgo::BaseAlgorithm::getParameters)>::value;
    static_assert(flagGetParameters, "getParameters() is declared in derived algorithm and hiding getParameters() in base class.");

    constexpr bool flagGetSetup = std::is_same<decltype(&Talgo::getSetup), decltype(&Talgo::BaseAlgorithm::getSetup)>::value;
    static_assert(flagGetSetup, "getSetup() is declared in derived algorithm and hiding getSetup() in base class.");

    constexpr bool flagGetCoefficientsTree = std::is_same<decltype(&Talgo::getCoefficientsTree), decltype(&Talgo::BaseAlgorithm::getCoefficientsTree)>::value;
    static_assert(flagGetCoefficientsTree, "getCoefficientsTree() is declared in derived algorithm and hiding getCoefficientsTree() in base class.");

    constexpr bool flagGetParametersTree = std::is_same<decltype(&Talgo::getParametersTree), decltype(&Talgo::BaseAlgorithm::getParametersTree)>::value;
    static_assert(flagGetParametersTree, "getParametersTree() is declared in derived algorithm and hiding getParametersTree() in base class.");

    constexpr bool flagGetSetupTree = std::is_same<decltype(&Talgo::getSetupTree), decltype(&Talgo::BaseAlgorithm::getSetupTree)>::value;
    static_assert(flagGetSetupTree, "setSetupTree() is declared in derived algorithm and hiding setSetupTree() in base class.");

    constexpr bool flagSetParameters = std::is_same<decltype(&Talgo::setParameters), decltype(&Talgo::BaseAlgorithm::setParameters)>::value;
    static_assert(flagSetParameters, "setParameters() is declared in derived algorithm and hiding setParameters() in base class.");

    constexpr bool flagSetSetup = std::is_same<decltype(&Talgo::setSetup), decltype(&Talgo::BaseAlgorithm::setSetup)>::value;
    static_assert(flagSetSetup, "setSetup() is declared in derived algorithm and hiding setSetup() in base class.");

    // The following tests require an algo object, either because the tested methods are overloaded or return type is auto deducted.
    // They are therefore run-time tests.
    Talgo algo;

    void (Talgo::*ipprocess)(typename Talgo::Input, typename Talgo::Output) = &Talgo::process;
    void (Talgo::*ipprocess2)(typename Talgo::Input, typename Talgo::Output) = &Talgo::BaseAlgorithm::process;
    bool flagProcess = ipprocess == ipprocess2;
    if (!flagProcess) { fmt::print("assertInterfaceTest failed: process(Input, Output) is declared in derived algorithm and hiding process(Input, Output) in base class.\n"); }

    auto c = algo.getCoefficients();
    void (Talgo::*ipsc)(const decltype(c) &) = &Talgo::setCoefficients;
    void (Talgo::*ipsc2)(const decltype(c) &) = &Talgo::BaseAlgorithm::setCoefficients;
    bool flagSetCoefficients = ipsc == ipsc2;
    if (!flagSetCoefficients)
    {
        fmt::print("assertInterfaceTest failed: setCoefficients(...) is declared in derived algorithm and hiding setCoefficients(...) in base class.\n");
    }

    auto cTree = algo.getCoefficientsTree();
    void (Talgo::*ipfsc)(const decltype(cTree) &) = &Talgo::setCoefficientsTree;
    void (Talgo::*ipfsc2)(const decltype(cTree) &) = &Talgo::BaseAlgorithm::setCoefficientsTree;
    bool flagSetCoefficientsTree = ipfsc == ipfsc2;
    if (!flagSetCoefficientsTree)
    {
        fmt::print("assertInterfaceTest failed: setCoefficientsTree(...) is declared in derived algorithm and hiding setCoefficientsTree(...) in base class.\n");
    }

    auto pTree = algo.getParametersTree();
    void (Talgo::*ipfsp)(const decltype(pTree) &) = &Talgo::setParametersTree;
    void (Talgo::*ipfsp2)(const decltype(pTree) &) = &Talgo::BaseAlgorithm::setParametersTree;
    bool flagSetParametersTree = ipfsp == ipfsp2;
    if (!flagSetParametersTree)
    {
        fmt::print("assertInterfaceTest failed: setParametersTree(...) is declared in derived algorithm and hiding setParametersTree(...) in base class.\n");
    }

    auto sTree = algo.getSetupTree();
    void (Talgo::*ipfss)(const decltype(sTree) &) = &Talgo::setSetupTree;
    void (Talgo::*ipfss2)(const decltype(sTree) &) = &Talgo::BaseAlgorithm::setSetupTree;
    bool flagSetSetupTree = ipfss == ipfss2;
    if (!flagSetSetupTree) { fmt::print("assertInterfaceTest failed: setSetupTree(...) is declared in derived algorithm and hiding setSetupTree(...) in base class.\n"); }

    return flagReset && flagGetDynamicSize && flagGetStaticSize && flagGetCoefficients && flagGetParameters && flagGetSetup && flagGetCoefficientsTree &&
           flagGetParametersTree && flagGetSetupTree && flagSetParameters && flagSetSetup && flagProcess && flagGetCoefficientsTree && flagGetParametersTree &&
           flagGetSetupTree && flagSetCoefficients && flagSetCoefficientsTree && flagSetParametersTree && flagSetSetupTree;
}

// test that process algorithm doesn't allocate memory in DEBUG mode
template <typename Talgo>
bool mallocDEBUGTest()
{
    Talgo algo;
    auto input = algo.initInput();
    auto output = algo.initOutput(input);
    Eigen::internal::set_is_malloc_allowed(false);
    algo.process(input, output);
    Eigen::internal::set_is_malloc_allowed(true);
    return true;
}

template <typename Talgo>
bool processTest()
{
    Talgo algo;
    auto input = algo.initInput();
    auto output = algo.initOutput(input);
    if (!algo.validInput(input))
    {
        fmt::print("processTest failed: initial input is not valid.\n");
        return false;
    }
    if (!algo.validOutput(output))
    {
        fmt::print("processTest failed: initial output is not valid.\n");
        return false;
    }
    algo.process(input, output);
    if (!algo.validOutput(output))
    {
        fmt::print("processTest failed: first output is not valid.\n");
        return false;
    }
    double durationMin = 1e10;
    double durationAvg = 0;
    double durationMax = 0;
    for (auto i = 0; i < 100; i++)
    {
        auto start = std::chrono::steady_clock::now();
        algo.process(input, output);
        auto end = std::chrono::steady_clock::now();
        auto time = std::chrono::duration<double, std::micro>(end - start).count();
        durationMin = std::min(durationMin, time);
        durationAvg += time / 100;
        durationMax = std::max(durationMax, time);
    }
    fmt::print("Execution time of processAlgorithm is (min - avg. - max): {:.3f} us - {:.3f} us - {:.3f} us.\n", durationMin, durationAvg, durationMax);
    if (!algo.validOutput(output))
    {
        fmt::print("processTest failed: output is not valid.\n");
        return false;
    }
    return true;
}

template <typename Talgo>
bool parametersTest()
{
    Talgo algo;
    auto c = algo.getParametersTree();
    nlohmann::json j(c); // convert c to json
    fmt::print("parameters: {}\n", j.dump(4));
    c = j; // convert json to c
    return true;
}

template <typename Talgo>
bool coefficientsTest()
{
    Talgo algo;
    auto c = algo.getCoefficientsTree();
    nlohmann::json j(c); // convert c to json
    fmt::print("coefficients: {}\n", j.dump(4));
    c = j; // convert json to c
    return true;
}

template <typename Talgo>
bool versionAlgorithmTest()
{
    constexpr int version = Talgo::ALGORITHM_VERSION_MINOR;
    fmt::print("Algorithm base minor version: {}.\n", version);
    return true;
}

template <typename Talgo>
bool algorithmInterfaceTest(bool testMallocFlag = true)
{
    fmt::print("----------------------------------------------------------------------------------------------------------------------------------\n");
    auto successFlag = coefficientsTest<Talgo>();
    successFlag &= parametersTest<Talgo>();
    successFlag &= versionAlgorithmTest<Talgo>();
    successFlag &= processTest<Talgo>();
    if (testMallocFlag) { successFlag &= mallocDEBUGTest<Talgo>(); }
    successFlag &= resetTest<Talgo>();
    successFlag &= getSetTest<Talgo>();
    successFlag &= assertInterfaceTest<Talgo>();

    Talgo algo;
    auto size = algo.getStaticSize();
    fmt::print("Static memory size: {}\n", size);
    if (size <= 0)
    {
        successFlag = false;
        fmt::print("algorithmInterfaceTest failed: Static memory size is non-positive. \n");
    }

    size = algo.getDynamicSize();
    fmt::print("Dynamic memory size: {}\n", size);
    if (size < 0)
    {
        successFlag = false;
        fmt::print("algorithmInterfaceTest failed: Dynamic memory size is negative.\n");
    }

    if (!successFlag) { fmt::print("algorithmInterfaceTest failed.\n"); }
    fmt::print("----------------------------------------------------------------------------------------------------------------------------------\n");
    return successFlag;
}

} // namespace InterfaceTests