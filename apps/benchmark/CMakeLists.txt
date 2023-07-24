
cmake_minimum_required(VERSION 3.16)

# set project name and version
project(DSP_Benchmark VERSION 0.0.1
	DESCRIPTION "Benchmark of DSP algorithms"
	LANGUAGES CXX)

# ---- Include guards ----
if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
message(FATAL_ERROR "builds are not allowed in this folder. Please make a new directory (called a build directory) and run CMake from there.")
endif()

# ---- Add dependencies via CPM ----

#set folder for 3rd party libraries
set(ENV{CPM_SOURCE_CACHE} ${PROJECT_SOURCE_DIR}/../../extern)
include(../../cmake/CPM.cmake)

# add Google benchmark
CPMAddPackage(
  NAME benchmark
  GITHUB_REPOSITORY google/benchmark
  VERSION 1.7.1
  OPTIONS "BENCHMARK_ENABLE_TESTING Off"
)

# add algorithm_library
CPMAddPackage(
  NAME AlgorithmLibrary 
  SOURCE_DIR ${PROJECT_SOURCE_DIR}/../..
)

add_executable(${PROJECT_NAME} src/benchmark_algorithms.cpp)

# We need to include this path in our search path
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/../../include ${PROJECT_SOURCE_DIR}/../../src)

# The project depends on the following libraries
target_link_libraries(${PROJECT_NAME} PRIVATE AlgorithmLibrary benchmark::benchmark)

# All users of this library will need at least C++14
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_14)

