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
          NEEDS_CURL FALSE
          NEEDS_WEB_BROWSER FALSE
)

target_sources(${PROJECT_NAME} PRIVATE
	src/main.cpp
)

# We need to include this path in our search path
set(INCLUDE_PATHS "${PROJECT_SOURCE_DIR}/../../../include")
if (NOT MSVC) # GTK AND WEBGTK must be included on Linux
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(GTK REQUIRED gtk+-3.0)
  pkg_check_modules(WEBGTK REQUIRED webkit2gtk-4.1)
  string(APPEND INCLUDE_PATHS " ${GTK_INCLUDE_DIRS} ${WEBGTK_INCLUDE_DIRS}")
endif()
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_PATHS})

# The project depends on the following libraries
target_link_libraries(${PROJECT_NAME} 
PRIVATE 
AlgorithmLibrary
juce::juce_audio_basics
juce::juce_gui_basics
juce::juce_audio_utils
juce::juce_recommended_config_flags 
juce::juce_recommended_lto_flags
)