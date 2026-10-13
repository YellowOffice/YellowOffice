#!/bin/bash
# Build YellowDocs and create a Haiku HPKG package.
#
# Run on Haiku. Requires: cmake, gcc/g++, rc, xres, package (all part of a
# normal Haiku development setup; zlib development headers for libz).
#
# Output: dist/YellowDocs-0.1.0-1-x86_64.hpkg

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(dirname "$SCRIPT_DIR")"
BUILD="$ROOT/build/haiku"
STAGE="$BUILD/yellowdocs-package"
DIST="$ROOT/dist"
VERSION="0.1.0-1"
ARCH="x86_64"

die() { echo "error: $*" >&2; exit 1; }

[ "$(uname -s)" = "Haiku" ] || die "this script must run on Haiku (needs rc, xres, package)"

for tool in cmake rc xres package; do
    command -v "$tool" >/dev/null 2>&1 || die "missing tool: $tool"
done

echo "== building (Release) =="
cmake -S "$ROOT" -B "$BUILD" -DCMAKE_BUILD_TYPE=Release
JOBS="$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)"
cmake --build "$BUILD" -j "$JOBS"

BINARY="$BUILD/apps/YellowDocs/YellowDocs"
[ -x "$BINARY" ] || die "built binary not found at $BINARY"

echo "== compiling resources =="
cd "$ROOT/apps/YellowDocs/resources"
rc -o "$BUILD/YellowDocs.rsrc" YellowDocs.rdef
xres -o "$BINARY" "$BINARY" "$BUILD/YellowDocs.rsrc"

echo "== staging package tree =="
rm -rf "$STAGE"
mkdir -p "$STAGE/bin" "$STAGE/documentation/YellowDocs"
cp "$BINARY" "$STAGE/bin/YellowDocs"
cp "$ROOT/packaging/hpkg/YellowDocs.PackageInfo" "$STAGE/.PackageInfo"
cp "$ROOT/LICENSE" "$STAGE/documentation/YellowDocs/"
[ -f "$ROOT/README.md" ] && cp "$ROOT/README.md" "$STAGE/documentation/YellowDocs/"

echo "== creating hpkg =="
mkdir -p "$DIST"
OUT="$DIST/YellowDocs-${VERSION}-${ARCH}.hpkg"
rm -f "$OUT"
package create -C "$STAGE" "$OUT"

echo "package written: $OUT"
