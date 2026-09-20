# YellowImportExport Library
# Import/export adapters for YellowOffice suite

# Library name
set(LIBRARY_NAME YellowImportExport)

# Source files
set(SOURCES
    # Will be added when import/export is implemented
)

# Headers
set(HEADERS
    include/YellowImportExport/FormatRegistry.h
    include/YellowImportExport/DocumentFormat.h
    include/YellowImportExport/Importer.h
    include/YellowImportExport/Exporter.h
    include/YellowImportExport/FormatCapabilities.h
    include/YellowImportExport/MarkdownImporter.h
    include/YellowImportExport/MarkdownExporter.h
    include/YellowImportExport/ODFImporter.h
    include/YellowImportExport/ODFExporter.h
    include/YellowImportExport/YDFImporter.h
    include/YellowImportExport/YDFExporter.h
)

# Include directories
set(INCLUDE_DIRS
    include
    ../YellowCommon/include
    ../YellowDocumentCore/include
)

# Create the library
add_library(${LIBRARY_NAME} SHARED ${SOURCES} ${HEADERS})

# Set library version
target_compile_definitions(${LIBRARY_NAME} PRIVATE YELLOWIMPORTEXPORT_BUILDING_DLL)

# Include directories
target_include_directories(${LIBRARY_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include/YellowImportExport>
    PRIVATE
        ${HAIKU_INCLUDE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../YellowCommon/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../YellowDocumentCore/include
)

# Link libraries
target_link_libraries(${LIBRARY_NAME}
    ${HAIKU_BE_LIBRARY}
    YellowCommon
    YellowDocumentCore
    z
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

install(DIRECTORY include/YellowImportExport
    DESTINATION include
)
