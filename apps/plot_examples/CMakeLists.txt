
cmake_minimum_required(VERSION 3.16)

# set project name and version
project(PlotExamples VERSION 0.0.1
	DESCRIPTION "Application for plotting examples using pyplot_cpp library"
	LANGUAGES CXX)

# ---- Include guards ----
if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
message(FATAL_ERROR "builds are not allowed in this folder. Please make a new directory (called a build directory) and run CMake from there.")
endif()

#set folder for 3rd party libraries
set(ENV{CPM_SOURCE_CACHE} ${PROJECT_SOURCE_DIR}/../../extern)
include(../../cmake/CPM.cmake)

# add algorithm_library
CPMAddPackage(
  NAME PyPlotCPP
  SOURCE_DIR ${PROJECT_SOURCE_DIR}/../../libs/pyplot_cpp
)

add_executable(${PROJECT_NAME} src/main.cpp)
 
# We need to include this path in our search path
#target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/../../include ${PROJECT_SOURCE_DIR}/../../src)

# The project depends on the following libraries
target_link_libraries(${PROJECT_NAME} PRIVATE PyPlotCPP)

