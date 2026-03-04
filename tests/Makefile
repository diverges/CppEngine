# Test Suite Makefile
# Uses Doctest framework for C++ unit testing

# Compiler and flags
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic -O2
DEBUG_FLAGS := -g -DDEBUG -O0
TEST_FLAGS := -DDOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

# Directories
UNIT_DIR := unit
INTEGRATION_DIR := integration
BUILD_DIR := build
ENGINE_DIR := ../engine

# Include paths
INCLUDE_DIRS := -I$(ENGINE_DIR)/include -I$(ENGINE_DIR)/deps/glm -I$(ENGINE_DIR)/deps/doctest -I$(ENGINE_DIR)/deps/SDL2/include -I$(ENGINE_DIR)/deps/glad/include

# Library paths for integration tests
LIBRARY_DIRS := -L$(ENGINE_DIR)/lib
LIBS := -lAIEngine -lSDL2 -lSDL2main -lopengl32 -lgdi32

# Source files
UNIT_SOURCES := $(wildcard $(UNIT_DIR)/*.cpp)
INTEGRATION_SOURCES := $(wildcard $(INTEGRATION_DIR)/*.cpp)

UNIT_OBJECTS := $(UNIT_SOURCES:$(UNIT_DIR)/%.cpp=$(BUILD_DIR)/unit/%.o)
INTEGRATION_OBJECTS := $(INTEGRATION_SOURCES:$(INTEGRATION_DIR)/%.cpp=$(BUILD_DIR)/integration/%.o)

# Test executables
UNIT_TARGETS := $(UNIT_SOURCES:$(UNIT_DIR)/%.cpp=$(BUILD_DIR)/unit/%)
INTEGRATION_TARGETS := $(INTEGRATION_SOURCES:$(INTEGRATION_DIR)/%.cpp=$(BUILD_DIR)/integration/%)

# Default target
all: unit integration

# Create build directories
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BUILD_DIR)/unit
	mkdir -p $(BUILD_DIR)/integration

# Check dependencies
check-doctest:
	@test -f $(ENGINE_DIR)/deps/doctest/doctest.h || (echo "ERROR: Doctest header not found at $(ENGINE_DIR)/deps/doctest/doctest.h"; exit 1)
	@echo "✓ Doctest framework found"

check-engine:
	@test -f $(ENGINE_DIR)/lib/libAIEngine.a || (echo "ERROR: Engine library not found. Run 'make' in engine/ directory first."; exit 1)
	@echo "✓ Engine library found"

# Build unit tests (header-only, no engine linking needed)
$(BUILD_DIR)/unit/%: $(UNIT_DIR)/%.cpp | $(BUILD_DIR) check-doctest
	$(CXX) $(CXXFLAGS) $(TEST_FLAGS) $(INCLUDE_DIRS) $< -o $@

# Build integration tests (requires engine library)
$(BUILD_DIR)/integration/%: $(INTEGRATION_DIR)/%.cpp | $(BUILD_DIR) check-engine
	$(CXX) $(CXXFLAGS) $(TEST_FLAGS) $(INCLUDE_DIRS) $(LIBRARY_DIRS) $< $(LIBS) -o $@

# Build all unit tests
unit: $(UNIT_TARGETS)
	@echo "Built unit tests: $(UNIT_TARGETS)"

# Build all integration tests  
integration: $(INTEGRATION_TARGETS)
	@echo "Built integration tests: $(INTEGRATION_TARGETS)"

# Run unit tests
run-unit: unit
	@echo "Running unit tests..."
	@for test in $(UNIT_TARGETS); do \
		echo "Running $$test..."; \
		./$$test || exit 1; \
	done
	@echo "✓ All unit tests passed"

# Run integration tests
run-integration: integration
	@echo "Running integration tests..."
	@for test in $(INTEGRATION_TARGETS); do \
		echo "Running $$test..."; \
		./$$test || exit 1; \
	done
	@echo "✓ All integration tests passed"

# Run all tests
run-all: run-unit run-integration
	@echo "✓ All tests completed successfully"

# Create sample unit test if none exist
sample-unit:
	@if [ ! -f $(UNIT_DIR)/sample_test.cpp ]; then \
		mkdir -p $(UNIT_DIR); \
		echo '#include "doctest.h"' > $(UNIT_DIR)/sample_test.cpp; \
		echo '' >> $(UNIT_DIR)/sample_test.cpp; \
		echo 'TEST_CASE("Sample test") {' >> $(UNIT_DIR)/sample_test.cpp; \
		echo '    CHECK(1 + 1 == 2);' >> $(UNIT_DIR)/sample_test.cpp; \
		echo '}' >> $(UNIT_DIR)/sample_test.cpp; \
		echo "Created sample unit test"; \
	fi

# Create sample integration test if none exist  
sample-integration:
	@if [ ! -f $(INTEGRATION_DIR)/sample_integration_test.cpp ]; then \
		mkdir -p $(INTEGRATION_DIR); \
		echo '#include "doctest.h"' > $(INTEGRATION_DIR)/sample_integration_test.cpp; \
		echo '#include <AIEngine/AIEngine.hpp>' >> $(INTEGRATION_DIR)/sample_integration_test.cpp; \
		echo '' >> $(INTEGRATION_DIR)/sample_integration_test.cpp; \
		echo 'TEST_CASE("Engine integration test") {' >> $(INTEGRATION_DIR)/sample_integration_test.cpp; \
		echo '    // Basic integration test placeholder' >> $(INTEGRATION_DIR)/sample_integration_test.cpp; \
		echo '    CHECK(true);' >> $(INTEGRATION_DIR)/sample_integration_test.cpp; \
		echo '}' >> $(INTEGRATION_DIR)/sample_integration_test.cpp; \
		echo "Created sample integration test"; \
	fi

# Debug builds
debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: all

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)
	@echo "Cleaned test build artifacts"

# Setup test environment
setup: check-doctest sample-unit sample-integration
	@echo "✓ Test environment ready"

# Continuous testing (watch for changes)
watch:
	@echo "Watching for changes... (Ctrl+C to stop)"
	while true; do \
		$(MAKE) run-all; \
		sleep 2; \
	done

# Print configuration
info:
	@echo "Compiler: $(CXX)"
	@echo "Flags: $(CXXFLAGS) $(TEST_FLAGS)"
	@echo "Unit tests: $(UNIT_SOURCES)"
	@echo "Integration tests: $(INTEGRATION_SOURCES)"
	@echo "Include dirs: $(INCLUDE_DIRS)"

.PHONY: all unit integration run-unit run-integration run-all sample-unit sample-integration debug clean setup watch info check-doctest check-engine