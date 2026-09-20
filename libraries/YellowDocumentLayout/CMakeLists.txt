# YellowDocumentLayout Library
# Document layout engine for YellowOffice suite

# Library name
set(LIBRARY_NAME YellowDocumentLayout)

# Source files
set(SOURCES
    # Will be added when layout engine is implemented
)

# Headers
set(HEADERS
    include/YellowDocumentLayout/LayoutEngine.h
    include/YellowDocumentLayout/TextLayouter.h
    include/YellowDocumentLayout/ParagraphLayouter.h
    include/YellowDocumentLayout/PageLayouter.h
    include/YellowDocumentLayout/SelectionManager.h
    include/YellowDocumentLayout/CaretManager.h
    include/YellowDocumentLayout/HitTester.h
    include/YellowDocumentLayout/RenderContext.h
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
target_compile_definitions(${LIBRARY_NAME} PRIVATE YELLOWDOCUMENTLAYOUT_BUILDING_DLL)

# Include directories
target_include_directories(${LIBRARY_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include/YellowDocumentLayout>
    PRIVATE
        ${HAIKU_INCLUDE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../YellowCommon/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../YellowDocumentCore/include>
)

# Link libraries
target_link_libraries(${LIBRARY_NAME}
    ${HAIKU_BE_LIBRARY}
    YellowCommon
    YellowDocumentCore
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

install(DIRECTORY include/YellowDocumentLayout
    DESTINATION include
)
