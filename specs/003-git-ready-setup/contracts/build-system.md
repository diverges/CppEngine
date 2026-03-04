# Build System Interface Contract

**Purpose**: Define the external interface for build operations that developers and CI systems will use
**Type**: Command-line interface via GNU Make
**Scope**: Root-level `Makefile` targets available to external users

## Available Targets

### Primary Build Targets

```bash
# Build debug variant (default)
make debug
make          # Equivalent to 'make debug'

# Build release variant  
make release

# Build all variants
make all      # Builds both debug and release
```

**Contracts**:

- **Input**: None (uses current source tree state)
- **Output**: Build artifacts in `bin/debug/` or `bin/release/`
- **Exit Codes**: 0 = success, non-zero = build failure
- **Side Effects**: Creates/updates build directories, generates object files
- **Dependencies**: Engine builds before testgame automatically

### Cleanup Targets

```bash
# Remove all build artifacts
make clean

# Remove only debug artifacts
make clean-debug

# Remove only release artifacts  
make clean-release
```

**Contracts**:

- **Input**: None
- **Output**: Removed directories and files
- **Exit Codes**: 0 = success (even if nothing to clean)
- **Side Effects**: Removes `bin/`, `engine/build/`, `testgame/build/`

### Test Execution Targets

```bash
# Run engine unit tests only
make test-unit

# Run integration tests only (testgame execution)
make test-integration

# Run all tests (unit + integration)
make test
```

**Contracts**:

- **Input**: Requires successful debug build
- **Output**: Test results to stdout, JUnit XML to `bin/debug/test-results/`
- **Exit Codes**: 0 = all tests pass, non-zero = test failures
- **Dependencies**: Automatically builds debug variant if needed

### Utility Targets

```bash
# Execute testgame
make run

# Show build information
make info

# Validate environment setup
make check-env
```

## Output Directory Contract

### Structure

```
bin/
├── debug/
│   ├── libAIEngine.a         # Engine static library
│   ├── testgame.exe          # TestGame executable
│   ├── engine_tests.exe      # Engine unit tests
│   └── test-results/         # Test output directory
│       ├── unit.xml          # Unit test results (JUnit format)
│       └── integration.xml   # Integration test results
└── release/
    ├── libAIEngine.a         
    ├── testgame.exe          
    └── engine_tests.exe
```

### File Naming Conventions

- **Static Libraries**: `lib{ComponentName}.a` (Unix convention)
- **Executables**: `{component}.exe` (Windows extension automatic)
- **Test Executables**: `{component}_tests.exe`
- **Test Results**: `{type}.xml` in JUnit format

### Path Requirements

- All paths relative to repository root
- Forward slashes supported on Windows (MinGW compatibility)
- No spaces or special characters in generated names
- Predictable locations for CI/CD automation

## Environment Contract

### Prerequisites

**Required Tools** (must be in PATH):

- `make` (GNU Make 4.0+)
- `g++` or `clang++` (C++17 support minimum)
- `git` (for version control operations)

**Directory Requirements**:

- Write permissions to repository root (for bin/ creation)
- Read access to `engine/deps/` (bundled dependencies)

**Platform Constraints**:

- Windows 10/11 operating system
- MSYS2/MinGW or Visual Studio toolchain
- Minimum 8GB RAM, 4-core CPU (per success criteria)

### Error Handling

**Build Failures**:

- Exit code indicates failure type (compilation=1, linking=2, test=3)
- Error messages include file and line number information
- Partial builds leave intermediate files for debugging

**Missing Dependencies**:

- Clear error message indicating missing toolchain component
- Suggested resolution steps (install MSYS2, add to PATH)
- Graceful degradation where possible

## Backward Compatibility  

**Existing Makefile Commands**:

- `engine/Makefile` and `testgame/Makefile` continue to work independently
- Developers can still build components individually if needed
- Existing scripts and documentation remain functional

**Migration Strategy**:

- New root Makefile supplements, not replaces, component Makefiles  
- Gradual migration of build outputs to centralized structure
- Old `testgame/bin/` directory can coexist during transition

## Integration Points

### VS Code Integration

- Build tasks invoke these Makefile targets directly
- Debug configurations reference output paths from this contract
- Problem matchers parse build output for error navigation

### CI/CD Integration

- Automated builds use same targets as developers
- Test results in predictable JUnit XML format for reporting
- Exit codes enable pipeline failure detection

### Documentation References

- README.md documents these make targets as the primary interface
- Individual component documentation references root build system
- Example workflows use standardized command patterns
