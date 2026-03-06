# Implementation Plan: Input-Controlled Camera Component

**Branch**: `005-input-camera` | **Date**: March 4, 2026 | **Spec**: [spec.md](spec.md)
**Input**: Feature specification from `/specs/005-input-camera/spec.md`

**Note**: This template is filled in by the `/speckit.plan` command. See `.specify/templates/plan-template.md` for the execution workflow.

## Summary

Implement a camera component with WASD movement and mouse rotation controls through an extensible input subsystem that integrates with AIEngine's engine subsystem architecture. The input subsystem follows the established IEngineSubsystem interface with provider pattern for hardware abstraction via SDL2InputProvider. Components access the input system through GetEngineSubsystem<IInputSubsystem>() and register for semantic Move/Rotate events. This architecture enables future input device extensibility (game controllers) while maintaining clean separation between input sources and camera behavior, following the same patterns as graphics and audio subsystems.

## Technical Context

**Language/Version**: C++17 (with GLM for mathematics)  
**Primary Dependencies**: SDL2 (window/input), OpenGL/GLEW (rendering), doctest (testing)  
**Storage**: N/A (real-time camera control)  
**Testing**: doctest unit testing framework  
**Target Platform**: Cross-platform (Windows/Linux/macOS via SDL2)  
**Project Type**: Game engine component library  
**Performance Goals**: 60fps minimum camera responsiveness, <16ms input latency  
**Constraints**: Component-based architecture, extensible input subsystem design  
**Scale/Scope**: 4 simultaneous cameras, 100 scene nodes, 10K triangles/frame, 1000 input events/sec, <1MB memory
**Existing Architecture**: Component system with SceneNode, TransformComponent, Window/SDL2 input handling

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

### ✅ I. Modern C++17 First

- **Compliance**: Feature uses C++17, smart pointers for memory management, RAII patterns
- **Gate**: Input subsystem MUST use modern C++17 practices (smart pointers, RAII, C++17 features)
- **Post-Design**: ✅ Data model specifies glm::vec3/quat, std::function callbacks, smart pointer component management

### ✅ II. Cross-Platform Compatibility  

- **Compliance**: SDL2 provides cross-platform input abstraction, OpenGL graphics
- **Gate**: Input handling MUST work consistently across Windows/macOS/Linux via SDL2
- **Post-Design**: ✅ Contracts guarantee identical behavior across platforms, SDL2 abstraction maintained

### ✅ III. Component-Driven Architecture (NON-NEGOTIABLE)

- **Compliance**: Using existing component system, InputCameraComponent extends base component
- **Gate**: Camera input MUST be implemented as component, NOT inheritance-based system
- **Post-Design**: ✅ InputCameraComponent properly extends Component interface, composition-based event handling

### ✅ IV. Performance-Conscious Development

- **Compliance**: 60fps target specified, <16ms input latency requirement  
- **Gate**: Input processing MUST achieve <16ms latency, no frame rate degradation during input
- **Post-Design**: ✅ Contracts specify <1ms event dispatch, <5% overhead, zero runtime allocation guarantees

### ✅ V. Test-Driven Learning

- **Compliance**: Feature includes unit test requirements for input subsystem and camera component
- **Gate**: Input subsystem and camera component MUST have comprehensive unit test coverage
- **Post-Design**: ✅ Source structure includes dedicated test directories, integration and unit test coverage planned

**Constitution Status**: ✅ **FULL COMPLIANCE MAINTAINED** - All constitutional principles satisfied through design phase

## Project Structure

### Documentation (this feature)

```text
specs/005-input-camera/
├── plan.md              # This file (/speckit.plan command output)
├── research.md          # Phase 0 output (/speckit.plan command)
├── data-model.md        # Phase 1 output (/speckit.plan command)
├── quickstart.md        # Phase 1 output (/speckit.plan command)
├── contracts/           # Phase 1 output (/speckit.plan command)
└── tasks.md             # Phase 2 output (/speckit.tasks command - NOT created by /speckit.plan)
```

### Source Code (repository root)

