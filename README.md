# Algorithm Library
Algorithm library is a collection of algorithms written in C++14. The public interface is found in [include](/include/). The implementations are found in subfolders of [src](/src/).

Both the interface and implementations are heavily dependent on [Eigen](https://gitlab.com/libeigen/eigen/).

## Benchmarks
Benchmarks are implemented using [Google benchmark](https://github.com/google/benchmark) and are located [here](apps/benchmark).

## Unit tests
Unit tests are implemented using [Google test](https://github.com/google/googletest) and are located [here](apps/test).

## Python wrapper library
A Python library that wraps the C++ algorithms using [pybind11](https://github.com/pybind/pybind11) is found [here](libs/python_algorithm_library). With this wrapper library one can use the algorithms inside Python. Note that this library requires C++17.

## Build System
Cmake is used as build system and the project setup is inspired by [ModernCppStarter](https://github.com/TheLartians/ModernCppStarter).