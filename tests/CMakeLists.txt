# Host-side unit tests for the non-UI libraries.

add_executable(yellowoffice-tests
    test_main.cpp
    test_json.cpp
    test_core.cpp
    test_formats.cpp
)

target_link_libraries(yellowoffice-tests
    YellowCommon
    YellowDocumentCore
    YellowImportExport
    ZLIB::ZLIB
)

add_test(NAME yellowoffice-tests COMMAND yellowoffice-tests)
