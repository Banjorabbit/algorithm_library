cmake_minimum_required(VERSION 3.16)

# set project name and version
project(DSP_Algorithms VERSION 0.0.1
	DESCRIPTION "Source code for library of DSP algorithms"
	LANGUAGES CXX C)

# If we want the library to be SHARED, aka a DLL, then we have to tell cmake to export symbols for the applications that are using the DLL. Therefore, uncomment this line if add_library is SHARED
#set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

file(GLOB_RECURSE HEADER_LIST_ALGORITHMS CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/include/algorithm_library/*.h")
file(GLOB_RECURSE HEADER_LIST_INTERFACE CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/include/interface/*.h")

file(GLOB_RECURSE SOURCE_LIST_CPP CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE SOURCE_LIST_C CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/*.c")
file(GLOB_RECURSE SOURCE_LIST_HEADERS CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/*.h")

# add SHARED if a DLL is needed (see CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS comment above about exporting symbols)
add_library(${PROJECT_NAME} ${HEADER_LIST_ALGORITHMS} ${HEADER_LIST_INTERFACE} ${SOURCE_LIST_C} ${SOURCE_LIST_CPP} ${SOURCE_LIST_HEADERS}
)

# We enforce standards conformance on MSVC both compiler and  preprocessor: https://learn.microsoft.com/en-us/cpp/preprocessor/preprocessor-experimental-overview?view=msvc-160
# The preproccessor is needed for macros to be interpreted correctly. Older versions of MSVC might not support these flags. The solution should be to update MSVC (should we print error?)
target_compile_options(${PROJECT_NAME} PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/permissive-; /Zc:preprocessor>")

#target_compile_options(${PROJECT_NAME} PRIVATE  $<$<CONFIG:Release>:-DNDEBUG -O2 -ffast-math -fno-exceptions -s -ffunction-sections -fdata-sections -flto>)
  
# With MSVC, enable natvis files to improve debugging experience and increase warning level
if(MSVC)
    target_sources(${PROJECT_NAME} INTERFACE $<BUILD_INTERFACE:${Eigen_SOURCE_DIR}/debug/msvc/eigen.natvis>)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /fp:fast /W4 -wd4127 -wd4100 -wd4459") # suppress warnings from 3rd party libraries - "C4100": unreferenced formal parameter, "C4127": conditional expression is constant, "C4459": Suppress 'declaration of 'var' hides global declaration'
else() # on GCC increase warning level
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra -Wno-unused-parameter -ffast-math") # "no-unused-parameter": suppress unused parameter warning from 3rd party libraries
endif()

# We need to include this path in our search path and users of this library also need it
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/src)

# The project depends on the following libraries. They are PUBLIC since they are used in the headers so anyone using this library needs them as well
target_link_libraries(${PROJECT_NAME} PUBLIC Eigen nlohmann_json::nlohmann_json fmt::fmt)

# All users of this library will need at least C++14
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_14)

# IDEs should put the files in a nice place
source_group(
  TREE "${CMAKE_SOURCE_DIR}/include"
  PREFIX "Public Header Files"
  FILES ${HEADER_LIST_ALGORITHMS} ${HEADER_LIST_INTERFACE})

source_group(
  TREE "${CMAKE_SOURCE_DIR}/src/"
  PREFIX "Internal Files"
  FILES ${SOURCE_LIST_CPP} ${SOURCE_LIST_C} ${SOURCE_LIST_HEADERS})