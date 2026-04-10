# Feature Specification: CMake Build System Migration

**Feature Branch**: `007-cmake-migration`  
**Created**: 2026-04-09  
**Status**: Draft  
**Input**: User description: "This project currently uses make for building, running, and debugging. I want to migrate it to CMAKE. The current make implementation is complex and implements many unneeded functions. I want CMAKE to handle three basic operations - a debug and release build alongside a way to run their output."

## Clarifications

### Session 2026-04-09

- Q: Should testgame independently declare its link dependencies on SDL2, GLEW, and other third-party libraries, or should those propagate transitively from AIEngine? → A: testgame should only link against AIEngine; AIEngine must expose its dependencies with sufficient visibility to propagate them to consumers automatically.

## User Scenarios & Testing *(mandatory)*

### User Story 1 - Debug Build (Priority: P1)

A developer runs a single command to produce a debug-mode binary for the engine and testgame. The resulting binary includes debug symbols and no optimizations so the developer can attach a debugger or inspect output immediately.

**Why this priority**: This is the most frequent day-to-day operation. Every developer depends on a working debug build before any other workflow is useful.

**Independent Test**: Running the debug build command produces `libAIEngine.a` and `testgame.exe` (or platform-equivalent) under a predictable output directory, with debug symbols present and compiler optimizations disabled.

**Acceptance Scenarios**:

1. **Given** a clean checkout, **When** the developer invokes the debug build command, **Then** both the engine library and testgame executable are produced under `bin/debug/` without errors.
2. **Given** the debug build has been run, **When** the developer inspects the binary, **Then** debug symbols are present and optimization level is set to none/minimal.
3. **Given** a source file has changed, **When** the developer re-runs the debug build, **Then** only the changed files are recompiled (incremental build).

---

### User Story 2 - Release Build (Priority: P2)

A developer runs a single command to produce an optimized release-mode binary for the engine and testgame, suitable for performance testing or distribution.

**Why this priority**: Release builds are needed for performance validation and final output, but less frequent than debug builds during active development.

**Independent Test**: Running the release build command produces `libAIEngine.a` and `testgame.exe` under `bin/release/` with full optimizations and no debug symbols.

**Acceptance Scenarios**:

1. **Given** a clean checkout, **When** the developer invokes the release build command, **Then** both the engine library and testgame executable are produced under `bin/release/` without errors.
2. **Given** the release build has been run, **When** the developer inspects the binary, **Then** optimizations are maximized and debug symbols are absent.
3. **Given** both debug and release builds exist, **When** inspecting the output directories, **Then** the two variants are kept in separate, non-overlapping output directories.

---

### User Story 3 - Run Testgame (Priority: P3)

A developer runs a single command to execute the testgame after a successful build, without manually locating the binary or constructing a path.

**Why this priority**: Providing a quick run shortcut reduces friction in the edit-build-run cycle, but is secondary to the builds themselves.

**Independent Test**: After a debug build, running the run command launches the testgame binary correctly without requiring the developer to know its location.

**Acceptance Scenarios**:

1. **Given** a successful debug build exists, **When** the developer invokes the run command, **Then** the testgame launches from the correct output directory.
2. **Given** no build has been performed, **When** the developer invokes the run command, **Then** the build system reports a clear error rather than silently failing.

---

### Edge Cases

- What happens when the build directory does not yet exist? The build system must create it automatically.
- What happens when a required dependency (SDL2, GLEW, GLM) is not found? The build system must halt with a clear, actionable error message.
- What happens when both debug and release builds exist simultaneously? They must remain isolated and not interfere with each other.
- How does the system handle compiler differences between GCC, Clang, and MSVC? Compiler selection should fall back gracefully and not require manual per-developer configuration.

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: The build system MUST support a debug build that compiles both the engine library and the testgame executable with debug symbols and minimal optimization.
- **FR-002**: The build system MUST support a release build that compiles both the engine library and the testgame executable with full optimizations and no debug symbols.
- **FR-003**: Debug and release build outputs MUST be placed in separate, non-overlapping directories (`bin/debug/` and `bin/release/` respectively).
- **FR-004**: The build system MUST support incremental compilation, rebuilding only changed files.
- **FR-005**: The build system MUST provide a way to run the testgame binary after a build without requiring the developer to manually specify a path.
- **FR-006**: The build system MUST locate all bundled dependencies (SDL2, GLEW, GLAD, GLM, doctest) automatically without requiring manual paths from the developer. The engine library MUST declare these dependencies with sufficient visibility that testgame consumers inherit them transitively — testgame MUST only need to declare a single link dependency on the engine library.
- **FR-007**: The build system MUST produce clear, actionable error messages when a required dependency is not found or when a build step fails.
- **FR-008**: The build system MUST support Windows, macOS, and Linux without platform-specific workarounds per developer.
- **FR-009**: The build system MUST NOT include build targets or functionality not directly related to the three core operations: debug build, release build, and run.

### Key Entities

- **CMakeLists.txt (root)**: The top-level build descriptor that orchestrates the engine and testgame sub-projects and exposes the three core operations.
- **CMakeLists.txt (engine)**: Defines the engine static library target, its sources, include paths, and bundled dependency references.
- **CMakeLists.txt (testgame)**: Defines the testgame executable target with a single link dependency on the engine library. All dependency libraries and include paths are inherited transitively from the engine library; no direct dependency declarations for SDL2, GLEW, or other third-party libraries are present in this file.
- **Build preset (debug)**: A named configuration that activates debug flags, disables optimizations, and routes output to `bin/debug/`.
- **Build preset (release)**: A named configuration that activates full optimizations, strips debug info, and routes output to `bin/release/`.

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: A developer on any supported platform can produce a working debug build with a single command from the repository root.
- **SC-002**: A developer on any supported platform can produce a working release build with a single command from the repository root.
- **SC-003**: A developer can launch the testgame after a build with a single command, without manually locating the binary.
- **SC-004**: The build system configuration is simpler than the current Make setup — measured by having fewer total build configuration files and no targets unrelated to the three core operations.
- **SC-005**: Incremental builds after a single-file change complete in under the time required for a full clean build (demonstrating CMake's dependency tracking is functioning).
- **SC-006**: Build errors due to missing dependencies produce a message that identifies the missing dependency by name and suggests a resolution.

## Assumptions

- Bundled dependencies (SDL2, GLEW, GLAD, GLM, doctest) remain in their current locations under `engine/deps/` and do not need to be fetched at build time.
- testgame declares only a single link dependency on the engine library (`AIEngine`). All third-party dependency resolution is owned by the engine library's build definition and propagated to consumers automatically.
- The primary target platform for initial implementation is Windows (matching current developer environment), with macOS and Linux validated subsequently.
- CMake version 3.20 or higher is assumed available, as it provides the `cmake --build` and preset features needed for simple single-command workflows.
- The testgame is the sole runnable output; no separate test runner invocation is required as part of this migration scope (test infrastructure migration is out of scope).
- The existing VS Code tasks in `.vscode/tasks.json` will be updated to use the new CMake commands, replacing the `make` invocations.
