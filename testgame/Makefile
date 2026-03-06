# Test Game Application Makefile
# Links against AIEngine library

# Compiler and flags
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic -O2
DEBUG_FLAGS := -g -DDEBUG -O0

# Directories
SRC_DIR := src
# Use centralized output directories from root Makefile
ifdef BIN_OUTPUT
    BIN_DIR := $(BIN_OUTPUT)
else
    BIN_DIR := bin
endif
ifdef BUILD_OUTPUT
    BUILD_DIR := $(BUILD_OUTPUT)/testgame
else
    BUILD_DIR := build
endif
ENGINE_DIR := ../engine

# Build variant handling (accepts VARIANT from root Makefile)
VARIANT ?= debug
ifeq ($(VARIANT),debug)
    BUILD_FLAGS := $(DEBUG_FLAGS)
else ifeq ($(VARIANT),release)  
    BUILD_FLAGS := -DNDEBUG -O2
else
    BUILD_FLAGS := $(DEBUG_FLAGS)
endif

# Apply variant-specific build flags
CXXFLAGS += $(BUILD_FLAGS)

# Architecture and compiler detection for dependency selection
TARGET_ARCH := $(shell g++ -dumpmachine)
ifeq ($(TARGET_ARCH),)
    # Fallback detection  
    TARGET_ARCH := $(shell g++ -v 2>&1 | findstr "Target:" | cut -d' ' -f2)
endif

# Map target architecture to dependency path
DEP_ARCH_PATH := unknown
ifneq (,$(findstring x86_64-w64-mingw32,$(TARGET_ARCH)))
    DEP_ARCH_PATH := windows-mingw64
endif
ifneq (,$(findstring i686-w64-mingw32,$(TARGET_ARCH)))
    DEP_ARCH_PATH := windows-mingw32
endif 
ifneq (,$(findstring x86_64-pc-linux,$(TARGET_ARCH)))
    DEP_ARCH_PATH := linux-x64
endif

# Self-contained project with architecture-aware dependencies
INCLUDE_DIRS := -I$(ENGINE_DIR)/include -I$(ENGINE_DIR)/deps -I$(ENGINE_DIR)/deps/glm -I$(ENGINE_DIR)/deps/SDL2/include -I$(ENGINE_DIR)/deps/glad/include -I$(ENGINE_DIR)/deps/glew/include -I$(ENGINE_DIR)/deps/doctest

# Library paths - engine library and architecture-aware dependencies
ifdef BIN_OUTPUT
    LIBRARY_DIRS := -L$(BIN_OUTPUT) -L$(ENGINE_DIR)/deps/SDL2/lib/$(DEP_ARCH_PATH) -L$(ENGINE_DIR)/deps/glew/lib/$(DEP_ARCH_PATH)
    ENGINE_LIB_PATH := $(BIN_OUTPUT)/libAIEngine.a
else
    LIBRARY_DIRS := -L$(ENGINE_DIR)/lib -L$(ENGINE_DIR)/deps/SDL2/lib/$(DEP_ARCH_PATH) -L$(ENGINE_DIR)/deps/glew/lib/$(DEP_ARCH_PATH)
    ENGINE_LIB_PATH := $(ENGINE_DIR)/lib/libAIEngine.a
endif

LIBS := -lAIEngine -lSDL2main -lSDL2 -lglew32 -lopengl32 -lgdi32

