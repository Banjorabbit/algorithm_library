cmake_minimum_required(VERSION 3.16)

# set project name and version
project(
  VisualizerComponents
  VERSION 0.0.1
  DESCRIPTION "Components for making an audio visualizer"
  LANGUAGES CXX
)


# ---- Add dependencies via CPM ----

# set folder for 3rd party libraries
set(ENV{CPM_SOURCE_CACHE} ${PROJECT_SOURCE_DIR}/../../../extern)
include(../../../cmake/CPM.cmake)

# add JUCE
cpmaddpackage(
  NAME JUCE
  GITHUB_REPOSITORY juce-framework/JUCE
  GIT_TAG 8.0.1
  OPTIONS
    "JUCE_ENABLE_MODULE_SOURCE_GROUPS ON"
    "JUCE_BUILD_EXAMPLES OFF"
    "JUCE_BUILD_EXTRAS OFF"
)

# add algorithm_library
cpmaddpackage(NAME AlgorithmLibrary SOURCE_DIR ${PROJECT_SOURCE_DIR}/../../..)

# create gui app
juce_add_gui_app (${PROJECT_NAME}
				  VERSION ${PROJECT_VERSION}
				  COMPANY_NAME "Kris Kode"
          PLUGIN_MANUFACTURER_CODE "KK"
          PRODUCT_NAME "Visualizer Components"
          BUNDLE_ID "kris_kode"
          NEEDS_CURL TRUE
          NEEDS_WEB_BROWSER FALSE
)

target_sources(${PROJECT_NAME} PRIVATE
	src/main.cpp
)

# We need to include this path in our search path
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/../../../include ${PROJECT_SOURCE_DIR}/../../../src)

# The project depends on the following libraries
target_link_libraries(${PROJECT_NAME} 
PRIVATE 
AlgorithmLibrary
juce::juce_audio_basics
juce::juce_gui_basics
)