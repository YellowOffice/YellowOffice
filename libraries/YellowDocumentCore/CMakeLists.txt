# YellowDocumentCore Library
# Core document model for YellowOffice suite

# Library name
set(LIBRARY_NAME YellowDocumentCore)

# Source files
set(SOURCES
    src/DocumentNode.cpp
    src/Style.cpp
    src/YellowDocument.cpp
)

# Headers
set(HEADERS
    include/YellowDocumentCore/DocumentNode.h
    include/YellowDocumentCore/Style.h
    include/YellowDocumentCore/YellowDocument.h
)

# Include directories
set(INCLUDE_DIRS
    include
    ../YellowCommon/include
)

# Create the library
add_library(${LIBRARY_NAME} SHARED ${SOURCES} ${HEADERS})

# Set library version
target_compile_definitions(${LIBRARY_NAME} PRIVATE YELLOWDOCUMENTCORE_BUILDING_DLL)

# Include directories
target_include_directories(${LIBRARY_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include/YellowDocumentCore>
    PRIVATE
        ${HAIKU_INCLUDE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../YellowCommon/include
)

# Link libraries
target_link_libraries(${LIBRARY_NAME}
    ${HAIKU_BE_LIBRARY}
    YellowCommon
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

install(DIRECTORY include/YellowDocumentCore
    DESTINATION include
)