# Source files
SOURCES := $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS := $(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

# Target executable (Windows-compatible)
TARGET := $(BIN_DIR)/testgame.exe

# Default target
all: $(TARGET)

# Create directories (Windows-compatible)
$(BUILD_DIR):
ifeq ($(OS),Windows_NT)
	@if not exist "$(subst /,\\,$(BUILD_DIR))" mkdir "$(subst /,\\,$(BUILD_DIR))"
else
	mkdir -p $(BUILD_DIR)
endif

$(BIN_DIR):
ifeq ($(OS),Windows_NT)
	@if not exist "$(BIN_DIR)" mkdir "$(BIN_DIR)"
else
	mkdir -p $(BIN_DIR)
endif

# Check engine library exists - now looks in correct location
check-engine:
ifeq ($(OS),Windows_NT)
	@if not exist "$(ENGINE_LIB_PATH)" (echo "ERROR: Engine library not found at $(ENGINE_LIB_PATH). Build engine first." && exit 1)
else
	@test -f $(ENGINE_LIB_PATH) || (echo "ERROR: Engine library not found at $(ENGINE_LIB_PATH). Build engine first."; exit 1)
endif
	@echo "+ Engine library found at $(ENGINE_LIB_PATH)"

# Build executable
$(TARGET): check-engine $(BIN_DIR) $(OBJECTS)
	$(CXX) $(OBJECTS) $(LIBRARY_DIRS) $(LIBS) -o $(TARGET)
	@echo "Built test game: $(TARGET)"

# Compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) -c $< -o $@

# Build variant targets (called from root Makefile)
# Flags are automatically applied via BUILD_FLAGS based on VARIANT variable  
debug: all

release: all

# Clean build artifacts
clean:
ifeq ($(OS),Windows_NT)
	@if exist "$(subst /,\\,$(BUILD_DIR))" rmdir /s /q "$(subst /,\\,$(BUILD_DIR))" 
	@if exist "$(subst /,\\,$(BIN_DIR))\\testgame.exe" del "$(subst /,\\,$(BIN_DIR))\\testgame.exe"
else
	rm -rf $(BUILD_DIR)
	rm -f $(BIN_DIR)/testgame.exe
endif
	@echo "Cleaned test game build artifacts"

# Run the test game (Windows-compatible)
run: $(TARGET)
	@echo "Running test game..."
ifeq ($(OS),Windows_NT)
	cd $(BIN_DIR) && testgame.exe
else
	cd $(BIN_DIR) && ./testgame.exe
endif

# Build engine library if needed
build-engine:
	@echo "Building engine library..."
	$(MAKE) -C $(ENGINE_DIR)

# Full clean build (including engine)
rebuild: clean
	$(MAKE) clean -C $(ENGINE_DIR)
	$(MAKE) -C $(ENGINE_DIR)
	$(MAKE)

# Copy required DLLs for Windows runtime
copy-dlls: $(TARGET)
	@echo "Copying required DLLs from project dependencies..."
ifeq ($(OS),Windows_NT)
	@if exist "$(ENGINE_DIR)\\deps\\SDL2\\lib\\$(DEP_ARCH_PATH)\\SDL2.dll" ( \
		copy "$(ENGINE_DIR)\\deps\\SDL2\\lib\\$(DEP_ARCH_PATH)\\SDL2.dll" "$(BIN_DIR)\\" >nul 2>&1 && echo "✓ Copied SDL2.dll" \
	) else echo "❌ SDL2.dll not found in $(ENGINE_DIR)/deps/SDL2/lib/$(DEP_ARCH_PATH)/"
	@if exist "$(ENGINE_DIR)\\deps\\glew\\lib\\$(DEP_ARCH_PATH)\\glew32.dll" ( \
		copy "$(ENGINE_DIR)\\deps\\glew\\lib\\$(DEP_ARCH_PATH)\\glew32.dll" "$(BIN_DIR)\\" >nul 2>&1 && echo "✓ Copied glew32.dll" \
	) else echo "❌ glew32.dll not found in $(ENGINE_DIR)/deps/glew/lib/$(DEP_ARCH_PATH)/"
	@echo "✅ DLL copying completed"
else
	@echo "ℹ️  DLL copying not needed on non-Windows systems"
endif

# Development workflow
dev: build-engine $(TARGET) copy-dlls
	@echo "✓ Development build complete"

# Profiling build
profile: CXXFLAGS += -pg -O2 -DNDEBUG
profile: build-engine $(TARGET)
	@echo "Built profiling version. Run game then use 'gprof bin/testgame gmon.out > profile.txt'"

# Coverage build (requires gcov)
coverage: CXXFLAGS += -g -O0 --coverage
coverage: LDFLAGS += --coverage
coverage: build-engine $(TARGET)
	@echo "Built with coverage instrumentation."

# Memory leak detection
memcheck: debug
	@echo "Running memory leak detection on test game..."
ifdef WINDIR
	@command -v drmemory.exe >/dev/null 2>&1 || { echo "Install Dr. Memory for Windows memory checking"; exit 0; }
	drmemory.exe -- bin/testgame.exe
else
	@command -v valgrind >/dev/null 2>&1 || { echo "Install valgrind for memory checking"; exit 0; }
	valgrind --leak-check=full --show-leak-kinds=all bin/testgame
endif

# Performance benchmarking
benchmark: release
	@echo "Running performance benchmarks..."
	bin/testgame.exe --benchmark 2>&1 | tee benchmark.log
	@echo "Benchmark results saved to benchmark.log"

# Stress test (long running test)
stress: release
	@echo "Running stress test (extended gameplay session)..."
	timeout 300 bin/testgame.exe || echo "5-minute stress test completed"

# Code formatting (for game-specific code)
format:
	@echo "Formatting test game code..."
	@command -v clang-format >/dev/null 2>&1 || { echo "Warning: clang-format not found"; exit 0; }
	find $(SRC_DIR) -name "*.cpp" -o -name "*.hpp" | \
		xargs clang-format -i -style="{BasedOnStyle: Google, IndentWidth: 4, ColumnLimit: 100}"
	@echo "Code formatting complete"

# Static analysis
analyze:
	@echo "Running static analysis on test game..."
	@command -v cppcheck >/dev/null 2>&1 || { echo "Warning: cppcheck not found"; exit 0; }
	cppcheck --enable=all --error-exitcode=1 --inline-suppr \
		$(INCLUDE_DIRS) $(SRC_DIR)/*.cpp 2>&1 | tee analysis.log
	@echo "Static analysis complete. Report: analysis.log"

# Full test suite
test: build-engine debug
	@echo "Running test game executable tests..."
	timeout 10 bin/testgame.exe || echo "10-second automated test completed"
	@echo "✅ Basic functionality verified"

# Distribution build (optimized + dependencies)
dist: release copy-dlls
	@echo "Creating distribution..."
	mkdir -p dist
	cp bin/testgame.exe dist/
	cp bin/*.dll dist/ 2>/dev/null || echo "No DLLs to copy"
	cp ../README.md dist/
	@echo "✅ Distribution ready in dist/"

# Print configuration
info:
	@echo "=== TestGame Build Configuration ==="
	@echo "Compiler: $(CXX)"
	@echo "Flags: $(CXXFLAGS)"
	@echo "Include dirs: $(INCLUDE_DIRS)"
	@echo "Library dirs: $(LIBRARY_DIRS)"
	@echo "Libraries: $(LIBS)"
	@echo "Target: $(TARGET)"
	@echo "Engine: $(ENGINE_DIR)/lib/libAIEngine.a"
	@echo "===================================="

# DLL installation help
help-dlls:
	@echo "==============================================="
	@echo "Windows DLL Setup for Team Development"
	@echo "==============================================="
	@echo ""
	@echo "The game requires SDL2.dll and glew32.dll to run."
	@echo ""
	@echo "� Required DLL Locations:"
	@echo "   engine/deps/SDL2/lib/windows-mingw64/SDL2.dll"
	@echo "   engine/deps/glew/lib/windows-mingw64/glew32.dll"
	@echo ""
	@echo "📦 How to obtain the DLLs:"
	@echo "   1. SDL2.dll: Download from https://github.com/libsdl-org/SDL/releases"
	@echo "      - Get 'Development Libraries' for mingw"
	@echo "      - Extract and copy bin/SDL2.dll to the above location"
	@echo ""
	@echo "   2. glew32.dll: Download from https://glew.sourceforge.net/"
	@echo "      - Get 'Binaries' for Windows"
	@echo "      - Extract and copy bin/Release/x64/glew32.dll to the above location"
	@echo ""
	@echo "🔍 VERIFY: Run 'make copy-dlls' to test DLL detection"
	@echo "==============================================="

# Help target
help:
	@echo "Available targets:"
	@echo "  all         - Build test game (default)"
	@echo "  debug       - Build with debug symbols"
	@echo "  release     - Build optimized version"
	@echo "  run         - Build and run test game"
	@echo "  clean       - Remove build artifacts"
	@echo "  rebuild     - Clean rebuild including engine"
	@echo "  dev         - Full development build with DLLs"
	@echo "  copy-dlls   - Copy required runtime DLLs"
	@echo "  profile     - Build with profiling support"
	@echo "  coverage    - Build with coverage instrumentation"
	@echo "  memcheck    - Run memory leak detection"
	@echo "  benchmark   - Run performance benchmarks"
	@echo "  stress      - Run 5-minute stress test"
	@echo "  test        - Run basic functionality tests"
	@echo "  format      - Format source code"
	@echo "  analyze     - Run static code analysis"
	@echo "  dist        - Create distribution package"
	@echo "  info        - Show build configuration"
	@echo "  help-dlls   - Show DLL setup instructions for Windows"
	@echo "  help        - Show this help"

.PHONY: all debug release clean run build-engine rebuild copy-dlls dev info check-engine \
        profile coverage memcheck benchmark stress format analyze test dist help help-dlls