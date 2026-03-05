# Research: Git Ready Multi-Developer Setup

**Feature**: 003-git-ready-setup  
**Research Date**: March 4, 2026  
**Scope**: Resolve technical gaps for multi-component C++ build orchestration and VS Code configuration

## Research Questions & Findings

### 1. Root Makefile Coordination Strategy

**Decision**: Hierarchical Make with explicit dependencies and variable export  
**Rationale**: GNU Make's built-in dependency management handles engine→testgame ordering automatically while allowing parallel builds within each component  

**Pattern**:

```makefile
# Root Makefile
export VARIANT
export BIN_OUTPUT := $(CURDIR)/bin/$(VARIANT)

all: engine testgame

engine: | $(BIN_OUTPUT)
 $(MAKE) -C engine

testgame: engine  # Explicit dependency - testgame waits for engine
 $(MAKE) -C testgame

$(BIN_OUTPUT):
 mkdir -p $@
```

**Alternatives considered**:

- Monolithic Makefile (rejected: breaks component encapsulation)
- CMake coordination (rejected: adds dependency, constitution prefers Make)
- Build scripts (rejected: less portable than Make)

### 2. VS Code Workspace Configuration

**Decision**: Complete .vscode/ configuration with IntelliSense, build tasks, and debugging  
**Rationale**: VS Code requires explicit configuration for C++ projects, unlike simpler languages with auto-detection

**Key Files**:

- `c_cpp_properties.json`: Configure include paths for engine/include/, engine/deps/
- `tasks.json`: Build tasks that call root Makefile (build, clean, test-unit, test-integration)
- `launch.json`: Debug configurations for testgame executable and engine tests
- `settings.json`: C++ formatter, IntelliSense settings, file associations

**Alternatives considered**:

- CMake Tools extension (rejected: sticking with Make per constitution)
- Multiple workspace files (rejected: complexity without benefit)
- Extension-dependent configuration (rejected: should work with minimal extensions)

### 3. Build Output Organization

**Decision**: `bin/debug/` and `bin/release/` subdirectories with flat structure  
**Rationale**: Matches user clarification preference and provides clear build type separation while keeping simple flat file layout within each directory

**Structure**:

```
bin/
├── debug/
│   ├── libAIEngine.a     # Engine static library
│   ├── testgame.exe      # TestGame executable  
│   └── engine_tests.exe  # Engine unit tests
└── release/
    ├── libAIEngine.a
    ├── testgame.exe
    └── engine_tests.exe
```

**Alternatives considered**:

- Component subdirectories (rejected: user preferred flat structure)
- Naming prefixes (rejected: subdirectories cleaner)
- Scattered outputs (rejected: original problem we're solving)

### 4. Cross-Component Incremental Build Detection

**Decision**: Make's automatic dependency tracking with .d files + target prerequisites  
**Rationale**: GNU Make handles file timestamp comparison automatically; component Makefiles already generate .d dependency files

**Mechanism**:

- Engine Makefile outputs to `$(BIN_OUTPUT)` when sources change
- TestGame Makefile depends on engine target completion, not just library file
- Pre-existing .d files track header dependencies within components
- Root Make coordinates between components

**Alternatives considered**:

- Sentinel files (rejected: unnecessary complexity for this scale)
- Hash-based change detection (rejected: overkill for small project)
- Forced rebuilds (rejected: performance impact)

## Implementation Strategy

### Phase 1: Root Build Coordination

1. Create root Makefile with engine→testgame dependency chain
2. Modify component Makefiles to output to `$(BIN_OUTPUT)`
3. Create centralized bin/debug/ and bin/release/ directories
4. Update clean targets to remove centralized outputs

### Phase 2: VS Code Configuration  

1. Create .vscode/c_cpp_properties.json with engine include paths
2. Create .vscode/tasks.json with root Makefile integration
3. Create .vscode/launch.json for debugging testgame and engine tests
4. Create .vscode/settings.json for C++ development

### Phase 3: Test Command Separation

1. Add unit-test and integration-test targets to root Makefile
2. Configure VS Code tasks for separate test execution
3. Ensure test outputs directory organization

## Technology Integration Points

- **GNU Make**: Root orchestration with sub-make delegation
- **MSYS2/MinGW**: Windows compilation toolchain (existing)
- **VS Code**: Primary IDE with C++ extension configurations
- **SDL2/OpenGL**: Dependencies remain in engine/deps/ (no changes)
- **doctest**: Testing framework integration (existing)