```text
# AIEngine C++ Game Engine Structure (Engine Subsystem Integration)
engine/
├── include/AIEngine/
│   ├── input/
│   │   ├── IInputSubsystem.hpp        # Engine subsystem interface
│   │   ├── InputSubsystem.hpp         # Concrete subsystem implementation
│   │   ├── IInputProvider.hpp         # Provider interface  
│   │   ├── SDL2InputProvider.hpp      # SDL2 provider implementation
│   │   ├── InputEvent.hpp             # Base input event types
│   │   ├── MoveEvent.hpp              # Movement event definition
│   │   └── RotateEvent.hpp            # Rotation event definition
│   ├── components/
│   │   ├── CameraComponent.hpp        # Base camera component (future)
│   │   └── InputCameraComponent.hpp   # Input-controlled camera
│   └── core/
│       ├── Component.hpp              # Existing component base
│       ├── IEngineSubsystem.hpp       # Existing engine subsystem interface
│       ├── ISubsystemProvider.hpp     # Existing provider interface
│       ├── EngineSystemDescriptor.hpp # Existing system descriptor
│       └── EngineSystemManager.hpp    # Existing system manager
├── src/
│   ├── input/
│   │   ├── InputSubsystem.cpp         # Main subsystem implementation
│   │   ├── SDL2InputProvider.cpp      # SDL2 hardware integration
│   │   ├── InputEvent.cpp             # Event base implementation
│   │   ├── MoveEvent.cpp              # Move event implementation
│   │   └── RotateEvent.cpp            # Rotate event implementation
│   ├── components/
│   │   └── InputCameraComponent.cpp   # Input camera implementation
│   └── (existing engine modules)
└── tests/
    ├── unit/
    │   ├── input_subsystem_test.cpp   # Subsystem unit tests
    │   ├── input_provider_test.cpp    # Provider unit tests
    │   └── input_camera_test.cpp      # Camera component tests
    └── integration/
        └── input_system_integration_test.cpp # Full system tests
```

**Structure Decision**: Input system properly integrated as engine subsystem following established architecture patterns. Input handling uses provider pattern for hardware abstraction. Components access subsystem via EngineSystemManager for O(1) lookups. All lifecycle management handled by engine subsystem infrastructure.

## Complexity Tracking

### Implementation Prerequisites *(CRITICAL - BLOCKING)*

**⚠️ CRITICAL DEPENDENCY**: This feature is **BLOCKED** pending implementation of [006-subsystem-infrastructure](../006-subsystem-infrastructure/spec.md).

**Engine Subsystem Foundation Required**: Input system architecture depends on proper engine subsystem implementation from bootstrap project (001). The following **MUST** be implemented before any work on this feature begins:

**Required from [006-subsystem-infrastructure](../006-subsystem-infrastructure/spec.md)**:

1. **IEngineSubsystem Interface** - Base interface for all engine subsystems
2. **ISubsystemProvider Interface** - Base interface for subsystem providers  
3. **EngineSystemDescriptor** - System creation and type management
4. **EngineSystemManager** - Central subsystem registry and lifecycle management
5. **GetEngineSubsystem<T>()** - Template-based subsystem access

**Current Status**: 🔴 **BLOCKED** - Core subsystem infrastructure does not exist in codebase.

**Implementation Order (MANDATORY)**:

```text
🚫 BLOCKED: Cannot proceed until subsystem infrastructure is complete

1. [006-subsystem-infrastructure] Core Engine Subsystem Architecture (PREREQUISITE)
   ✅ Specification: Complete (see ../006-subsystem-infrastructure/spec.md)
   ⏳ Implementation: Required before this feature
   ⏳ Testing: Required before this feature

2. [005-input-camera] Input Subsystem Implementation (THIS FEATURE)
   🔴 Status: Blocked until step 1 is complete
   → IInputSubsystem interface
   → InputSubsystem implementation  
   → SDL2InputProvider implementation
   → InputCameraComponent integration
```

**⚠️ CRITICAL BLOCKING ISSUE**: This entire feature specification assumes the engine subsystem foundation exists. **NO WORK** should begin on the input camera feature until the subsystem infrastructure from [006-subsystem-infrastructure](../006-subsystem-infrastructure/spec.md) is fully implemented and tested.

**Next Action Required**:

1. Complete implementation of [006-subsystem-infrastructure](../006-subsystem-infrastructure/spec.md)
2. Verify all subsystem infrastructure contracts are working
3. Only then proceed with `/speckit.tasks` for this input camera feature

> **Fill ONLY if Constitution Check has violations that must be justified**

| Violation | Why Needed | Simpler Alternative Rejected Because |
|-----------|------------|-------------------------------------|
| [e.g., 4th project] | [current need] | [why 3 projects insufficient] |
| [e.g., Repository pattern] | [specific problem] | [why direct DB access insufficient] |
