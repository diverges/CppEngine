# AIEngine Development Guidelines

Auto-generated from all feature plans. Last updated: 2026-03-03

## Active Technologies
- C++17+ with industry standard practices (RAII, smart pointers, const correctness) + OpenGL (graphics), SDL2 (windowing/input), GLAD/GLEW (OpenGL loading), GLM (mathematics), Doctest (testing) (001-bootstrap-project)
- N/A (initial version uses in-memory scene graph and geometry data) (001-bootstrap-project)
- C++17 minimum (C++20 preferred) with modern practices (RAII, smart pointers, const correctness) + SDL2 (windowing/input), OpenGL 3.3+ (graphics), GLM (mathematics), GLAD/GLEW (OpenGL loading) (001-bootstrap-project)
- C++17 minimum (C++20 preferred per constitution) + SDL2, OpenGL, doctest (testing framework), currently checked into `engine/deps/` (003-git-ready-setup)
- N/A (game engine, no persistent storage requirements) (003-git-ready-setup)
- C++17 (with GLM for mathematics) + SDL2 (window/input), OpenGL/GLEW (rendering), doctest (testing) (005-input-camera)
- N/A (real-time camera control) (005-input-camera)
- C++17 (with modern C++17 features and practices) + Standard library (std::unordered_map, std::unique_ptr, std::function, type hashing) (006-subsystem-infrastructure)
- In-memory registry (no persistent storage required) (006-subsystem-infrastructure)

- C++17 minimum, C++20 preferred (GCC compiler available on Windows 11) + OpenGL (graphics), SDL2 (windowing/input), GLAD/GLEW (OpenGL loading) (001-bootstrap-project)

## Project Structure

```text
src/
tests/
```

## Commands

# Add commands for C++17 minimum, C++20 preferred (GCC compiler available on Windows 11)

## Code Style

C++17 minimum, C++20 preferred (GCC compiler available on Windows 11): Follow standard conventions

## Recent Changes
- 006-subsystem-infrastructure: Added C++17 (with modern C++17 features and practices) + Standard library (std::unordered_map, std::unique_ptr, std::function, type hashing)
- 005-input-camera: Added C++17 (with GLM for mathematics) + SDL2 (window/input), OpenGL/GLEW (rendering), doctest (testing)
- 003-git-ready-setup: Added C++17 minimum (C++20 preferred per constitution) + SDL2, OpenGL, doctest (testing framework), currently checked into `engine/deps/`


<!-- MANUAL ADDITIONS START -->
<!-- MANUAL ADDITIONS END -->
