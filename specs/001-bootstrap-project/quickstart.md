# Quickstart: Bootstrap Basic Project

**Purpose**: Get started with the AIEngine C++ game engine bootstrap project  
**Target Audience**: C++ developers learning game engine development  
**Prerequisites**: Windows 11, GCC compiler, Make build tools, Git  

## Quick Setup (5 minutes)

### 1. Clone and Setup

```bash
# Clone the repository  
git clone <repository-url> AIEngine
cd AIEngine

# Switch to feature branch
git checkout 001-bootstrap-project

# Verify build tools
gcc --version
make --version
```

### 2. Build Engine Library

```bash
# Build the engine library
cd engine
make clean && make

# Expected output:
# Compiling src/core/Engine.cpp...
# Compiling src/graphics/Renderer.cpp...
# Compiling src/scene/SceneNode.cpp...
# Compiling src/components/TransformComponent.cpp...
# Creating libAIEngine.a...
# Build complete!
```

**Success Check**: Verify `engine/lib/libAIEngine.a` exists

### 3. Build and Run Test Game

```bash
# Build test game (links against engine)
cd ../testgame  
make clean && make

# Run the test game
./testgame

# Expected result: Window opens showing a rotating 3D cube
```

**Success Check**: Window displays with 3D cube, frame rate shows ~60fps

## Project Structure Overview

```
AIEngine/
├── engine/              # Game engine library
│   ├── include/         # Public API headers
│   ├── src/             # Engine implementation
│   ├── lib/             # Built library output
│   └── Makefile         # Engine build configuration
│
├── testgame/            # Test game application
│   ├── src/             # Game source code  
│   ├── assets/          # Game assets (if any)
│   └── Makefile         # Game build (links engine)
│
├── tests/               # Test infrastructure
│   ├── unit/            # Unit tests for engine
│   └── integration/     # Integration tests
│
└── docs/                # Documentation
    ├── architecture.md  # Engine design docs
    └── examples/        # Usage examples
```

## Building from Scratch

### Engine Library Build Process

```bash
cd engine

# Clean previous builds
make clean

# Build in debug mode (default)  
make debug

# Build optimized release version
make release

# Run engine unit tests
make test
```

**Build Outputs**:

- `lib/libAIEngine.a` - Static library for linking
- `lib/libAIEngine_d.a` - Debug version with symbols
- `bin/engine_tests` - Unit test executable

### Test Game Build Process

```bash
cd testgame

# Build debug version
make debug

# Build release version  
make release

# Run with debug output
make run-debug
```

**Build Outputs**:

- `bin/testgame` - Game executable
- `bin/testgame_d` - Debug version with symbols

## Development Workflow

### 1. Making Engine Changes

```bash
# Edit engine source files
vim engine/src/graphics/Renderer.cpp

# Rebuild engine library
cd engine && make

# Rebuild test game with new library  
cd ../testgame && make

# Test changes
./bin/testgame
```

### 2. Adding New Components to Scene Nodes

```bash
# Create new component header
vim engine/include/AIEngine/components/MyComponent.hpp

# Create implementation
vim engine/src/components/MyComponent.cpp

# Add to build system
# (Edit engine/Makefile SOURCES list)

# Rebuild and test
cd engine && make && cd ../testgame && make
```

### 3. Working with Scene Graph

```bash
# Modify scene hierarchy in test game
vim testgame/src/main.cpp

# Add new scene nodes and components
# Test hierarchical transforms and rendering
```

### 3. Running Tests

```bash
# Run all engine unit tests
cd tests && make test

# Run specific test suite  
./bin/engine_tests --test-case="Transform*"

# Run integration tests
make integration-test
```

## Common Commands

### Build Commands

```bash
make clean          # Remove all build artifacts
make debug          # Build with debug symbols
make release        # Build optimized version
make test           # Build and run tests
make install        # Install to system (optional)
```

### Development Commands

```bash
make format         # Format code with clang-format
make lint           # Run static analysis
make docs          # Generate documentation
make package       # Create distribution package
```

### Debugging Commands  

```bash
gdb ./bin/testgame_d    # Debug test game with GDB
valgrind ./bin/testgame # Memory leak detection
perf ./bin/testgame     # Performance profiling
```

## Verifying Installation

### 1. Engine Library Verification

```bash
# Check library was built
ls -la engine/lib/libAIEngine.a

# Verify symbols in library  
nm engine/lib/libAIEngine.a | grep "CreateCube"

# Expected: Symbol for GeometryFactory::CreateCube function
```

### 2. Test Game Verification

```bash
# Run test game
cd testgame && ./bin/testgame

# Expected behavior:
# - Window opens (800x600 default size)
# - 3D cube appears in center
# - Cube rotates continuously  
# - Frame rate counter shows ~60fps
# - No console errors or warnings
```

### 3. Cross-Platform Check (Future)

```bash
# Windows (current)
./bin/testgame.exe

# Linux (future)  
./bin/testgame

# macOS (future)
./bin/testgame
```

## Troubleshooting

### Build Issues

**Missing GCC/Make**:

```bash
# Install via MSYS2 on Windows
pacman -S gcc make

# Verify installation
gcc --version && make --version
```

**OpenGL Issues**:

```bash
# Update graphics drivers
# Download latest from GPU manufacturer

# Verify OpenGL support
glxinfo | grep "OpenGL version"  # Linux
# Windows: Use GPU-Z or similar tool
```

**Library Linking Errors**:

```bash
# Check library exists  
ls engine/lib/libAIEngine.a

# Rebuild engine if missing
cd engine && make clean && make
```

### Runtime Issues

**Window Doesn't Open**:

- Check graphics drivers are up to date
- Verify monitor connection and settings
- Try running in windowed mode first

**Low Frame Rate**:

- Check system performance (Task Manager)
- Verify V-Sync settings in game
- Update graphics drivers

**Crash on Startup**:

```bash
# Run with debug symbols
./bin/testgame_d

# Use debugger for stack trace
gdb ./bin/testgame_d
(gdb) run
(gdb) bt  # Print backtrace on crash
```

## Next Steps

### Learning Path

1. **Understand the Architecture**: Read `docs/architecture.md`
2. **Explore Scene Nodes**: Study `engine/include/AIEngine/scene/`
3. **Modify the Cube**: Change colors, size, rotation in test game
4. **Add New Components**: Create custom component types
5. **Performance Analysis**: Profile and optimize rendering

### Extension Ideas

- Add texture support to cube rendering
- Implement input handling for camera movement  
- Create additional primitive shapes (sphere, plane)
- Add lighting and shading effects
- Expand scene graph with complex hierarchies

### Resources

- **Architecture Documentation**: `docs/architecture.md`
- **API Reference**: `docs/api/`  
- **Example Projects**: `docs/examples/`
- **Performance Guidelines**: `docs/performance.md`

## Success Criteria

✅ **Engine builds** without errors  
✅ **Test game builds** and links successfully  
✅ **3D cube renders** at 60fps  
✅ **No runtime crashes** during normal operation  
✅ **Cross-platform design** ready for expansion  

When all criteria are met, you have a working foundation for C++ game engine development and can proceed to more advanced features and learning objectives.
