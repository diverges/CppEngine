# Research: CMake Build System Migration

**Feature**: 007-cmake-migration  
**Date**: 2026-04-09  
**Status**: Complete — all NEEDS CLARIFICATION resolved

---

## R-001: Vendored Pre-built Library Approach

**Decision**: Use manually-created **IMPORTED targets** in a helper CMake module (`engine/cmake/VendoredDeps.cmake`) for SDL2 and GLEW. Header-only libraries (GLM, doctest, GLAD) use `target_include_directories` directly. All deps must be declared **PUBLIC** on AIEngine so they propagate transitively to testgame — testgame links only `AIEngine` with no direct dep declarations.

**Rationale**:  
The project's dependencies live under `engine/deps/` as pre-built archives and DLLs. They are not installed system-wide, so `find_package()` would not find them without a custom finder. IMPORTED targets give each dependency a named target (e.g., `SDL2::SDL2`) that can be referenced cleanly in `target_link_libraries`, supports generator expressions for DLL copying, and avoids polluting CMakeLists.txt files with absolute path strings.

```cmake
# engine/cmake/VendoredDeps.cmake - example pattern
set(_DEPS_DIR "${CMAKE_CURRENT_LIST_DIR}/../deps")

# SDL2
add_library(SDL2::SDL2 IMPORTED STATIC)
set_target_properties(SDL2::SDL2 PROPERTIES
  IMPORTED_LOCATION            "${_DEPS_DIR}/SDL2/lib/windows-mingw64/libSDL2.a"
  IMPORTED_IMPLIB              "${_DEPS_DIR}/SDL2/lib/windows-mingw64/libSDL2.dll.a"
  INTERFACE_INCLUDE_DIRECTORIES "${_DEPS_DIR}/SDL2/include"
)
add_library(SDL2::main IMPORTED STATIC)
set_target_properties(SDL2::main PROPERTIES
  IMPORTED_LOCATION "${_DEPS_DIR}/SDL2/lib/windows-mingw64/libSDL2main.a"
)

# GLEW
add_library(GLEW::GLEW IMPORTED STATIC)
set_target_properties(GLEW::GLEW PROPERTIES
  IMPORTED_LOCATION            "${_DEPS_DIR}/glew/lib/windows-mingw64/libglew32.a"
  IMPORTED_IMPLIB              "${_DEPS_DIR}/glew/lib/windows-mingw64/libglew32.dll.a"
  INTERFACE_INCLUDE_DIRECTORIES "${_DEPS_DIR}/glew/include"
)

# Header-only
add_library(GLM::GLM INTERFACE IMPORTED)
target_include_directories(GLM::GLM INTERFACE "${_DEPS_DIR}/glm")

add_library(doctest::doctest INTERFACE IMPORTED)
target_include_directories(doctest::doctest INTERFACE "${_DEPS_DIR}/doctest")

add_library(GLAD::GLAD INTERFACE IMPORTED)
target_include_directories(GLAD::GLAD INTERFACE "${_DEPS_DIR}/glad/include")
```

**Alternatives considered**:

- `find_package()` with `CMAKE_PREFIX_PATH` hints — viable but adds indirection with no gain for fully vendored deps.
- Direct absolute paths in `target_link_libraries` — works but loses generator expression capability needed for DLL copying.

---

## R-002: Debug/Release Output Directory Organization

**Decision**: Set `CMAKE_RUNTIME_OUTPUT_DIRECTORY` using a `$<CONFIG>` generator expression at the root level, mapping to `${PROJECT_SOURCE_DIR}/bin/$<LOWER_CASE:$<CONFIG>>`. This single setting works for both single-config (Ninja/Makefiles) and multi-config (VS) generators.

**Rationale**:  
Using the generator expression `$<CONFIG>` avoids duplicating logic for single-config vs multi-config generators. On single-config generators with `CMAKE_BUILD_TYPE=Debug`, `$<CONFIG>` evaluates to `Debug` at build time; combined with `$<LOWER_CASE:...>` this produces `bin/debug`. The static library (archive) goes to the same directory via `CMAKE_ARCHIVE_OUTPUT_DIRECTORY`.

```cmake
# Root CMakeLists.txt
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin/$<LOWER_CASE:$<CONFIG>>")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin/$<LOWER_CASE:$<CONFIG>>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin/$<LOWER_CASE:$<CONFIG>>")
```

Result: `bin/debug/testgame.exe` and `bin/release/testgame.exe`.

**Alternatives considered**:

- Setting `CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG` and `_RELEASE` separately — works only for single-config, breaks multi-config generators.
- Setting per-target via `set_target_properties` — tedious repeat for every target.

---

## R-003: CMakePresets.json for Simple Developer Commands

**Decision**: Create `CMakePresets.json` at the repository root with two configure presets (`debug`, `release`) and two corresponding build presets. Use Ninja generator. Developer commands become:

```bash
cmake --preset debug && cmake --build --preset debug        # full debug build
cmake --preset release && cmake --build --preset release    # full release build
cmake --build --preset debug --target run                   # run testgame
```

**Rationale**:  
`CMakePresets.json` was introduced in CMake 3.19 and is fully supported in 3.20+. It eliminates the need to remember `-DCMAKE_BUILD_TYPE=Debug -B build/debug` incantations. VS Code's CMake Tools extension also automatically discovers presets, replacing the existing `tasks.json` make commands. Ninja is chosen as the generator because it is fast, cross-platform, and available everywhere (bundled with many toolchains).

