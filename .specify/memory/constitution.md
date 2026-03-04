<!--
Sync Impact Report - Constitution v1.0.0 (Initial Creation)
- Version change: TEMPLATE → v1.0.0 
- New project: AIEngine C++ Game Engine
- Initial principles established for learning-focused game engine development
- Templates requiring updates: ✅ All validated during initial setup
- Follow-up TODOs: None
-->

# AIEngine Constitution

## Core Principles

### I. Modern C++ First

All code MUST utilize modern C++ standards (C++17 minimum, C++20 preferred) and follow established best practices. Smart pointers are mandatory for memory management. Raw pointers allowed only for non-owning references. RAII principles must be followed throughout the codebase.

**Rationale**: Modern C++ features improve safety, performance, and maintainability while serving the educational goal of learning current C++ practices.

### II. Cross-Platform Compatibility

All engine components MUST build and run consistently across Windows, macOS, and Linux. Platform-specific code must be clearly isolated and abstracted through unified interfaces. OpenGL is the mandated graphics API for cross-platform rendering.

**Rationale**: Cross-platform compatibility ensures broader learning opportunities and demonstrates proper abstraction techniques essential in game engine architecture.

### III. Component-Driven Architecture (NON-NEGOTIABLE)

The engine MUST follow component-based design patterns. Systems operate on components, not inheritance hierarchies. Entity-Component-System (ECS) architecture is preferred for game objects. Clear separation between engine core and game-specific logic is mandatory.

**Rationale**: Component-driven design scales better than inheritance hierarchies and teaches fundamental game engine architecture patterns.

### IV. Performance-Conscious Development

All core engine systems MUST be designed with performance in mind. Frame rate targets of 60fps minimum for simple scenes. Memory allocations during runtime should be minimized. Profiling tools must be integrated for performance measurement and optimization.

**Rationale**: Performance awareness is critical in game engine development and teaches optimization principles essential for real-time graphics programming.

### V. Test-Driven Learning

Unit tests are mandatory for all engine subsystems. Integration tests required for system interactions. Each feature must include example usage demonstrating the concept being learned. Documentation must explain both "what" and "why" for educational value.

**Rationale**: Testing ensures code quality while examples and documentation reinforce learning objectives and demonstrate proper usage patterns.

## Build System Standards

**Build Tool**: GNU Make is the mandated build system for simplicity and cross-platform support.
**Dependencies**: Minimal external dependencies; prefer standard library and well-established libraries (SDL2, GLEW/GLAD).
**Configuration**: Project must build with standard make commands without complex setup procedures.
**Toolchain**: Support for GCC, Clang, and MSVC compilers across target platforms.

## Development Workflow

**Version Control**: Git with descriptive commit messages following conventional commits format.
**Code Organization**: Clear directory structure separating engine core, examples, tests, and documentation.
**Code Reviews**: All changes must be reviewed for adherence to modern C++ practices and architecture principles.
**Documentation**: Each major system requires architectural documentation explaining design decisions and learning outcomes.

## Governance

This constitution supersedes all other development practices. Any deviations must be documented and justified. Amendments require clear rationale and migration plan for existing code. All features and modifications must demonstrate alignment with the learning objectives of modern C++ and game engine architecture.

**Version**: 1.0.0 | **Ratified**: 2026-03-03 | **Last Amended**: 2026-03-03
