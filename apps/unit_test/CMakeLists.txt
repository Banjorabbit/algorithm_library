cmake_minimum_required(VERSION 3.16)

# set project name and version
project(
  DSP_Test
  VERSION 0.0.1
  DESCRIPTION "Tests of DSP algorithms"
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
cpmaddpackage(NAME AlgorithmLibrary SOURCE_DIR ${PROJECT_SOURCE_DIR}/../..)

# add onnx runtime
cpmaddpackage( NAME ONNXRuntime URL https://github.com/microsoft/onnxruntime/releases/download/v1.18.0/onnxruntime-linux-x64-1.18.0.tgz DOWNLOAD_ONLY YES )
add_library(onnxruntime SHARED IMPORTED)
set_target_properties(onnxruntime PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${ONNXRuntime_SOURCE_DIR}/include)
if (MSVC) # TODO: MSVC properties are not set correctly
  #set_target_properties(onnxruntime PROPERTIES IMPORTED_IMPLIB ${ONNXRuntime_SOURCE_DIR}/win32/libonnxruntime.lib)
  #set_target_properties(onnxruntime PROPERTIES IMPORTED_LOCATION ${ONNXRuntime_SOURCE_DIR}/win32/libonnxruntime.dll)
else()
  set_target_properties(onnxruntime PROPERTIES IMPORTED_LOCATION ${ONNXRuntime_SOURCE_DIR}/lib/libonnxruntime.so)
endif()

file(GLOB_RECURSE SOURCE_LIST_TEST_CPP CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCE_LIST_TEST_CPP})

# We need to include this path in our search path
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/../../include ${PROJECT_SOURCE_DIR}/../../src)

# The project depends on the following libraries
target_link_libraries(${PROJECT_NAME} PRIVATE AlgorithmLibrary gtest gtest_main fmt::fmt onnxruntime)

# All users of this library will need at least C++14
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_14)

# define this to detect dynamic memory allocation in debug mode
target_compile_definitions(${PROJECT_NAME} PRIVATE EIGEN_RUNTIME_NO_MALLOC)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})

add_custom_command(
  TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy "${PROJECT_SOURCE_DIR}/../../libs/onnx/*.onnx" $<TARGET_FILE_DIR:${PROJECT_NAME}> 
)
