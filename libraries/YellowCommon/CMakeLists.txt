# YellowCommon - shared utilities (logging, JSON, errors)

set(LIBRARY_NAME YellowCommon)

set(SOURCES
    src/Logger.cpp
    src/Error.cpp
    src/Json.cpp
)

add_library(${LIBRARY_NAME} STATIC ${SOURCES})

target_include_directories(${LIBRARY_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

if(YELLOW_ON_HAIKU)
    target_link_libraries(${LIBRARY_NAME} PUBLIC ${YELLOW_PLATFORM_LIBS})
endif()

set_target_properties(${LIBRARY_NAME} PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)

if(ENABLE_INSTALL AND YELLOW_ON_HAIKU)
    install(TARGETS ${LIBRARY_NAME} ARCHIVE DESTINATION lib/YellowOffice)
    install(DIRECTORY include/YellowCommon DESTINATION include)
endif()
