# HPKG Packaging for YellowOffice
# This file creates the Haiku package files

# Find packagefs tool (Haiku's package creation tool)
find_program(PACKAGEFS_CMD packagefs)
find_program(PACKAGE_CMD package)

if(NOT PACKAGEFS_CMD AND NOT PACKAGE_CMD)
    message(WARNING "Haiku packaging tools not found. Package targets will not be available.")
    return()
endif()

# Package creation target
add_custom_target(yellowoffice-package
    COMMAND ${CMAKE_COMMAND} -E echo "Creating YellowOffice package..."
    COMMAND ${PACKAGE_CMD} create \
        -d ${CMAKE_BINARY_DIR}/yellowoffice-package \
        --name YellowOffice \
        --version 0.1.0 \
        --architecture any \
        --vendor "YellowOffice Team" \
        --summary "Native office productivity suite for Haiku OS" \
        --description "YellowOffice is a comprehensive office productivity suite designed specifically for Haiku OS." \
        --copyright "Copyright (c) 2024 YellowOffice Contributors" \
        --license MIT \
        --url https://github.com/YellowOffice/YellowOffice
    COMMENT "Creating YellowOffice HPKG package"
    VERBATIM
)

# YellowDocs package target
add_custom_target(yellowdocs-package
    COMMAND ${CMAKE_COMMAND} -E echo "Creating YellowDocs package..."
    COMMAND ${PACKAGE_CMD} create \
        -d ${CMAKE_BINARY_DIR}/yellowdocs-package \
        --name YellowDocs \
        --version 0.1.0 \
        --architecture any \
        --vendor "YellowOffice Team" \
        --summary "YellowOffice word processor and document editor" \
        --description "YellowDocs is a full-featured word processor and document editor for Haiku OS." \
        --copyright "Copyright (c) 2024 YellowOffice Contributors" \
        --license MIT \
        --url https://github.com/YellowOffice/YellowOffice
    COMMENT "Creating YellowDocs HPKG package"
    VERBATIM
)

# Install targets for packaging
install(DIRECTORY ${CMAKE_BINARY_DIR}/yellowoffice-package
    DESTINATION .
    COMPONENT package
)

install(DIRECTORY ${CMAKE_BINARY_DIR}/yellowdocs-package
    DESTINATION .
    COMPONENT package
)

# Package all target
add_custom_target(package-all
    DEPENDS yellowoffice-package yellowdocs-package
    COMMENT "Creating all YellowOffice packages"
)
