cmake_minimum_required(VERSION 3.16)

# set project name and version
project(
  Hello_World_Foleys_GUI_Magic
  VERSION 0.0.1
  DESCRIPTION "Hello world example of JUCE application using Foleys GUI magic"
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

cpmaddpackage(
    NAME foleys_gui_magic
    GITHUB_REPOSITORY ffAudio/foleys_gui_magic
    GIT_TAG v1.4.0
    OPTIONS
        "FOLEYS_BUILD_EXAMPLES OFF"
        "FOLEYS_BUILD_TESTS OFF"
        "FOLEYS_RUN_PLUGINVAL OFF"
)
set(FORMATS "Standalone" "VST3")

set(${PROJECT_NAME}_sources
    src/PluginProcessor.cpp
    src/PluginProcessor.h
)

juce_add_plugin(${PROJECT_NAME} VERSION "${version}"
FORMATS ${FORMATS}
VST3_CATEGORIES "Fx" "Analyser" "EQ"
PRODUCT_NAME "Hello World" 
)

juce_add_binary_data(${PROJECT_NAME}_data
SOURCES
    resources/Logo.png
    resources/magic.xml
)

juce_generate_juce_header(${PROJECT_NAME})

target_sources(${PROJECT_NAME}
PRIVATE
    ${${PROJECT_NAME}_sources}
)

# add required flags
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    ${project_sources}
    ${PROJECT_NAME}_data
    foleys_gui_magic
    juce::juce_audio_basics
    juce::juce_audio_plugin_client
    juce::juce_audio_processors
    juce::juce_audio_utils
    juce::juce_dsp
    juce::juce_cryptography
    juce::juce_gui_extra
    juce::juce_opengl
    juce::juce_recommended_warning_flags 
    juce::juce_recommended_config_flags 
    juce::juce_recommended_lto_flags
)

target_compile_definitions(${PROJECT_NAME}
PUBLIC
    # switch the following off in the product to hide editor
    FOLEYS_SHOW_GUI_EDITOR_PALLETTE=1
    FOLEYS_SAVE_EDITED_GUI_IN_PLUGIN_STATE=0
    FOLEYS_ENABLE_BINARY_DATA=1
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_SILENCE_XCODE_15_LINKER_WARNING=1
    JUCE_WEB_BROWSER=0
)
