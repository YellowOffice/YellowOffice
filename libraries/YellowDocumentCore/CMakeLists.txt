# YellowDocumentCore - document model (nodes, styles, commands)

set(LIBRARY_NAME YellowDocumentCore)

set(SOURCES
    src/DocumentNode.cpp
    src/Style.cpp
    src/YellowDocument.cpp
    src/Nodes.cpp
    src/Command.cpp
)

add_library(${LIBRARY_NAME} STATIC ${SOURCES})

target_include_directories(${LIBRARY_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(${LIBRARY_NAME} PUBLIC YellowCommon)

if(YELLOW_ON_HAIKU)
    target_link_libraries(${LIBRARY_NAME} PUBLIC ${YELLOW_PLATFORM_LIBS})
endif()

set_target_properties(${LIBRARY_NAME} PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)

if(ENABLE_INSTALL AND YELLOW_ON_HAIKU)
    install(TARGETS ${LIBRARY_NAME} ARCHIVE DESTINATION lib/YellowOffice)
    install(DIRECTORY include/YellowDocumentCore DESTINATION include)
endif()
