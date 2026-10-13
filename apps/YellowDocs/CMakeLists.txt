# YellowDocs - word processor application (Haiku-only)

set(APP_NAME YellowDocs)

set(SOURCES
    src/YellowDocsApp.cpp
    src/YellowDocsWindow.cpp
    src/DocumentCanvas.cpp
)

add_executable(${APP_NAME} ${SOURCES})

target_include_directories(${APP_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(${APP_NAME}
    YellowCommon
    YellowDocumentCore
    YellowDocumentLayout
    YellowImportExport
    YellowUI
    ${YELLOW_PLATFORM_LIBS}
)

set_target_properties(${APP_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/apps/YellowDocs
)

if(ENABLE_INSTALL AND YELLOW_ON_HAIKU)
    install(TARGETS ${APP_NAME} RUNTIME DESTINATION bin)
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/resources/YellowDocs.rdef)
        install(FILES resources/YellowDocs.rdef DESTINATION data/YellowDocs)
    endif()
    file(GLOB YELLOWDOCS_ICONS ${CMAKE_CURRENT_SOURCE_DIR}/resources/*.png)
    if(YELLOWDOCS_ICONS)
        install(FILES ${YELLOWDOCS_ICONS} DESTINATION data/YellowDocs)
    endif()
endif()
