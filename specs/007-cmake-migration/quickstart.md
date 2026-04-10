# Quickstart: CMake Build System

**Feature**: 007-cmake-migration  
**Replaces**: GNU Make (`make debug`, `make release`, `make run`)

---

## Prerequisites

| Tool | Minimum Version | Install |
|------|----------------|---------|
| CMake | 3.20 | <https://cmake.org/download/> |
| Ninja | any | <https://ninja-build.org/> or bundled with many toolchains |
| GCC (MinGW-w64) | 12+ | existing — same as Make setup |

Verify your environment:

```bash
cmake --version   # must be ≥ 3.20
ninja --version
g++ --version
```

---

## First-Time Setup (configure)

CMake separates _configure_ from _build_. You must configure once before building. Re-configure only if you add/remove source files.

```bash
# From the repository root

# Configure debug
cmake --preset debug

# Configure release
cmake --preset release
```

This creates `build/debug/` and `build/release/` with Ninja build files. You do this once per clone.

---

## Build

```bash
# Debug build  →  bin/debug/testgame.exe
cmake --build --preset debug

# Release build  →  bin/release/testgame.exe
cmake --build --preset release
```

Incremental: only changed files recompile. No need to clean between changes.

---

## Run

```bash
# Run the debug testgame
cmake --build --preset debug --target run

# Run the release testgame
cmake --build --preset release --target run
```

The `run` target builds (if needed) and then launches the executable from its output directory so DLLs are found automatically.

---

## VS Code Integration

The CMake Tools extension picks up `CMakePresets.json` automatically. After installing it:

1. Open the Command Palette → **CMake: Select Configure Preset** → choose `debug` or `release`
2. Click the **Build** button in the status bar (or press `F7`)
3. Run via the **Launch** button or **CMake: Run Without Debugging**

The `tasks.json` `make`-based tasks are replaced by these CMake-native workflows.

---

## Clean / Reconfigure

```bash
# Delete build artifacts for debug
Remove-Item -Recurse -Force build/debug, bin/debug   # PowerShell
rm -rf build/debug bin/debug                          # bash/zsh

# Then reconfigure
cmake --preset debug
```

There is no `make clean` equivalent — delete the preset's `build/` directory to start fresh.

---

## Complete Workflow Reference

| Old Make command | New CMake command |
|-----------------|-------------------|
| `make debug` | `cmake --preset debug && cmake --build --preset debug` |
| `make release` | `cmake --preset release && cmake --build --preset release` |
| `make run` | `cmake --build --preset debug --target run` |
| `make clean` | `Remove-Item -Recurse -Force build/debug bin/debug` |

---

## Troubleshooting

**"cmake: command not found"** — CMake is not in PATH. Install from cmake.org and restart your terminal.

**"ninja: command not found"** — Install Ninja or change the `generator` in `CMakePresets.json` to `"Unix Makefiles"`.

**"Could not find SDL2/GLEW"** — Deps are vendored under `engine/deps/`. Verify the directory structure matches what `engine/cmake/VendoredDeps.cmake` expects. Check that `windows-mingw64/` exists under `engine/deps/SDL2/lib/`.

**SDL2.dll not found at runtime** — The `POST_BUILD` copy step should handle this. If it failed, manually copy `engine/deps/SDL2/lib/windows-mingw64/SDL2.dll` to `bin/debug/`.

**Wrong architecture** — Ensure your `g++` is the same MinGW-w64 target as the pre-built libs (`x86_64-w64-mingw32`). Run `g++ -dumpmachine` — it must return `x86_64-w64-mingw32`.
