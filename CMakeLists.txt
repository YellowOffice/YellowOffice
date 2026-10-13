cmake_minimum_required(VERSION 3.16)

# YellowOffice - Native Office Suite for Haiku OS
project(YellowOffice LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Platform detection: a real Haiku build uses the system API headers and
# libbe; any other host uses the stub header layer in build/haiku-stub for
# development builds of the non-UI libraries and unit tests.
if(CMAKE_SYSTEM_NAME STREQUAL "Haiku")
    set(YELLOW_ON_HAIKU TRUE)
else()
    set(YELLOW_ON_HAIKU FALSE)
endif()

if(YELLOW_ON_HAIKU)
    option(YELLOW_USE_STUBS "Use the bundled Haiku API stub headers" OFF)
else()
    option(YELLOW_USE_STUBS "Use the bundled Haiku API stub headers" ON)
endif()
option(ENABLE_TESTS "Build unit tests" ON)
option(ENABLE_INSTALL "Enable install targets" ${YELLOW_ON_HAIKU})

add_compile_options(-Wall -Wextra -Wno-unused-parameter)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-g -O0)
    add_definitions(-DDEBUG -D_DEBUG)
else()
    add_compile_options(-O2 -DNDEBUG)
endif()

if(YELLOW_ON_HAIKU AND CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "/boot/common" CACHE PATH "Default install path" FORCE)
endif()

# Common include roots.
set(YELLOW_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(YELLOW_COMMON_INC ${YELLOW_ROOT}/libraries/YellowCommon/include)
set(YELLOW_CORE_INC ${YELLOW_ROOT}/libraries/YellowDocumentCore/include)
set(YELLOW_LAYOUT_INC ${YELLOW_ROOT}/libraries/YellowDocumentLayout/include)
set(YELLOW_IMPORT_EXPORT_INC ${YELLOW_ROOT}/libraries/YellowImportExport/include)
set(YELLOW_UI_INC ${YELLOW_ROOT}/libraries/YellowUI/include)
set(YELLOW_STUB_INC ${YELLOW_ROOT}/build/haiku-stub)

if(YELLOW_ON_HAIKU)
    find_library(HAIKU_BE_LIBRARY be REQUIRED)
    find_library(HAIKU_TRACKER_LIBRARY tracker)
    set(YELLOW_PLATFORM_LIBS ${HAIKU_BE_LIBRARY})
    if(HAIKU_TRACKER_LIBRARY)
        list(APPEND YELLOW_PLATFORM_LIBS ${HAIKU_TRACKER_LIBRARY})
    endif()
else()
    set(YELLOW_PLATFORM_LIBS "")
endif()

# Platform include path: system headers on Haiku, stubs elsewhere.
if(YELLOW_USE_STUBS AND NOT YELLOW_ON_HAIKU)
    include_directories(${YELLOW_STUB_INC})
endif()

find_package(ZLIB REQUIRED)

# Libraries
add_subdirectory(libraries/YellowCommon)
add_subdirectory(libraries/YellowDocumentCore)
add_subdirectory(libraries/YellowImportExport)

if(YELLOW_ON_HAIKU)
    add_subdirectory(libraries/YellowDocumentLayout)
    add_subdirectory(libraries/YellowUI)
    add_subdirectory(apps/YellowDocs)
endif()

# Tests (host-only: non-UI libraries)
if(ENABLE_TESTS AND NOT YELLOW_ON_HAIKU)
    enable_testing()
    add_subdirectory(tests)
endif()

# Haiku packaging is driven by scripts/package.sh (build, rc/xres, package
# create); it is not part of the CMake build.
