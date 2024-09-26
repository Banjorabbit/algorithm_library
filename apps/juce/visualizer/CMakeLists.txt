cmake_minimum_required(VERSION 3.16)

# set project name and version
project(
  Visualizer
  VERSION 0.0.1
  DESCRIPTION "Visualization of audio"
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
  NAME JUCE
  GITHUB_REPOSITORY juce-framework/JUCE
  GIT_TAG 8.0.1
  OPTIONS
    "JUCE_ENABLE_MODULE_SOURCE_GROUPS ON"
    "JUCE_BUILD_EXAMPLES OFF"
    "JUCE_BUILD_EXTRAS OFF"
)

# add gui magic
cpmaddpackage(
    NAME foleys_gui_magic
    GITHUB_REPOSITORY ffAudio/foleys_gui_magic
    GIT_TAG v1.4.0
    OPTIONS
        "FOLEYS_BUILD_EXAMPLES OFF"
        "FOLEYS_BUILD_TESTS OFF"
        "FOLEYS_RUN_PLUGINVAL OFF"
)

# create gui app
juce_add_gui_app (${PROJECT_NAME}
				  VERSION ${PROJECT_VERSION}
				  COMPANY_NAME "Kris Kode"
          PLUGIN_MANUFACTURER_CODE "KK"
          PRODUCT_NAME "Visualizer"
          BUNDLE_ID "kris_kode"
          NEEDS_CURL FALSE
          NEEDS_WEB_BROWSER FALSE
)

juce_generate_juce_header (${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE
	src/main.cpp
	src/main_component.cpp
	src/main_component.h
)

juce_add_binary_data(${PROJECT_NAME}_data
    SOURCES
    resources/magic.xml
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    ${PROJECT_NAME}_data
    foleys_gui_magic
    # juce::juce_audio_basics
    # juce::juce_audio_plugin_client
    # juce::juce_audio_processors
    # juce::juce_audio_utils
    # juce::juce_dsp
    # juce::juce_cryptography
    # juce::juce_gui_extra
    # juce::juce_opengl
    # juce::juce_recommended_warning_flags 
    juce::juce_recommended_config_flags 
    juce::juce_recommended_lto_flags)

target_compile_definitions(${PROJECT_NAME}
    PUBLIC 
    # switch the following off in the product to hide editor
    FOLEYS_ENABLE_BINARY_DATA=1
    FOLEYS_SHOW_GUI_EDITOR_PALLETTE=1
    FOLEYS_SAVE_EDITED_GUI_IN_PLUGIN_STATE=0
    JUCE_WEB_BROWSER=0
)

if (NOT MSVC)
    # foleys gui magic is full of deprecated warnings so suppress them to make the output readable
    target_compile_options(${PROJECT_NAME} PUBLIC "-Wno-deprecated-declarations")
endif()