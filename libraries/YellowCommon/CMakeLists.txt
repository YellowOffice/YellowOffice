# YellowCommon Library
# Shared utilities for YellowOffice suite

# Library name
set(LIBRARY_NAME YellowCommon)

# Source files
set(SOURCES
    src/Logger.cpp
    src/Error.cpp
)

# Headers
set(HEADERS
    include/YellowCommon/Defines.h
    include/YellowCommon/Logger.h
    include/YellowCommon/Error.h
)

# Include directories
set(INCLUDE_DIRS
    include
)

# Create the library
add_library(${LIBRARY_NAME} SHARED ${SOURCES} ${HEADERS})

# Set library version
target_compile_definitions(${LIBRARY_NAME} PRIVATE YELLOWCOMMON_BUILDING_DLL)

# Include directories
target_include_directories(${LIBRARY_NAME} 
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include/YellowCommon>
    PRIVATE
        ${HAIKU_INCLUDE_DIR}
)

# Link libraries
target_link_libraries(${LIBRARY_NAME}
    ${HAIKU_BE_LIBRARY}
)

# Output directory
set_target_properties(${LIBRARY_NAME} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    OUTPUT_NAME ${LIBRARY_NAME}
)

# Installation
install(TARGETS ${LIBRARY_NAME}
    LIBRARY DESTINATION lib/YellowOffice
    ARCHIVE DESTINATION lib/YellowOffice
)

install(DIRECTORY include/YellowCommon
    DESTINATION include
)
