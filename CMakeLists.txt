cmake_minimum_required(VERSION 3.16)

# set project name and version
project(FrameworkDSP VERSION 0.0.1
	DESCRIPTION "Library of DSP algorithms"
	LANGUAGES CXX C)

# ---- Include guards ----
if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  message(FATAL_ERROR "builds are not allowed in this folder. Please make a new directory (called a build directory) and run CMake from there.")
endif()

option(FrameworkDSP_BUILD_TESTING 0)

# Only do these if this is the main project, and not if it is included through add_subdirectory
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME) 

  # Optionally set things like CMAKE_CXX_STANDARD, CMAKE_POSITION_INDEPENDENT_CODE here
  set(CMAKE_CXX_STANDARD 14)
  
  # Let's ensure -std=c++xx instead of -std=g++xx
  set(CMAKE_CXX_EXTENSIONS OFF)

  # Let's nicely support folders in IDEs
  set_property(GLOBAL PROPERTY USE_FOLDERS ON)

  # enable testing
  set(BUILD_TESTING 1)

    # # Docs only available if this is the main app
  # find_package(Doxygen)
  # if(Doxygen_FOUND)
    # add_subdirectory(docs)
  # else()
    # message(STATUS "Doxygen not found, not building docs")
  # endif()
endif()

# ---- Add dependencies via CPM ----

#set folder for 3rd party libraries
set(ENV{CPM_SOURCE_CACHE} ${PROJECT_SOURCE_DIR}/extern)
include(cmake/CPM.cmake)

# add nlohmann json library
CPMAddPackage(
  NAME nlohmann_json
  VERSION 3.10.5
  GITHUB_REPOSITORY nlohmann/json #ArthurSonzogni/nlohmann_json_cmake_fetchcontent
  OPTIONS "JSON_BuildTests OFF" "JSON_Install ON"
)
list(APPEND FRAMEWORKDSP_EXTRA_LIBS nlohmann_json::nlohmann_json) # link to library

# add Eigen library
CPMAddPackage(
    NAME Eigen
    URL https://gitlab.com/libeigen/eigen/-/archive/3.4.0/eigen-3.4.0.zip
    GIT_TAG 3.4.0
    DOWNLOAD_ONLY YES
)
add_library(Eigen INTERFACE IMPORTED)
target_include_directories(Eigen INTERFACE ${Eigen_SOURCE_DIR})
list(APPEND FRAMEWORKDSP_EXTRA_LIBS Eigen) # link to library

# add format library
CPMAddPackage(
  NAME fmt
  GIT_TAG 9.0.0
  GITHUB_REPOSITORY fmtlib/fmt
  OPTIONS "FMT_INSTALL YES" # create an installable target
)
list(APPEND FRAMEWORKDSP_EXTRA_LIBS fmt::fmt) # link to library	

add_subdirectory(src)

# Testing only available if this is the main app or variable FrameworkDSP_BUILD_TESTING is set 
if(BUILD_TESTING OR FrameworkDSP_BUILD_TESTING)
  add_subdirectory(test)
endif()

