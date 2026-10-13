# YellowImportExport - file formats (YDF, Markdown, PlainText, ODF)

set(LIBRARY_NAME YellowImportExport)

set(SOURCES
    src/Importer.cpp
    src/Exporter.cpp
    src/DocumentFormat.cpp
    src/FormatCapabilities.cpp
    src/FormatRegistry.cpp
    src/DocumentSerializer.cpp
    src/ZipArchive.cpp
    src/Xml.cpp
    src/YDFImporter.cpp
    src/YDFExporter.cpp
    src/MarkdownImporter.cpp
    src/MarkdownExporter.cpp
    src/PlainText.cpp
    src/ODFImporter.cpp
    src/ODFExporter.cpp
)

add_library(${LIBRARY_NAME} STATIC ${SOURCES})

target_include_directories(${LIBRARY_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(${LIBRARY_NAME}
    PUBLIC YellowCommon YellowDocumentCore ZLIB::ZLIB
)

if(YELLOW_ON_HAIKU)
    target_link_libraries(${LIBRARY_NAME} PUBLIC ${YELLOW_PLATFORM_LIBS})
endif()

set_target_properties(${LIBRARY_NAME} PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)

if(ENABLE_INSTALL AND YELLOW_ON_HAIKU)
    install(TARGETS ${LIBRARY_NAME} ARCHIVE DESTINATION lib/YellowOffice)
    install(DIRECTORY include/YellowImportExport DESTINATION include)
endif()
