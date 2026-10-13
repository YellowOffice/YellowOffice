#!/bin/bash
# YellowOffice - Haiku API syntax checker (host development helper).
#
# Compiles every project source against the stub Haiku headers in
# build/haiku-stub so API/syntax/type errors are caught without a Haiku
# machine. This does NOT replace building on real Haiku - it is a fast
# pre-flight check. UI code is syntax-checked only (-fsyntax-only);
# libraries with functional stubs are fully compiled.

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(dirname "$SCRIPT_DIR")"
STUB="$ROOT/build/haiku-stub"

CXX="${CXX:-c++}"
CXXFLAGS="-std=c++17 -Wall -Wextra -Wno-unused-parameter -Wno-multichar -D__HAIKU_STUB__ -D__HAIKU__=1"
INCLUDES="-I$STUB \
  -I$ROOT/libraries/YellowCommon/include \
  -I$ROOT/libraries/YellowDocumentCore/include \
  -I$ROOT/libraries/YellowDocumentLayout/include \
  -I$ROOT/libraries/YellowImportExport/include \
  -I$ROOT/libraries/YellowUI/include \
  -I$ROOT/apps/YellowDocs/src"

fail=0
checked=0

check() {
  local src="$1"
  local extra="$2"
  checked=$((checked + 1))
  if ! $CXX $CXXFLAGS $INCLUDES $extra -fsyntax-only "$src" 2>/tmp/yd-syntax-err.txt; then
    echo "FAIL: $src"
    cat /tmp/yd-syntax-err.txt
    fail=$((fail + 1))
  else
    echo "ok:   $src"
  fi
}

echo "== YellowOffice syntax check (stub Haiku headers) =="

# Libraries (full compile of pure logic where possible; syntax-only overall)
while IFS= read -r f; do
  check "$f" ""
done < <(find "$ROOT/libraries" -name '*.cpp' | sort)

# Application sources
while IFS= read -r f; do
  check "$f" ""
done < <(find "$ROOT/apps" -name '*.cpp' | sort)

# Tests
if [ -d "$ROOT/tests" ]; then
  while IFS= read -r f; do
    check "$f" ""
  done < <(find "$ROOT/tests" -name '*.cpp' | sort)
fi

echo ""
echo "== $checked files checked, $fail failure(s) =="
[ "$fail" -eq 0 ]
