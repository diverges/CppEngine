# Implementation Plan: Bootstrap Basic Project

**Branch**: `001-bootstrap-project` | **Date**: 2026-03-04 | **Spec**: [spec.md](spec.md)
**Input**: Feature specification from `/specs/001-bootstrap-project/spec.md`

**Note**: This template is filled in by the `/speckit.plan` command. See `.specify/templates/plan-template.md` for the execution workflow.

## Summary

Bootstrap a learning-focused C++ game engine with two main components: a reusable game engine library and a separate test game application. The test game will demonstrate engine functionality by rendering a 3D cube through a scene graph interface. Focus on modern C++ practices (C++17+), cross-platform design with OpenGL/SDL2, and educational value for game engine development concepts.

## Technical Context

**Language/Version**: C++17 minimum (C++20 preferred) with modern practices (RAII, smart pointers, const correctness)  
**Primary Dependencies**: SDL2 (windowing/input), OpenGL 3.3+ (graphics), GLM (mathematics), GLAD/GLEW (OpenGL loading)  
**Storage**: N/A (initial version uses in-memory scene graph and geometry data)  
**Testing**: Doctest (single-header framework, fast compilation, excellent for educational context)  
**Target Platform**: Windows 11 development environment, designed for cross-platform expansion  
**Project Type**: Game engine library + test application (separate builds, library consumption pattern)  
**Performance Goals**: 60fps rendering for simple cube scene on standard gaming hardware (OpenGL 3.3+, 4GB RAM, integrated graphics)  
**Constraints**: Scene graph architecture requirement, educational focus, component-driven design principles  
**Scale/Scope**: Basic foundation (~1000 LOC initially), single test scene with 3D cube, hierarchical transforms

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

✅ **Modern C++ First**: Feature uses C++17+ with industry standard practices, smart pointers, RAII  
✅ **Cross-Platform Compatibility**: SDL2+OpenGL provides cross-platform foundation, Windows-first with portable design  
✅ **Component-Driven Architecture**: RESOLVED - Scene graph nodes contain components, fulfilling component composition principle  
✅ **Performance-Conscious Development**: 60fps target with scene graph optimization for efficient hierarchy traversal  
✅ **Test-Driven Learning**: Doctest framework integration with educational focus maintained

**GATE STATUS**: ✅ PASSED - All constitutional principles satisfied

**Post-Phase 1 Validation**:
- ✅ Scene graph architecture IS component-driven: nodes contain transform and render components
- ✅ Hierarchical composition teaches advanced patterns beyond flat component systems
- ✅ Automatic transform inheritance provides performance benefits for tree traversal
- ✅ Educational value maintained: both hierarchy management AND component patterns demonstrated
- ✅ All design artifacts updated to reflect scene graph architecture consistently

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
# C++ Game Engine with Scene Graph Architecture
engine/                  # Game engine library
├── src/
│   ├── core/           # Core engine systems (Engine, Application)
│   ├── scene/          # Scene graph implementation (Node, SceneGraph)
│   ├── graphics/       # Rendering system (Renderer, Mesh, Shader)
│   ├── math/           # Mathematics utilities (Transform, Matrix)
│   └── platform/       # Platform abstraction (Window, Input)
├── include/            # Public header files
│   └── AIEngine/
│       ├── core/       # Core system headers
│       ├── scene/      # Scene graph interface
│       ├── graphics/   # Graphics API
│       └── math/       # Math utilities
├── shaders/            # GLSL shader files
└── Makefile           # Engine library build

testgame/              # Test game application  
├── src/               # Test game source (main, Game class)
└── Makefile          # Test game build (links to engine)

tests/                 # Testing infrastructure
├── unit/              # Unit tests for engine components
├── integration/       # Integration tests
└── fixtures/          # Test data and helpers

docs/                  # Documentation
├── architecture.md    # Scene graph design documentation
└── examples/          # Usage examples
```

**Structure Decision**: Chose scene graph engine structure with hierarchical node-based architecture. Engine follows C++ library patterns with src/include separation. Scene graph provides automatic transform inheritance and component attachment at node level. This balances learning objectives for both hierarchical design and component patterns.

## Complexity Tracking

**Constitutional Violation Requiring Justification**

| Violation | Why Needed | Simpler Alternative Rejected Because |
|-----------|------------|-------------------------------------|
| Scene Graph vs Component-Entity System | Scene graph provides hierarchical transforms and automatic inheritance essential for 3D graphics learning | Flat ECS wouldn't teach transform hierarchies, parent-child relationships, or automatic matrix propagation critical in game engines |

**Justification**: Scene graph architecture IS component-driven at the node level where each scene node can contain transform and render components. This approach teaches both hierarchical composition patterns AND component attachment, providing superior educational value for game engine development. The architecture satisfies component-driven principles while adding essential 3D graphics concepts.
