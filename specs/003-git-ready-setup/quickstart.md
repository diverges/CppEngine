# Quick Start: Git Ready Multi-Developer Setup

**Feature**: Git Ready Multi-Developer Setup  
**Target Audience**: New developers joining the AIEngine project  
**Time to Complete**: ~10 minutes

## Prerequisites ✅

Before starting, ensure you have:
- **Git** installed and configured
- **MSYS2** with MinGW-w64 toolchain installed
- **VS Code** with C++ extension pack
- **Windows 10/11** with at least 8GB RAM, 4-core CPU

## Getting Started

### 1. Clone and Build (5 minutes)

```bash  
# Clone the repository
git clone <repository-url> AIEngine
cd AIEngine

# Verify environment (optional but recommended)
make check-env

# Build the entire project (engine + testgame)
make debug
```

**Expected Output**:
```
Building engine...
✓ Engine library: bin/debug/libAIEngine.a
Building testgame...  
✓ TestGame executable: bin/debug/testgame.exe
✓ Debug build complete
```

### 2. Open in VS Code (2 minutes)

```bash
# Open workspace in VS Code
code .
```

**What happens automatically**:
- ✅ IntelliSense activates for all C++ files
- ✅ Build tasks available via Ctrl+Shift+P → "Run Build Task" 
- ✅ Debug configurations ready for F5 debugging
- ✅ Error highlighting works across all components

### 3. Verify Setup (3 minutes)

```bash
# Run unit tests
make test-unit

# Run integration tests  
make test-integration

# Launch the test game
make run
```

**Expected Results**:
- Unit tests pass with results in `bin/debug/test-results/`
- TestGame window opens and displays basic graphics
- No compilation errors or missing dependencies

## Development Workflow

### Daily Development Commands

| Task | Command | Description |
|------|---------|-------------|
| **Build for development** | `make` or `make debug` | Default debug build |
| **Build for release** | `make release` | Optimized release build |
| **Run unit tests** | `make test-unit` | Engine component tests only |
| **Run integration tests** | `make test-integration` | Full system validation |
| **Clean build artifacts** | `make clean` | Remove all bin/ contents |
| **Launch test game** | `make run` | Execute testgame.exe |

### VS Code Shortcuts

| Action | Shortcut | Result |
|--------|----------|--------|
| **Build default** | `Ctrl+Shift+B` | Runs "Build Debug" task |
| **Start debugging** | `F5` | Launches TestGame with debugger |
| **Run task** | `Ctrl+Shift+P` → "Run Task" | Access all build tasks |
| **Go to definition** | `F12` or `Ctrl+Click` | Navigate to symbol definition |
| **Find references** | `Shift+F12` | Find all usages of symbol |

## Directory Structure After Setup

```
AIEngine/
├── Makefile                 # Root build coordination
├── bin/                     # ← NEW: Centralized build outputs
│   ├── debug/              #   Debug artifacts
│   └── release/            #   Release artifacts  
├── .vscode/                # ← NEW: VS Code configuration
│   ├── c_cpp_properties.json
│   ├── tasks.json
│   ├── launch.json
│   └── settings.json
├── engine/                 # Engine component (existing)
├── testgame/              # TestGame component (existing)
└── docs/                  # Documentation (existing)
```

## Troubleshooting

### Build Issues

**Problem**: `make: command not found`  
**Solution**: Install MSYS2 and add to PATH: `C:\msys64\ucrt64\bin`

**Problem**: `g++: command not found` during build  
**Solution**: Install MinGW-w64 via MSYS2: `pacman -S mingw-w64-ucrt-x86_64-gcc`

**Problem**: Build succeeds but no files in `bin/debug/`  
**Solution**: Check component Makefiles are outputting to `$(BIN_OUTPUT)` variable

### VS Code Issues

**Problem**: IntelliSense shows red squiggles on valid code  
**Solution**: Check that `c_cpp_properties.json` includes all necessary include paths

**Problem**: Build task fails with "make not found"  
**Solution**: Verify MSYS2 `bin` directory is in Windows PATH environment variable

**Problem**: Debugging doesn't start or breakpoints don't hit  
**Solution**: Ensure debug build completed successfully and gdb path is correct in `launch.json`

### Performance Issues

**Problem**: Initial build takes longer than 5 minutes  
**Solution**: Use parallel builds: `make -j4 debug` (adjust number to CPU cores)

**Problem**: IntelliSense is slow or unresponsive  
**Solution**: Check that `files.exclude` pattern in settings hides build artifacts

## Advanced Usage

### Building Specific Components

```bash
# Build only engine
make engine

# Build only testgame (after engine is built)
make testgame

# Force rebuild all
make clean && make debug
```

### Custom Build Configurations

```bash
# Build with custom flags
make debug EXTRA_CXXFLAGS="-DVERBOSE_LOGGING"

# Build release with profiling
make release EXTRA_CXXFLAGS="-pg"
```

### Multiple Workspaces

The configuration supports multiple developers working on the same codebase:
- Each developer can have personal VS Code settings
- Build outputs are shared but intermediate files are per-component
- Git ignores personal workspace customizations automatically

## Success Indicators

After following this quickstart, you should achieve:
- ✅ Complete build in under 10 minutes from clone
- ✅ IntelliSense working with code completion and error detection  
- ✅ Debugging functional with breakpoints in engine and testgame code
- ✅ All tests passing (unit + integration)
- ✅ TestGame launches and displays graphics correctly

## Next Steps

- Review [architecture documentation](../../docs/architecture.md) to understand engine design
- Explore [testing framework documentation](../../docs/testing-framework-research.md) for writing new tests
- Check [performance guidelines](../../docs/performance_optimization.md) for optimization practices

## Support

If you encounter issues not covered in troubleshooting:
1. Check that your environment meets prerequisites listed above
2. Verify that existing builds work if cloning after initial setup
3. Consult team documentation or reach out for assistance

**Target Completion**: This quickstart should result in a fully functional development environment within 10 minutes for developers with proper prerequisites installed.