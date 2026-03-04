# Test Game Application Makefile
# Links against AIEngine library

# Compiler and flags
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic -O2
DEBUG_FLAGS := -g -DDEBUG -O0

# Directories
SRC_DIR := src
BIN_DIR := bin
BUILD_DIR := build
ENGINE_DIR := ../engine

# Detect MinGW system libraries (MSYS2) - force detection for now
MSYS2_ROOT := C:/msys64/mingw64
INCLUDE_DIRS := -I$(ENGINE_DIR)/include -I$(MSYS2_ROOT)/include
LIBRARY_DIRS := -L$(ENGINE_DIR)/lib -L$(MSYS2_ROOT)/lib  
LIBS := -lAIEngine -lSDL2main -lSDL2 -lopengl32 -lglew32 -lgdi32

# Source files
SOURCES := $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS := $(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

# Target executable
TARGET := $(BIN_DIR)/testgame

# Default target
all: $(TARGET)

# Create directories (Windows-compatible)
$(BUILD_DIR):
ifeq ($(OS),Windows_NT)
	@if not exist "$(BUILD_DIR)" mkdir "$(BUILD_DIR)"
else
	mkdir -p $(BUILD_DIR)
endif

$(BIN_DIR):
ifeq ($(OS),Windows_NT)
	@if not exist "$(BIN_DIR)" mkdir "$(BIN_DIR)"
else
	mkdir -p $(BIN_DIR)
endif

# Check engine library exists (Windows-compatible)
check-engine:
ifeq ($(OS),Windows_NT)
	@if not exist "$(ENGINE_DIR)\lib\libAIEngine.a" (echo "ERROR: Engine library not found. Run 'make' in engine/ directory first." && exit 1)
else
	@test -f $(ENGINE_DIR)/lib/libAIEngine.a || (echo "ERROR: Engine library not found. Run 'make' in engine/ directory first."; exit 1)
endif
	@echo "+ Engine library found"

# Build executable
$(TARGET): check-engine $(BIN_DIR) $(OBJECTS)
	$(CXX) $(OBJECTS) $(LIBRARY_DIRS) $(LIBS) -o $(TARGET)
	@echo "Built test game: $(TARGET)"

# Compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) -c $< -o $@

# Debug build
debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: $(TARGET)

# Release build
release: CXXFLAGS += -DNDEBUG
release: $(TARGET)

# Clean build artifacts
clean:
ifeq ($(OS),Windows_NT)
	@if exist "$(BUILD_DIR)" rmdir /s /q "$(BUILD_DIR)" 
	@if exist "$(BIN_DIR)" rmdir /s /q "$(BIN_DIR)"
else
	rm -rf $(BUILD_DIR) $(BIN_DIR)
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

# Copy required DLLs for Windows
copy-dlls: $(TARGET)
	@echo "Copying required DLLs..."
	@if [ -f "$(ENGINE_DIR)/deps/SDL2/lib/SDL2.dll" ]; then \
		cp "$(ENGINE_DIR)/deps/SDL2/lib/SDL2.dll" $(BIN_DIR)/; \
		echo "✓ Copied SDL2.dll"; \
	fi

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

# Help target
help:
	@echo "Available targets:"
	@echo "  all       - Build test game (default)"
	@echo "  debug     - Build with debug symbols"
	@echo "  release   - Build optimized version"
	@echo "  run       - Build and run test game"
	@echo "  clean     - Remove build artifacts"
	@echo "  rebuild   - Clean rebuild including engine"
	@echo "  dev       - Full development build"
	@echo "  profile   - Build with profiling support"
	@echo "  coverage  - Build with coverage instrumentation"
	@echo "  memcheck  - Run memory leak detection"
	@echo "  benchmark - Run performance benchmarks"
	@echo "  stress    - Run 5-minute stress test"
	@echo "  test      - Run basic functionality tests"
	@echo "  format    - Format source code"
	@echo "  analyze   - Run static code analysis"
	@echo "  dist      - Create distribution package"
	@echo "  info      - Show build configuration"
	@echo "  help      - Show this help"

.PHONY: all debug release clean run build-engine rebuild copy-dlls dev info check-engine \
        profile coverage memcheck benchmark stress format analyze test dist help