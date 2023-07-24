# this script adds all subprojects to a single build to allow IDEs understand the full project
# structure.
cmake_minimum_required(VERSION 3.16)

# set project name and version
project(Build_All VERSION 0.0.1
	DESCRIPTION "Build all apps"
	LANGUAGES CXX)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../benchmark ${CMAKE_BINARY_DIR}/benchmark)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../test ${CMAKE_BINARY_DIR}/test)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../spectrogramzer ${CMAKE_BINARY_DIR}/spectrogramzer)