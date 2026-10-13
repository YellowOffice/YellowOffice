# YellowDocumentLayout - layout engine (Haiku-only: needs the Be API)

set(LIBRARY_NAME YellowDocumentLayout)

set(SOURCES
    src/LayoutTypes.cpp
    src/LayoutEngine.cpp
    src/TextLayouter.cpp
    src/ParagraphLayouter.cpp
    src/PageLayouter.cpp
    src/SelectionManager.cpp
    src/CaretManager.cpp
    src/HitTester.cpp
    src/RenderContext.cpp
)

add_library(${LIBRARY_NAME} STATIC ${SOURCES})

target_include_directories(${LIBRARY_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(${LIBRARY_NAME}
    PUBLIC YellowCommon YellowDocumentCore ${YELLOW_PLATFORM_LIBS}
)

set_target_properties(${LIBRARY_NAME} PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)

if(ENABLE_INSTALL AND YELLOW_ON_HAIKU)
    install(TARGETS ${LIBRARY_NAME} ARCHIVE DESTINATION lib/YellowOffice)
    install(DIRECTORY include/YellowDocumentLayout DESTINATION include)
endif()
