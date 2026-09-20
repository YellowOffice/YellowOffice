cmake_minimum_required(VERSION 3.16)

# YellowOffice - Native Office Suite for Haiku OS
project(YellowOffice LANGUAGES C CXX)

# Haiku-specific settings
set(CMAKE_SYSTEM_NAME Haiku)
set(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Haiku development headers and libraries
find_path(HAIKU_INCLUDE_DIR NAMES SupportDefs.h PATHS /boot/develop/headers /system/develop/headers)
find_library(HAIKU_BE_LIBRARY NAMES be PATHS /boot/develop/lib /system/develop/lib)
find_library(HAIKU_TRACKER_LIBRARY NAMES tracker PATHS /boot/develop/lib /system/develop/lib)

if(NOT HAIKU_INCLUDE_DIR OR NOT HAIKU_BE_LIBRARY)
    message(FATAL_ERROR "Haiku development headers and libraries not found. Please install Haiku development tools.")
endif()

include_directories(${HAIKU_INCLUDE_DIR})
link_directories(${CMAKE_BINARY_DIR})

# Enable warnings
add_compile_options(-Wall -Wextra -Wno-unused-parameter)

# Debug vs Release
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-g -O0)
    add_definitions(-DDEBUG -D_DEBUG)
else()
    add_compile_options(-O2 -DNDEBUG)
endif()

# Options
option(ENABLE_TESTS "Build unit tests" ON)
option(ENABLE_DOCS "Build documentation" OFF)
option(ENABLE_INSTALL "Install to system" ON)

# Set default install prefix for Haiku
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "/boot/common" CACHE PATH "Default install path" FORCE)
endif()

# Add subdirectories
add_subdirectory(libraries/YellowCommon)
add_subdirectory(libraries/YellowDocumentCore)
add_subdirectory(libraries/YellowDocumentLayout)
add_subdirectory(libraries/YellowImportExport)
add_subdirectory(libraries/YellowUI)
add_subdirectory(apps/YellowDocs)

# Packaging
if(ENABLE_INSTALL)
    add_subdirectory(packaging/hpkg)
endif()

# Documentation
if(ENABLE_DOCS)
    add_subdirectory(docs)
endif()
