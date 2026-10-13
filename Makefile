# Convenience wrapper around CMake for host (non-Haiku) development:
# builds the core libraries and runs the unit tests against the Haiku API
# stubs in build/haiku-stub. On Haiku, use CMake directly instead.
#
#   make            configure + build
#   make test       build + run unit tests
#   make syntax     run the stub syntax check over all project sources
#   make clean      remove the host build directory

BUILD_DIR ?= build/host-build

.PHONY: all configure test syntax clean

all: configure
	cmake --build $(BUILD_DIR) -j 4

configure:
	cmake -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Debug

test: all
	./$(BUILD_DIR)/tests/yellowoffice-tests

syntax:
	./scripts/syntax-check.sh

clean:
	rm -rf $(BUILD_DIR)
