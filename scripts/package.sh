#!/bin/bash

# YellowOffice Packaging Script
# This script creates Haiku HPKG packages for the YellowOffice suite

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_ROOT/build"
PACKAGE_DIR="$PROJECT_ROOT/packaging"
OUTPUT_DIR="$PROJECT_ROOT/dist"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Functions
log_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

log_warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check if running on Haiku
check_haiku() {
    if [ "$(uname -s)" != "Haiku" ]; then
        log_warn "This script is designed to run on Haiku OS. Some features may not work on other platforms."
        log_warn "Continuing anyway..."
    fi
}

# Check for required tools
check_tools() {
    local missing_tools=()
    
    # Check for Haiku tools
    if ! command -v package &> /dev/null; then
        missing_tools+=("package")
    fi
    
    if ! command -v packagefs &> /dev/null; then
        missing_tools+=("packagefs")
    fi
    
    if ! command -v pkgman &> /dev/null; then
        missing_tools+=("pkgman")
    fi
    
    if ! command -v jam &> /dev/null; then
        missing_tools+=("jam")
    fi
    
    if [ ${#missing_tools[@]} -ne 0 ]; then
        log_error "Missing required tools: ${missing_tools[*]}"
        log_error "Please install Haiku development tools."
        exit 1
    fi
    
    log_info "All required tools are available."
}

# Build the project
build_project() {
    log_info "Building YellowOffice..."
    
    cd "$PROJECT_ROOT"
    
    # Create build directory if it doesn't exist
    mkdir -p "$BUILD_DIR"
    
    # Build using jam
    if [ -f "$PROJECT_ROOT/Jamfile" ]; then
        log_info "Building with Jam..."
        cd "$BUILD_DIR"
        jam -q -j$(nproc) || { log_error "Jam build failed"; exit 1; }
    else
        # Build using CMake
        log_info "Building with CMake..."
        cd "$BUILD_DIR"
        cmake .. -DCMAKE_BUILD_TYPE=Release || { log_error "CMake configuration failed"; exit 1; }
        make -j$(nproc) || { log_error "Make build failed"; exit 1; }
    fi
    
    log_info "Build completed successfully."
}

# Create package structure
create_package_structure() {
    log_info "Creating package structure..."
    
    # Create temporary package directories
    local temp_dir="$BUILD_DIR/package-temp"
    rm -rf "$temp_dir"
    mkdir -p "$temp_dir"
    
    # Create YellowDocs package structure
    local yellowdocs_pkg="$temp_dir/YellowDocs"
    mkdir -p "$yellowdocs_pkg/bin"
    mkdir -p "$yellowdocs_pkg/app/YellowDocs"
    mkdir -p "$yellowdocs_pkg/app/YellowDocs/resources"
    mkdir -p "$yellowdocs_pkg/data/mime/ydf"
    mkdir -p "$yellowdocs_pkg/data/mime/YellowDocs"
    mkdir -p "$yellowdocs_pkg/documentation/YellowDocs"
    mkdir -p "$yellowdocs_pkg/apps/YellowDocs"
    
    # Copy binaries
    if [ -f "$BUILD_DIR/apps/YellowDocs/YellowDocs" ]; then
        cp "$BUILD_DIR/apps/YellowDocs/YellowDocs" "$yellowdocs_pkg/bin/YellowDocs"
    elif [ -f "$BUILD_DIR/YellowDocs" ]; then
        cp "$BUILD_DIR/YellowDocs" "$yellowdocs_pkg/bin/YellowDocs"
    else
        log_error "YellowDocs binary not found in build directory"
        exit 1
    fi
    
    # Copy libraries
    mkdir -p "$yellowdocs_pkg/lib/YellowOffice"
    for lib in libYellowDocumentCore libYellowDocumentLayout libYellowImportExport libYellowUI libYellowCommon; do
        if [ -f "$BUILD_DIR/lib${lib}.so" ]; then
            cp "$BUILD_DIR/lib${lib}.so" "$yellowdocs_pkg/lib/YellowOffice/"
        fi
    done
    
    # Copy resources
    if [ -d "$PROJECT_ROOT/apps/YellowDocs/resources" ]; then
        cp -r "$PROJECT_ROOT/apps/YellowDocs/resources/"* "$yellowdocs_pkg/app/YellowDocs/resources/"
    fi
    
    # Copy MIME type files
    if [ -f "$PROJECT_ROOT/packaging/mime/ydf.mimetype" ]; then
        cp "$PROJECT_ROOT/packaging/mime/ydf.mimetype" "$yellowdocs_pkg/data/mime/ydf/"
    fi
    
    if [ -f "$PROJECT_ROOT/packaging/mime/YellowDocs.mimetype" ]; then
        cp "$PROJECT_ROOT/packaging/mime/YellowDocs.mimetype" "$yellowdocs_pkg/data/mime/YellowDocs/"
    fi
    
    # Copy documentation
    if [ -d "$PROJECT_ROOT/docs" ]; then
        cp -r "$PROJECT_ROOT/docs/"* "$yellowdocs_pkg/documentation/YellowDocs/"
    fi
    
    # Copy license
    cp "$PROJECT_ROOT/LICENSE" "$yellowdocs_pkg/documentation/YellowDocs/"
    
    # Copy package info
    cp "$PROJECT_ROOT/packaging/hpkg/YellowDocs.PackageInfo" "$yellowdocs_pkg/.PackageInfo"
    
    log_info "Package structure created."
    
    echo "$temp_dir"
}

# Create HPKG package
create_hpkg() {
    local temp_dir=$1
    
    log_info "Creating HPKG package..."
    
    # Create output directory
    mkdir -p "$OUTPUT_DIR"
    
    # Create YellowDocs package
    cd "$temp_dir/YellowDocs"
    
    # Use package command to create HPKG
    if command -v package &> /dev/null; then
        package create \
            -d "$OUTPUT_DIR" \
            --name YellowDocs \
            --version 0.1.0 \
            --architecture any \
            --vendor "YellowOffice Team" \
            --summary "YellowOffice word processor and document editor" \
            --description "YellowDocs is a full-featured word processor and document editor for Haiku OS." \
            --copyright "Copyright (c) 2024 YellowOffice Contributors" \
            --license MIT \
            --url https://github.com/YellowOffice/YellowOffice \
            . || { log_error "Package creation failed"; exit 1; }
    else
        log_error "package command not found"
        exit 1
    fi
    
    log_info "HPKG package created: $OUTPUT_DIR/YellowDocs-0.1.0-1-any.hpkg"
}

# Clean up
cleanup() {
    log_info "Cleaning up temporary files..."
    rm -rf "$BUILD_DIR/package-temp"
}

# Main function
main() {
    log_info "Starting YellowOffice packaging..."
    
    check_haiku
    check_tools
    
    # Parse command line arguments
    local build_only=false
    local clean=false
    
    for arg in "$@"; do
        case "$arg" in
            --build-only)
                build_only=true
                ;;
            --clean)
                clean=true
                ;;
            --help|-h)
                echo "Usage: $0 [options]"
                echo ""
                echo "Options:"
                echo "  --build-only    Only build, don't package"
                echo "  --clean        Clean before building"
                echo "  --help, -h     Show this help message"
                exit 0
                ;;
        esac
    done
    
    if [ "$clean" = true ]; then
        log_info "Cleaning build directory..."
        rm -rf "$BUILD_DIR"
    fi
    
    # Build the project
    build_project
    
    if [ "$build_only" = true ]; then
        log_info "Build only mode. Skipping packaging."
        exit 0
    fi
    
    # Create package structure and package
    local temp_dir=$(create_package_structure)
    create_hpkg "$temp_dir"
    cleanup
    
    log_info "Packaging completed successfully!"
    log_info "Package file: $OUTPUT_DIR/YellowDocs-0.1.0-1-any.hpkg"
}

# Run main function
main "$@"
