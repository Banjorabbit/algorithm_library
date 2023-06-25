
cmake_minimum_required(VERSION 3.16)

# add Google benchmark
CPMAddPackage(
  NAME benchmark
  GITHUB_REPOSITORY google/benchmark
  VERSION 1.7.1
  OPTIONS "BENCHMARK_ENABLE_TESTING Off"
)
list(APPEND EXTRA_LIBS benchmark::benchmark) # link to library

# # ----------------------------- BENCHMARK ---------------------------------------------------

# # set project name and version
# project(DSP_Benchmark VERSION 0.0.1
	# DESCRIPTION "Benchmark of DSP algorithms"
	# LANGUAGES CXX)

# add_executable(${PROJECT_NAME} benchmark/Benchmark.cpp)

# # We need to include this path in our search path
# target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)

# # The project depends on the following libraries
# target_link_libraries(${PROJECT_NAME} PRIVATE DSP_Algorithms benchmark::benchmark)

# # All users of this library will need at least C++14
# target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_14)

# # ---------------------------- BENCHMARK ASYNCHRONOUS --------------------------------------

# # set project name and version
# project(DSP_BenchmarkAsynchronous VERSION 0.0.1
	# DESCRIPTION "Benchmark of Asynchronous DSP algorithms"
	# LANGUAGES CXX)

# add_executable(${PROJECT_NAME} benchmark/BenchmarkAsynchronous.cpp)

# # We need to include this path in our search path
# target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)

# # The project depends on the following libraries
# target_link_libraries(${PROJECT_NAME} PRIVATE DSP_Algorithms benchmark::benchmark)

# # All users of this library will need at least C++14
# target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_14)

# ---------------------------- BENCHMARK --------------------------------------

# set project name and version
project(DSP_BenchmarkStatic VERSION 0.0.1
	DESCRIPTION "Benchmark of DSP algorithms"
	LANGUAGES CXX)

add_executable(${PROJECT_NAME} benchmark/benchmark_algorithms.cpp)

# We need to include this path in our search path
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)

# The project depends on the following libraries
target_link_libraries(${PROJECT_NAME} PRIVATE DSP_Algorithms benchmark::benchmark)

# All users of this library will need at least C++14
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_14)

# ------------------------ Google Test -----------------------------------------------------------

# set project name and version
project(DSP_Test VERSION 0.0.1
	DESCRIPTION "Tests of DSP algorithms"
	LANGUAGES CXX)

# add Google test
CPMAddPackage(
  NAME googletest
  GITHUB_REPOSITORY google/googletest
  GIT_TAG release-1.12.1
  VERSION 1.12.1
  OPTIONS "INSTALL_GTEST OFF" "gtest_force_shared_crt"
)  
list(APPEND EXTRA_LIBS gtest gtest_main gmock) # link to library

# add Matplot++ json library
CPMAddPackage(
  NAME matplot
  GIT_TAG v1.1.0
  GITHUB_REPOSITORY alandefreitas/matplotplusplus
)
list(APPEND FRAMEWORKDSP_EXTRA_LIBS Matplot++::matplot) # link to library

file(GLOB_RECURSE SOURCE_LIST_TEST_CPP CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/googleTest/algorithms/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCE_LIST_TEST_CPP})
  
# We need to include this path in our search path
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)

# The project depends on the following libraries
target_link_libraries(${PROJECT_NAME} PRIVATE DSP_Algorithms fmt::fmt gtest gtest_main Matplot++::matplot)

# All users of this library will need at least C++14
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_14)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})