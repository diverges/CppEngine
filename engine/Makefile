# AIEngine Library Makefile
# C++17 Game Engine with Modern Practices

# Compiler and flags
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic -O2

# Installation prefix
INSTALL_PREFIX := /usr/local
ifdef WINDIR
    INSTALL_PREFIX := C:/AIEngine
endif

# Detect MinGW system libraries (MSYS2)
MSYS2_ROOT := C:/msys64/mingw64
ifneq (,$(wildcard $(MSYS2_ROOT)/include))
    INCLUDE_DIRS := -Iinclude -I$(MSYS2_ROOT)/include
    LIB_DIRS := -L$(MSYS2_ROOT)/lib
    LIBS := -lSDL2main -lSDL2 -lopengl32 -lglew32
else
    # Fallback to local deps
    INCLUDE_DIRS := -Iinclude -Ideps -Ideps/SDL2/include -Ideps/glad/include
    LIB_DIRS := 
    LIBS := 
endif

DEBUG_FLAGS := -g -DDEBUG -O0

# Directories
SRC_DIR := src
INCLUDE_DIR := include
LIB_DIR := lib
BUILD_DIR := build
DEPS_DIR := deps

# Source files (explicitly listed for Windows compatibility)
SOURCES := src/core/Engine.cpp \
           src/scene/SceneNode.cpp \
           src/scene/SceneGraph.cpp \
           src/math/Transform.cpp \
           src/components/TransformComponent.cpp \
           src/components/RenderComponent.cpp \
           src/graphics/Renderer.cpp \
           src/graphics/Mesh.cpp \
           src/graphics/GeometryFactory.cpp \
           src/graphics/Shader.cpp \
           src/platform/Window.cpp

OBJECTS := $(SOURCES:src/%.cpp=$(BUILD_DIR)/%.o) 
DEPS := $(OBJECTS:.o=.d)

# Target library
TARGET := $(LIB_DIR)/libAIEngine.a

# Default target
all: $(TARGET)

# Create directories (Windows-compatible)
$(BUILD_DIR):
ifeq ($(OS),Windows_NT)
	@if not exist "$(BUILD_DIR)" mkdir "$(BUILD_DIR)"
	@if not exist "$(BUILD_DIR)\\core" mkdir "$(BUILD_DIR)\\core"
	@if not exist "$(BUILD_DIR)\\graphics" mkdir "$(BUILD_DIR)\\graphics"
	@if not exist "$(BUILD_DIR)\\scene" mkdir "$(BUILD_DIR)\\scene"
	@if not exist "$(BUILD_DIR)\\components" mkdir "$(BUILD_DIR)\\components"
	@if not exist "$(BUILD_DIR)\\math" mkdir "$(BUILD_DIR)\\math"
	@if not exist "$(BUILD_DIR)\\platform" mkdir "$(BUILD_DIR)\\platform"
else
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BUILD_DIR)/core
	mkdir -p $(BUILD_DIR)/graphics
	mkdir -p $(BUILD_DIR)/scene
	mkdir -p $(BUILD_DIR)/components
	mkdir -p $(BUILD_DIR)/math
	mkdir -p $(BUILD_DIR)/platform
endif

$(LIB_DIR):
ifeq ($(OS),Windows_NT)
	@if not exist "$(LIB_DIR)" mkdir "$(LIB_DIR)"
else
	mkdir -p $(LIB_DIR)
endif

# Build static library
$(TARGET): $(LIB_DIR) $(OBJECTS)
	ar rcs $(TARGET) $(OBJECTS)
	@echo "Built engine library: $(TARGET)"

# Compile source files with dependency generation
$(BUILD_DIR)/%.o: src/%.cpp | $(BUILD_DIR)
ifeq ($(OS),Windows_NT)
	@if not exist "$(dir $@)" mkdir "$(dir $@)"
else
	@mkdir -p $(dir $@)
endif
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) -MMD -MP -c $< -o $@

# Include dependencies
-include $(DEPS)

# Debug build
debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: $(TARGET)

# Release build (default optimization)
release: CXXFLAGS += -DNDEBUG
release: $(TARGET)