```json
{
  "version": 3,
  "configurePresets": [
    {
      "name": "debug",
      "displayName": "Debug",
      "generator": "Ninja",
      "binaryDir": "${sourceDir}/build/debug",
      "cacheVariables": { "CMAKE_BUILD_TYPE": "Debug" }
    },
    {
      "name": "release",
      "displayName": "Release",
      "generator": "Ninja",
      "binaryDir": "${sourceDir}/build/release",
      "cacheVariables": { "CMAKE_BUILD_TYPE": "Release" }
    }
  ],
  "buildPresets": [
    { "name": "debug",   "configurePreset": "debug"   },
    { "name": "release", "configurePreset": "release" }
  ]
}
```

**Alternatives considered**:

- Unix Makefiles generator — works but slower and not cross-platform (unavailable on Windows without MinGW Make in PATH).
- No presets file, just a README with cmake commands — valid but violates FR-001/FR-002 (single-command requirement) and poor developer experience.

---

## R-004: Custom "Run" Target

**Decision**: Define a `run` target in the root `CMakeLists.txt` using `add_custom_target` with `$<TARGET_FILE:testgame>` generator expression and `USES_TERMINAL` so the game window receives I/O.

```cmake
add_custom_target(run
  COMMAND "$<TARGET_FILE:testgame>"
  DEPENDS testgame
  WORKING_DIRECTORY "$<TARGET_FILE_DIR:testgame>"
  COMMENT "Launching testgame..."
  USES_TERMINAL
)
```

**Rationale**:  
`$<TARGET_FILE:testgame>` is a generator expression resolved at build time to the full path of the executable regardless of config or platform. `WORKING_DIRECTORY "$<TARGET_FILE_DIR:testgame>"` ensures the executable runs from its own directory, which is required so it can find SDL2.dll and glew32.dll next to it on Windows. `USES_TERMINAL` passes stdin/stdout to the terminal for interactive output.

**Alternatives considered**:

- Shell script wrapper — not cross-platform, requires separate maintenance.
- `cmake -E` invocation — adds indirection without benefit.

---

## R-005: DLL Copy at Build Time (Windows)

**Decision**: Use `add_custom_command(TARGET testgame POST_BUILD ...)` in the testgame `CMakeLists.txt` with `${CMAKE_COMMAND} -E copy_if_different` and `$<TARGET_FILE_DIR:testgame>` to copy SDL2.dll and glew32.dll after each build. Guard the block with `if(WIN32)`.

```cmake
# testgame/CMakeLists.txt
if(WIN32)
  set(_DEPS_DIR "${CMAKE_SOURCE_DIR}/engine/deps")
  set(_DEP_ARCH "windows-mingw64")

  foreach(_dll
    "${_DEPS_DIR}/SDL2/lib/${_DEP_ARCH}/SDL2.dll"
    "${_DEPS_DIR}/glew/lib/${_DEP_ARCH}/glew32.dll"
  )
    add_custom_command(TARGET testgame POST_BUILD
      COMMAND "${CMAKE_COMMAND}" -E copy_if_different
        "${_dll}"
        "$<TARGET_FILE_DIR:testgame>"
      COMMENT "Copying ${_dll} to output directory"
    )
  endforeach()
endif()
```

**Rationale**:  
`copy_if_different` avoids unnecessary copies on incremental builds. `$<TARGET_FILE_DIR:testgame>` correctly resolves to `bin/debug` or `bin/release` based on the active config. The `foreach` over a list is cleaner than repeating `add_custom_command` per DLL.

**Alternatives considered**:

- Copying at configure time with `configure_file` — copies once, not after every rebuild.
- CMake `install()` rule — designed for distribution, not for in-tree dev workflow.

---

## R-006: MinGW-w64 Toolchain Considerations

**Decision**: No dedicated toolchain file required for the primary Windows/MinGW-w64 workflow. Add a `CMakeUserPresets.json.example` file documenting how to override the compiler if needed. Set MinGW-specific flags via `target_compile_options` in the CMakeLists files.

**Rationale**:  
CMake auto-detects `g++` and `gcc` from PATH on MinGW-w64. A toolchain file is only required when cross-compiling or when the compiler is not in PATH. Since all current developers use the same MinGW-w64 setup, auto-detection is sufficient. The flags `-static-libgcc` and `-static-libstdc++` should be added as linker options to the testgame target on Windows/MinGW to avoid distributing compiler runtime DLLs alongside the game.

```cmake
# testgame/CMakeLists.txt
if(WIN32 AND CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  target_link_options(testgame PRIVATE -static-libgcc -static-libstdc++)
endif()
```

**`-mwindows` decision**: Do NOT add `-mwindows`. The testgame renders to an SDL2 window but may also print to stdout for debugging. Suppressing the console would hide useful output during development. If a release mode with no console is desired, that can be added as a future enhancement.

**Alternatives considered**:

- Dedicated `cmake/MinGW.cmake` toolchain file — adds complexity not justified while only one toolchain is in active use.
- Conditional in `CMakePresets.json` — environment variables can detect MinGW, but this is over-engineering for the current scope.

---

## Summary: Resolved Unknowns

| Unknown | Resolution |
|---------|------------|
| Vendored dep handling | IMPORTED targets in `engine/cmake/VendoredDeps.cmake`; declared PUBLIC on AIEngine for transitive propagation to testgame |
| Testgame link scope | testgame declares only `AIEngine`; all SDL2/GLEW/system deps propagate via AIEngine's PUBLIC link interface |
| Output directory structure | `$<LOWER_CASE:$<CONFIG>>` generator expression at root |
| Simple debug/release commands | `CMakePresets.json` with Ninja generator |
| Run target | `add_custom_target(run ...)` with `$<TARGET_FILE:testgame>` |
| DLL copying on Windows | `POST_BUILD` custom command with `copy_if_different` |
| MinGW-w64 specifics | Auto-detect; add `-static-libgcc/-static-libstdc++` for release |
