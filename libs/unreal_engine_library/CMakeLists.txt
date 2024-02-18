cmake_minimum_required(VERSION 3.16)

# A cmake-variants.yaml file has been implemented that can switch between two different builds in VS Code (ctrl+shift+p -> Cmake: Select Variant):
# 1) Clang: use Clang compiler and set -stdlib=libc++.
# 2) UnrealPath: use default compiler and link to libc++ headers in Unreal Engine source repo. This only works if the path UNREAL_ENGINE_PATH in the yaml script has been set correctly.
#
# author: Kristian Timm Andersen

# The output static library is put in:
# 1) build/algorithm_library_unreal_engine/Release/libc++
# 2) build/algorithm_library_unreal_engine/Release/unreal_path

# To compile a static library for integration into Unreal Engine on Linux we need to link to libc++, instead of the usual libstdc++.
# if UNREAL_ENGINE_PATH is not defined, then set the compiler to use default Clang (i.e. don't specify any path other than clang/clang++)
# It is necessary in this case since Unreal Engine requires compilation with libc++, and we don't have a path to the libc++ directory, so we have to specify -stdlib=libc++ which only exists in Clang.
if(NOT UNREAL_ENGINE_PATH) 
  set(CMAKE_CXX_COMPILER clang++ CACHE PATH "clang++" FORCE)
  set(CMAKE_C_COMPILER clang CACHE PATH "clang" FORCE )
endif()

# set project name and version
project(AlgorithmLibraryUnrealEngine VERSION 0.0.1
	DESCRIPTION "Static library of DSP algorithms for Unreal Engine"
  LANGUAGES C CXX)

  # include AlgorithmLibrary and set binary output directory
if(UNREAL_ENGINE_PATH) 
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../ ${CMAKE_BINARY_DIR}/unreal_path/)
  # if UNREAL_ENGINE_PATH is given, then link to that version of libc++
  target_compile_options(AlgorithmLibrary PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-nostdinc++>)
  target_compile_options(AlgorithmLibrary PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-I${UNREAL_ENGINE_PATH}Engine/Source/ThirdParty/Unix/LibCxx/include/c++/v1/>)
else()
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../ ${CMAKE_BINARY_DIR}/libc++/)
  # set -stdlib=libc++ 
  target_compile_options(AlgorithmLibrary PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-stdlib=libc++>)
endif()

# add position-independent-code flag since Unreal Engine integrates the static library into a shared library
target_compile_options(AlgorithmLibrary PUBLIC -fPIC)