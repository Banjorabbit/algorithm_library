cmake_minimum_required(VERSION 3.16)

# set project name and version
project(
  DSP_Integration_Test
  VERSION 0.0.1
  DESCRIPTION "Integration Tests of DSP algorithms library"
  LANGUAGES CXX
)

# ---- Include guards ----
if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  message(FATAL_ERROR "builds are not allowed in this folder. Please make a new directory (called a build directory) and run CMake from there.")
endif()

# ---- Add dependencies via CPM ----

# set folder for 3rd party libraries
set(ENV{CPM_SOURCE_CACHE} ${PROJECT_SOURCE_DIR}/../../extern)
include(../../cmake/CPM.cmake)

# add Google test
cpmaddpackage(
  NAME
  googletest
  GITHUB_REPOSITORY
  google/googletest
  GIT_TAG
  release-1.12.1
  VERSION
  1.12.1
  OPTIONS
  "INSTALL_GTEST OFF"
  "gtest_force_shared_crt"
)

# add format library
cpmaddpackage(
  NAME
  fmt
  GIT_TAG
  9.0.0
  GITHUB_REPOSITORY
  fmtlib/fmt
  OPTIONS
  "FMT_INSTALL YES" # create an installable target
)

# add algorithm_library
cpmaddpackage(NAME AlgorithmLibrary SOURCE_DIR ${PROJECT_SOURCE_DIR}/../.. OPTIONS "ALGORITHM_LIBRARY_SHARED_LIBRARY")

# add nlohmann json library
cpmaddpackage(
  NAME
  nlohmann_json
  VERSION
  3.11.2
  GITHUB_REPOSITORY
  nlohmann/json # ArthurSonzogni/nlohmann_json_cmake_fetchcontent
  OPTIONS
  "JSON_BuildTests OFF"
  "JSON_Install ON"
)

# add Eigen library (CMakeLists.txt in this project is not suitable for including as subdirectory, and it's a header-only project so only download it)
cpmaddpackage(NAME Eigen URL https://gitlab.com/libeigen/eigen/-/archive/3.4.0/eigen-3.4.0.zip DOWNLOAD_ONLY YES)
add_library(Eigen INTERFACE IMPORTED)
target_include_directories(Eigen INTERFACE ${Eigen_SOURCE_DIR})

file(GLOB_RECURSE SOURCE_LIST_TEST_CPP CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCE_LIST_TEST_CPP})

# We need to include this path in our search path
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src)

# This path is to the headers of the shared library
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/../../include)

# The project depends on the following libraries
target_link_libraries(${PROJECT_NAME} PRIVATE gtest gtest_main fmt::fmt Eigen nlohmann_json::nlohmann_json)

# All users of this library will need at least C++14
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_14)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})

# Add a custom command to copy the shared AlgorithmLibrary to the build directory
add_custom_command(
  TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:AlgorithmLibrary> $<TARGET_FILE_DIR:${PROJECT_NAME}>
  COMMENT "Copying shared library to build directory"
)
