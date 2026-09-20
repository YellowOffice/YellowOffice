# YellowDocs Application
# Word processor for YellowOffice suite

# Application name
set(APP_NAME YellowDocs)

# Source files
set(SOURCES
    src/YellowDocsApp.cpp
    src/YellowDocsWindow.cpp
)

# Headers
set(HEADERS
    src/YellowDocsApp.h
    src/YellowDocsWindow.h
)

# Resources
set(RESOURCES
    resources/app_icon_16.png
    resources/app_icon_32.png
    resources/app_icon_64.png
    resources/app_icon_128.png
    resources/app_icon_256.png
    resources/document_icon_16.png
    resources/document_icon_32.png
    resources/document_icon_64.png
)

# Include directories
set(INCLUDE_DIRS
    src
    ../../libraries/YellowCommon/include
    ../../libraries/YellowDocumentCore/include
    ../../libraries/YellowDocumentLayout/include
    ../../libraries/YellowUI/include
    ../../libraries/YellowImportExport/include
)

# Create the application
add_executable(${APP_NAME} ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(${APP_NAME}
    PRIVATE
        ${HAIKU_INCLUDE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/../../libraries/YellowCommon/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../libraries/YellowDocumentCore/include
)

# Link libraries
target_link_libraries(${APP_NAME}
    ${HAIKU_BE_LIBRARY}
    ${HAIKU_TRACKER_LIBRARY}
    YellowCommon
    YellowDocumentCore
    YellowDocumentLayout
    YellowUI
    YellowImportExport
)

# Output directory
set_target_properties(${APP_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/apps/YellowDocs
    OUTPUT_NAME ${APP_NAME}
)

# Installation
install(TARGETS ${APP_NAME}
    RUNTIME DESTINATION bin
)

# Install resources
install(DIRECTORY resources/
    DESTINATION app/YellowDocs
)
