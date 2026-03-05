# Implementation Plan: Git Ready Multi-Developer Setup

**Branch**: `003-git-ready-setup` | **Date**: March 4, 2026 | **Spec**: [spec.md](spec.md)
**Input**: Feature specification from `/specs/003-git-ready-setup/spec.md`

**Note**: This template is filled in by the `/speckit.plan` command. See `.specify/templates/plan-template.md` for the execution workflow.

## Summary

Transform the bootstrapped AIEngine project into a collaboration-ready codebase that works seamlessly across different Windows machines. Primary requirement: eliminate local machine paths and provide unified build workflow with centralized output structure. Technical approach: root-level Makefile orchestration with `bin/debug/` and `bin/release/` directories, VS Code workspace configuration, and single-version dependency management.

## Technical Context

**Language/Version**: C++17 minimum (C++20 preferred per constitution)  
**Primary Dependencies**: SDL2, OpenGL, doctest (testing framework), currently checked into `engine/deps/`  
**Storage**: N/A (game engine, no persistent storage requirements)  
**Testing**: doctest for unit tests, integration tests via testgame executable  
**Target Platform**: Windows 10/11 (multi-machine collaboration focus)
**Project Type**: Static library (engine) + executable (testgame) + test executables  
**Performance Goals**: 60fps minimum per constitution, <5 minute builds on mid-range hardware  
**Constraints**: No external package managers, relative paths only, VS Code-focused IDE support  
**Scale/Scope**: Small team collaboration, engine + testgame + tests (~50 source files currently)

**Current Gaps Requiring Research**:

- Root Makefile coordination strategy for engine + testgame dependency management
- VS Code workspace configuration best practices for C++ multi-component projects  
- Build output organization patterns for centralized bin/ structure
- Cross-component incremental build detection mechanisms

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

### Phase 0 Check (Pre-Research)

✅ **Modern C++ First**: Feature maintains existing C++17/20 standards, no impact on language version or practices  
✅ **Cross-Platform Compatibility**: Currently Windows-focused by design, does not violate cross-platform principle (scope limitation)  
✅ **Component-Driven Architecture**: Feature enhances build system without affecting ECS architecture  
✅ **Performance-Conscious Development**: Build time optimization aligns with performance goals, 60fps target maintained  
✅ **Test-Driven Learning**: Separates unit and integration testing, maintains testing discipline  

**Build System Standards Check**:
✅ **GNU Make**: Feature extends existing Make-based build system  
✅ **Minimal Dependencies**: No new external dependencies, organizes existing SDL2/OpenGL libs  
✅ **Standard Make Commands**: Provides unified make commands from root  
✅ **Multi-Compiler Support**: Maintains GCC/Clang/MSVC compatibility

### Phase 1 Check (Post-Design)

✅ **Modern C++ First**: Build configurations enforce C++17 minimum via `-std=c++17` flags
✅ **Cross-Platform Compatibility**: Forward slash paths and Make patterns support future cross-platform expansion
✅ **Component-Driven Architecture**: Design preserves component isolation while adding coordination
✅ **Performance-Conscious Development**: Parallel builds (`make -j4`) and incremental compilation optimize build times
✅ **Test-Driven Learning**: Separate unit/integration test commands maintain testing discipline

**Build System Standards Re-Check**:
✅ **GNU Make**: Root Makefile uses standard Make patterns (dependencies, phony targets, variables)
✅ **Minimal Dependencies**: No additional external tools required, uses existing toolchain
✅ **Standard Make Commands**: Interface contract provides predictable `make debug`, `make release`, `make test` commands
✅ **Multi-Compiler Support**: VS Code configuration supports both GCC and Clang via compiler path settings

**Gate Status: PASSED** - Design maintains full constitution compliance and enhances rather than violates established standards.

## Project Structure

### Documentation (this feature)

```text
specs/[###-feature]/
├── plan.md              # This file (/speckit.plan command output)
├── research.md          # Phase 0 output (/speckit.plan command)
├── data-model.md        # Phase 1 output (/speckit.plan command)
├── quickstart.md        # Phase 1 output (/speckit.plan command)
├── contracts/           # Phase 1 output (/speckit.plan command)
└── tasks.md             # Phase 2 output (/speckit.tasks command - NOT created by /speckit.plan)
```

### Source Code (repository root)

```text
# Current AIEngine structure (to be enhanced)
engine/
├── Makefile             # Component-level build
├── src/                 # Engine source code
├── include/AIEngine/    # Public headers  
├── tests/unit/          # Engine unit tests
├── deps/               # Bundled dependencies
├── build/              # Intermediate build files
└── lib/                # Engine library output

testgame/
├── Makefile            # Component-level build
├── src/                # Test game source
├── build/              # Intermediate build files  
└── bin/                # Test game executable output

# Target structure after feature implementation
Makefile                 # NEW: Root build orchestration
bin/
├── debug/              # NEW: Centralized debug builds
└── release/            # NEW: Centralized release builds

.vscode/                # NEW: VS Code workspace configuration
├── settings.json       # C++ configuration
├── tasks.json          # Build tasks
├── launch.json         # Debug configuration
└── c_cpp_properties.json # IntelliSense configuration

engine/
├── Makefile            # MODIFIED: Output to root bin/
├── src/ [unchanged]
├── include/ [unchanged] 
├── tests/ [unchanged]
├── deps/ [unchanged]
└── build/ [unchanged]  # Intermediate files stay local

testgame/
├── Makefile            # MODIFIED: Output to root bin/  
├── src/ [unchanged]
└── build/ [unchanged]  # Intermediate files stay local
```

**Structure Decision**: Hybrid approach that preserves existing component organization while adding root-level coordination and centralized output. Intermediate build files remain in component directories for isolation, but final artifacts are unified in root `bin/` directory.

## Complexity Tracking

> **No violations found** - Feature aligns with all constitution principles.
