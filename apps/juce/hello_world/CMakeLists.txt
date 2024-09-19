cmake_minimum_required(VERSION 3.16)

# set project name and version
project(
  Hello_World
  VERSION 0.0.1
  DESCRIPTION "Hello world example of JUCE application"
  LANGUAGES CXX
)

# ---- Include guards ----
if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  message(FATAL_ERROR "builds are not allowed in this folder. Please make a new directory (called a build directory) and run CMake from there.")
endif()

# set folder for 3rd party libraries
set(ENV{CPM_SOURCE_CACHE} ${PROJECT_SOURCE_DIR}/../../../extern)
include(../../../cmake/CPM.cmake)

# add JUCE
cpmaddpackage(
  NAME
  JUCE
  GITHUB_REPOSITORY
  juce-framework/JUCE
  GIT_TAG
  8.0.1
  OPTIONS
    "JUCE_ENABLE_MODULE_SOURCE_GROUPS ON"
    "JUCE_BUILD_EXAMPLES OFF"
    "JUCE_BUILD_EXTRAS OFF"
)

# add algorithm_library
cpmaddpackage(NAME AlgorithmLibrary SOURCE_DIR ${PROJECT_SOURCE_DIR}/../../..)

#add_executable(${PROJECT_NAME} src/main.cpp)
juce_add_gui_app(${PROJECT_NAME} PRODUCT_NAME "Hello World")

target_sources(${PROJECT_NAME}
PRIVATE
    Main.cpp
    MainComponent.cpp)

#JUCE modules make use of compile definitions to switch certain features on/off, so if there's a particular feature you
# need that's not on by default, check the module header for the correct flag to set here. These
# definitions will be visible both to your code, and also the JUCE module code, so for new
# definitions, pick unique names that are unlikely to collide! This is a standard CMake command.
target_compile_definitions(${PROJECT_NAME}
PRIVATE
    # JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might not need them.
    JUCE_WEB_BROWSER=0  # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_gui_app` call
    JUCE_USE_CURL=0     # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_gui_app` call
    JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:Hello_World,JUCE_PRODUCT_NAME>"
    JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:Hello_World,JUCE_VERSION>")

# We need to include this path in our search path
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/../../../include ${PROJECT_SOURCE_DIR}/../../../src)

# The project depends on the following libraries
target_link_libraries(${PROJECT_NAME} 
    PRIVATE 
        AlgorithmLibrary juce::juce_gui_extra
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)