# Clean build artifacts
clean:
ifeq ($(OS),Windows_NT)
	@if exist "$(BUILD_DIR)" rmdir /s /q "$(BUILD_DIR)"
	@if exist "$(LIB_DIR)" rmdir /s /q "$(LIB_DIR)"
else
	rm -rf $(BUILD_DIR) $(LIB_DIR)
endif
	@echo "Cleaned build artifacts"

# Install headers and library (for development)
install: $(TARGET)
ifdef WINDIR
	@echo "Installing to $(INSTALL_PREFIX)..."
	@if not exist "$(INSTALL_PREFIX)\\include\\AIEngine" mkdir "$(INSTALL_PREFIX)\\include\\AIEngine"
	xcopy "$(INCLUDE_DIR)\\AIEngine\\*" "$(INSTALL_PREFIX)\\include\\AIEngine\\" /E /Y /Q
	@if not exist "$(INSTALL_PREFIX)\\lib" mkdir "$(INSTALL_PREFIX)\\lib"
	copy "$(TARGET)" "$(INSTALL_PREFIX)\\lib\\"
else
	mkdir -p $(INSTALL_PREFIX)/include/AIEngine
	cp -r $(INCLUDE_DIR)/AIEngine/* $(INSTALL_PREFIX)/include/AIEngine/
	mkdir -p $(INSTALL_PREFIX)/lib
	cp $(TARGET) $(INSTALL_PREFIX)/lib/
endif
	@echo "Installed AIEngine library and headers"

# Uninstall
uninstall:
ifdef WINDIR
	@if exist "$(INSTALL_PREFIX)\\include\\AIEngine" rmdir /s /q "$(INSTALL_PREFIX)\\include\\AIEngine"
	@if exist "$(INSTALL_PREFIX)\\lib\\libAIEngine.a" del "$(INSTALL_PREFIX)\\lib\\libAIEngine.a"
else
	rm -rf $(INSTALL_PREFIX)/include/AIEngine
	rm -f $(INSTALL_PREFIX)/lib/libAIEngine.a
endif
	@echo "Uninstalled AIEngine"

# Development targets
test: $(TARGET)
	@echo "Building and running unit tests..."
	$(MAKE) -C tests unit

# Code analysis and formatting targets
analyze: $(SOURCES)
	@echo "Running static analysis..."
	@command -v cppcheck >/dev/null 2>&1 || { echo "Warning: cppcheck not found, install with: pacman -S cppcheck"; exit 0; }
	cppcheck --enable=all --error-exitcode=1 --inline-suppr \
		--suppress=missingInclude --suppress=unmatchedSuppression \
		$(INCLUDE_DIRS) --check-config $(SOURCES) 2>&1 | tee analysis.log
	@echo "Static analysis complete. Report: analysis.log"

format: 
	@echo "Formatting code..."
	@command -v clang-format >/dev/null 2>&1 || { echo "Warning: clang-format not found, install with: pacman -S clang"; exit 0; }
	find $(SRC_DIR) $(INCLUDE_DIR) -name "*.cpp" -o -name "*.hpp" | \
		xargs clang-format -i -style="{BasedOnStyle: Google, IndentWidth: 4, ColumnLimit: 100}"
	@echo "Code formatting complete"

format-check:
	@echo "Checking code formatting..."
	@command -v clang-format >/dev/null 2>&1 || { echo "Warning: clang-format not found"; exit 0; }
	@format_issues=0; \
	for file in $$(find $(SRC_DIR) $(INCLUDE_DIR) -name "*.cpp" -o -name "*.hpp"); do \
		if ! clang-format -style="{BasedOnStyle: Google, IndentWidth: 4, ColumnLimit: 100}" "$$file" | diff -u "$$file" - >/dev/null; then \
			echo "❌ Format issue: $$file"; \
			format_issues=1; \
		fi; \
	done; \
	if [ $$format_issues -eq 0 ]; then \
		echo "✅ All files properly formatted"; \
	else \
		echo "❌ Format issues found. Run 'make format' to fix."; \
		exit 1; \
	fi

# Profiling build
profile: CXXFLAGS += -pg -O2 -DNDEBUG
profile: $(TARGET)
	@echo "Built profiling version. Use 'gprof' to analyze execution."

# Coverage build (requires gcov)
coverage: CXXFLAGS += -g -O0 --coverage
coverage: LDFLAGS += --coverage
coverage: $(TARGET)
	@echo "Built with coverage instrumentation. Run tests then 'make coverage-report'."

coverage-report:
	@echo "Generating coverage report..."
	@command -v gcov >/dev/null 2>&1 || { echo "Warning: gcov not found"; exit 0; }
	mkdir -p coverage
	gcov -r $(OBJECTS) --object-directory $(BUILD_DIR)
	@command -v lcov >/dev/null 2>&1 && { \
		lcov --capture --directory . --output-file coverage/coverage.info; \
		genhtml coverage/coverage.info --output-directory coverage/html; \
		echo "Coverage report: coverage/html/index.html"; \
	} || echo "Install lcov for HTML coverage reports"

# Documentation generation
docs:
	@echo "Generating documentation..."
	@command -v doxygen >/dev/null 2>&1 || { echo "Warning: doxygen not found, install with: pacman -S doxygen"; exit 0; }
	doxygen Doxyfile 2>/dev/null || echo "Doxyfile not found - creating basic documentation"
	@echo "Documentation generated in docs/html/"

# Memory leak detection (Windows: Dr. Memory, Linux: Valgrind)
memcheck: debug
	@echo "Running memory leak detection..."
ifdef WINDIR
	@command -v drmemory.exe >/dev/null 2>&1 || { echo "Install Dr. Memory for Windows memory checking"; exit 0; }
	drmemory.exe -- ../testgame/bin/testgame.exe
else
	@command -v valgrind >/dev/null 2>&1 || { echo "Install valgrind for memory checking"; exit 0; }
	cd ../testgame && valgrind --leak-check=full --show-leak-kinds=all ./bin/testgame
endif

# Performance benchmarking
benchmark: release
	@echo "Running performance benchmarks..."
	cd ../testgame && make release && ./bin/testgame --benchmark 2>&1 | tee ../engine/benchmark.log
	@echo "Benchmark results saved to benchmark.log"

# Full validation suite
validate: format-check analyze test coverage-report
	@echo "✅ Full validation complete"

# Development environment setup
setup-dev:
	@echo "Setting up development environment..."
ifdef WINDIR
	@echo "Install MSYS2 packages for enhanced development:"
	@echo "  pacman -S mingw-w64-x86_64-cppcheck"
	@echo "  pacman -S mingw-w64-x86_64-clang" 
	@echo "  pacman -S mingw-w64-x86_64-doxygen"
	@echo "  pacman -S mingw-w64-x86_64-gdb"
else
	@echo "Install development packages:"
	@echo "  sudo apt install cppcheck clang-format doxygen valgrind lcov"
endif
	@echo "Development environment info printed."

# Print configuration
info:
	@echo "=== AIEngine Build Configuration ==="
	@echo "OS: $(OS)"
	@echo "Compiler: $(CXX)"
	@echo "Flags: $(CXXFLAGS)"
	@echo "Include dirs: $(INCLUDE_DIRS)"
	@echo "Library dirs: $(LIB_DIRS)"
	@echo "Libraries: $(LIBS)"
	@echo "Sources: $(SOURCES)"
	@echo "Target: $(TARGET)"
	@echo "Install prefix: $(INSTALL_PREFIX)"
	@echo "=================================="

# Verify build environment
verify:
	@echo "Verifying build environment..."
	@$(CXX) --version || (echo "ERROR: C++ compiler not found" && exit 1)
	@echo "+ C++ compiler found"
ifeq ($(OS),Windows_NT)
	@if not exist "$(INCLUDE_DIR)" (echo "ERROR: Include directory missing" && exit 1)
else
	@test -d $(INCLUDE_DIR) || (echo "ERROR: Include directory missing" && exit 1)
endif
	@echo "+ Include directory exists"
	@echo "Build environment verified"

.PHONY: all debug release clean install uninstall test analyze format format-check \
        profile coverage coverage-report docs memcheck benchmark validate setup-dev info verify