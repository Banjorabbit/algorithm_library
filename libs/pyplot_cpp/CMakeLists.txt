
cmake_minimum_required(VERSION 3.16)

project(PyPlotCPP VERSION 0.0.1
DESCRIPTION "Library for plotting using Eigen arrays as input"
LANGUAGES CXX)

# ---- Include guards ----
if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
message(FATAL_ERROR "builds are not allowed in this folder. Please make a new directory (called a build directory) and run CMake from there.")
endif()

# ---- Add dependencies via CPM ----

#set folder for 3rd party libraries
set(ENV{CPM_SOURCE_CACHE} ${PROJECT_SOURCE_DIR}/../../extern)
include(../../cmake/CPM.cmake)

# add algorithm_library
CPMAddPackage(
  NAME AlgorithmLibrary 
  SOURCE_DIR ${PROJECT_SOURCE_DIR}/../..
)

# library target
add_library(${PROJECT_NAME} 
  "${PROJECT_SOURCE_DIR}/include/pyplot_cpp/pyplot_cpp.h" 
  "${PROJECT_SOURCE_DIR}/src/matplotlibcpp.h"
  "${PROJECT_SOURCE_DIR}/src/pyplot_cpp.cpp"
)

# We enforce standards conformance on MSVC both compiler and  preprocessor: https://learn.microsoft.com/en-us/cpp/preprocessor/preprocessor-experimental-overview?view=msvc-160
# The preproccessor is needed for macros to be interpreted correctly. Older versions of MSVC might not support these flags. The solution should be to update MSVC (should we print error?)
target_compile_options(${PROJECT_NAME} PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/permissive-; /Zc:preprocessor>")

# With MSVC, enable natvis files to improve debugging experience and increase warning level
if(MSVC)
    target_sources(${PROJECT_NAME} INTERFACE $<BUILD_INTERFACE:${Eigen_SOURCE_DIR}/debug/msvc/eigen.natvis>)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /fp:fast /W4 -wd4127 -wd4100 -wd4459") # suppress warnings from 3rd party libraries - "C4100": unreferenced formal parameter, "C4127": conditional expression is constant, "C4459": Suppress 'declaration of 'var' hides global declaration'
else() # on GCC increase warning level
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra -Wno-unused-parameter -ffast-math") # "no-unused-parameter": suppress unused parameter warning from 3rd party libraries
endif()


target_include_directories(${PROJECT_NAME} PUBLIC "${PROJECT_SOURCE_DIR}/include")

find_package(Python3 COMPONENTS Interpreter Development NumPy REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC
  Python3::Python
  Python3::Module
  Python3::NumPy
  AlgorithmLibrary
)
