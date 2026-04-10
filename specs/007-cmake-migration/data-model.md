# Data Model: CMake Build System Migration

**Feature**: 007-cmake-migration  
**Date**: 2026-04-09  
**Source**: research.md + spec.md

This document defines the CMake build graph — the targets, files, and relationships that make up the new build system. It is not implementation code; it is the structural design from which implementation tasks are derived.

---

## Build Graph Overview

```
CMakePresets.json (root)
      │
      ├── configure preset: "debug"  ──── binaryDir: build/debug
      └── configure preset: "release" ── binaryDir: build/release
                │
                ▼
      CMakeLists.txt (root)
      ├── output dirs:  bin/debug/  |  bin/release/
      ├── add_subdirectory(engine)
      ├── add_subdirectory(testgame)
      └── add_custom_target: run
                │
      ┌─────────┴──────────┐
      ▼                    ▼
CMakeLists.txt (engine)  CMakeLists.txt (testgame)
STATIC library           EXECUTABLE
AIEngine                 testgame
      │                      │
      ▼                      └── links: AIEngine (only)
engine/cmake/                     (SDL2, GLEW, opengl32, gdi32
VendoredDeps.cmake                 all propagated transitively
IMPORTED targets                   via AIEngine PUBLIC deps)
SDL2::SDL2 (PUBLIC)
SDL2::main (PUBLIC)
GLEW::GLEW (PUBLIC)
GLM::GLM (INTERFACE/PUBLIC)
doctest::doctest (INTERFACE/PUBLIC)
GLAD::GLAD (INTERFACE/PUBLIC)

POST_BUILD on testgame: copy SDL2.dll, glew32.dll (Windows only)
```

---

## File Structure (New Files)

```
AIEngine/
├── CMakeLists.txt                     # Root orchestrator
├── CMakePresets.json                  # Preset definitions (debug/release/run)
├── engine/
│   ├── CMakeLists.txt                 # Engine static library definition
│   └── cmake/
│       └── VendoredDeps.cmake         # IMPORTED targets for all bundled deps
└── testgame/
    └── CMakeLists.txt                 # Testgame executable definition
```

**Files NOT created by this migration** (remain, replaced by CMake equivalents):

- `Makefile` (root) → replaced by `CMakeLists.txt` + `CMakePresets.json`
- `engine/Makefile` → replaced by `engine/CMakeLists.txt`
- `testgame/Makefile` → replaced by `testgame/CMakeLists.txt`

---

## CMake Targets

### Target: `AIEngine` (STATIC library)

| Property | Value |
|----------|-------|
| Type | `add_library(AIEngine STATIC ...)` |
| Sources | All `.cpp` under `engine/src/**` |
| Public headers | `engine/include/AIEngine/` |
| Link libraries | `GLEW::GLEW`, `SDL2::SDL2`, `SDL2::main`, `opengl32`, `gdi32` (Windows), `GLM::GLM`, `GLAD::GLAD` — all declared **PUBLIC** so they propagate transitively to consumers |
| C++ standard | `target_compile_features(AIEngine PUBLIC cxx_std_17)` |
| Output | `bin/debug/libAIEngine.a` or `bin/release/libAIEngine.a` |

### Target: `testgame` (EXECUTABLE)

| Property | Value |
|----------|-------|
| Type | `add_executable(testgame ...)` |
| Sources | All `.cpp` under `testgame/src/` |
| Link libraries | `AIEngine` only — all third-party deps (SDL2, GLEW, GLM, GLAD, opengl32, gdi32) are inherited transitively via AIEngine's PUBLIC link interface |
| Post-build | Copy `SDL2.dll`, `glew32.dll` to output dir (Windows only) |
| Linker options | `-static-libgcc -static-libstdc++` (Windows/GCC only) |
| Output | `bin/debug/testgame.exe` or `bin/release/testgame.exe` |

### Target: `run` (custom)

| Property | Value |
|----------|-------|
| Type | `add_custom_target(run ...)` |
| Command | `$<TARGET_FILE:testgame>` |
| Working dir | `$<TARGET_FILE_DIR:testgame>` |
| Depends on | `testgame` |
| Terminal | `USES_TERMINAL` |

---

## IMPORTED Dependency Targets (`engine/cmake/VendoredDeps.cmake`)

| Target Name | Type | Source |
|-------------|------|--------|
| `SDL2::SDL2` | IMPORTED STATIC | `engine/deps/SDL2/lib/windows-mingw64/libSDL2.a` + implib `.dll.a` |
| `SDL2::main` | IMPORTED STATIC | `engine/deps/SDL2/lib/windows-mingw64/libSDL2main.a` |
| `GLEW::GLEW` | IMPORTED STATIC | `engine/deps/glew/lib/windows-mingw64/libglew32.a` + implib `.dll.a` |
| `GLM::GLM` | INTERFACE IMPORTED | `engine/deps/glm/` (headers only) |
| `doctest::doctest` | INTERFACE IMPORTED | `engine/deps/doctest/` (headers only) |
| `GLAD::GLAD` | INTERFACE IMPORTED | `engine/deps/glad/include/` (headers only) |

---

## Build Configuration Mapping

| Preset | `CMAKE_BUILD_TYPE` | `binaryDir` | Output dir | Compile flags |
|--------|--------------------|-------------|------------|---------------|
| `debug` | `Debug` | `build/debug` | `bin/debug/` | `-g -O0 -DDEBUG` |
| `release` | `Release` | `build/release` | `bin/release/` | `-O3 -DNDEBUG` |

CMake sets these flag sets automatically for GCC/Clang when `CMAKE_BUILD_TYPE` is `Debug` or `Release`. No manual flag definition required at the project level for the standard flags.

---

## Output Directory Variables (Root CMakeLists.txt)

```cmake
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin/$<LOWER_CASE:$<CONFIG>>")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin/$<LOWER_CASE:$<CONFIG>>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin/$<LOWER_CASE:$<CONFIG>>")
```

These three variables cover: executables (RUNTIME), static libraries (ARCHIVE), and shared libraries (LIBRARY). Setting all three at the root ensures all build outputs land in the correct `bin/debug/` or `bin/release/` directory regardless of which sub-project creates them.

---

## Validation Rules

- `AIEngine` must declare all runtime deps (`SDL2::SDL2`, `SDL2::main`, `GLEW::GLEW`, `opengl32`, `gdi32`) as **PUBLIC** in `target_link_libraries` so testgame inherits them transitively without declaring them directly.
- `AIEngine` must declare `cxx_std_17` as a PUBLIC compile feature so consumers automatically inherit C++17 mode.
- `VendoredDeps.cmake` must be included via `include()` (not `add_subdirectory()`) since it defines IMPORTED targets, not a build sub-directory.
- The `run` target must have `WORKING_DIRECTORY "$<TARGET_FILE_DIR:testgame>"` so SDL2.dll and glew32.dll (copied alongside the executable) are found at runtime.
- Platform guards (`if(WIN32)`) must wrap all DLL copy commands and Windows-specific link flags.

---

## Out of Scope

- Test targets (`engine_tests`) — test infrastructure migration is deferred (documented in spec Assumptions).
- `tests/Makefile` and `engine/tests/` — not touched by this migration.
- `CMakeUserPresets.json` — personal developer overrides; an example template may be provided in quickstart.md but is not a build artifact.
