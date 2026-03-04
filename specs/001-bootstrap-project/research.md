# Research: Bootstrap Basic Project

**Date**: 2026-03-04  
**Feature**: [Bootstrap Basic Project](spec.md)  
**Phase**: 0 - Outline & Research  
**Clarifications Resolved**: Testing framework selection, C++ game engine best practices

## Needs Clarification Resolutions

### Testing Framework Decision

**Decision**: Doctest single-header testing framework

**Rationale**: Doctest provides optimal balance for learning-focused game engine development:
- **Fastest compilation**: 20-40x faster than alternatives, crucial for learning iteration cycles
- **Zero build friction**: Single header inclusion, no Make complexity or external dependencies  
- **Educational clarity**: BDD-style syntax with excellent error messages aids understanding
- **Modern C++17+ compatibility**: Full support for project language requirements
- **Flexible scope**: Equally effective for unit tests and integration scenarios

**Alternatives considered**: Google Test (over-engineered, complex build), Catch2 v3 (no longer single-header), utest.h (too minimal, lacking features)

**Implementation**: Include single `doctest.h` header in test files, add simple Make targets for test compilation

---

## C++ Game Engine Development Best Practices

This research covers essential best practices for implementing a game engine using modern C++ with SDL2, OpenGL, GLM, and cross-platform build systems.

## 1. SDL2 + OpenGL Integration

### Key Best Practices

**Window and Context Management:**
- Use RAII for SDL/OpenGL resource management with custom deleter classes
- Create OpenGL context after SDL window creation but before any GL calls
- Set OpenGL attributes before window creation (version, profile, buffer sizes)
- Use SDL_GL_SwapWindow() for proper double buffering
- Handle window events properly in main event loop

```cpp
class Window {
    std::unique_ptr<SDL_Window, void(*)(SDL_Window*)> window_;
    SDL_GLContext gl_context_;
public:
    Window(int width, int height, const std::string& title)
        : window_(nullptr, SDL_DestroyWindow) {
        // Set OpenGL attributes first
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        
        window_.reset(SDL_CreateWindow(title.c_str(), 
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            width, height, SDL_WINDOW_OPENGL));
            
        gl_context_ = SDL_GL_CreateContext(window_.get());
        SDL_GL_SetSwapInterval(1); // V-Sync
    }
    
    ~Window() {
        if (gl_context_) SDL_GL_DeleteContext(gl_context_);
    }
};
```

**Graphics State Management:**
- Initialize OpenGL function pointers using GLAD or similar
- Set initial OpenGL state once after context creation
- Use state caching to avoid redundant GL calls
- Group state changes to minimize driver overhead

### Common Pitfalls to Avoid

- Creating OpenGL context before setting attributes
- Forgetting to initialize OpenGL function loaders
- Not handling window resize events for viewport updates
- Missing proper cleanup of SDL/OpenGL resources
- Calling OpenGL functions before context creation

### Performance Considerations

- Minimize SDL_PollEvent() calls per frame
- Use SDL_GL_SwapWindow() instead of glFinish() for frame synchronization
- Cache SDL window properties to avoid repeated queries
- Handle high-DPI displays with proper scaling

## 2. GLM Mathematics Library

### Key Best Practices

**Vector and Matrix Operations:**
- Use GLM types consistently for all math operations
- Enable appropriate SIMD optimizations with GLM_FORCE_SIMD
- Use GLM_FORCE_RADIANS for consistent angle units
- Prefer const references for function parameters

```cpp
#define GLM_FORCE_RADIANS
#define GLM_FORCE_SIMD
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Transform {
    glm::vec3 position_{0.0f};
    glm::quat rotation_{1.0f, 0.0f, 0.0f, 0.0f};
    glm::vec3 scale_{1.0f};
    mutable glm::mat4 matrix_{1.0f};
    mutable bool dirty_ = true;

public:
    const glm::mat4& GetMatrix() const {
        if (dirty_) {
            matrix_ = glm::translate(glm::mat4(1.0f), position_) *
                     glm::mat4_cast(rotation_) *
                     glm::scale(glm::mat4(1.0f), scale_);
            dirty_ = false;
        }
        return matrix_;
    }
};
```

**Quaternion Usage:**
- Always normalize quaternions after mathematical operations
- Use GLM's quaternion functions for rotations instead of Euler angles
- Store rotations as quaternions to avoid gimbal lock
- Use slerp (spherical linear interpolation) for smooth rotations

### Common Pitfalls to Avoid

- Mixing degrees and radians (always use radians with GLM_FORCE_RADIANS)
- Forgetting to normalize vectors and quaternions
- Creating transformation matrices every frame instead of caching
- Using Euler angles for GameObject rotations (causes gimbal lock)
- Not understanding GLM's column-major matrix storage

### Recommended Implementation Approaches

- Separate transform components (position, rotation, scale) for easier manipulation
- Use lazy evaluation for expensive matrix calculations
- Implement operator overloads for common transform operations
- Use GLM's value_ptr() for OpenGL uniform uploads

### Performance Considerations

- Cache computed matrices and only recalculate when dirty
- Use GLM's fast functions (fastNormalize, etc.) where precision allows
- Enable compiler vectorization with appropriate GLM defines
- Minimize temporary object creation in math operations

## 3. Modern C++ in Game Engines

### Key Best Practices

**C++17/20 Features for Game Engines:**

**std::optional for Safe Returns:**
```cpp
std::optional<Component*> GetComponent(ComponentType type) {
    auto it = components_.find(type);
    return (it != components_.end()) ? std::make_optional(it->second) : std::nullopt;
}
```

**std::variant for Polymorphic Data:**
```cpp
using Event = std::variant<KeyPressEvent, MouseMoveEvent, WindowResizeEvent>;
void HandleEvent(const Event& event) {
    std::visit([](auto&& arg) { arg.Handle(); }, event);
}
```

**Smart Pointers for Resource Management:**
```cpp
class ResourceManager {
    std::unordered_map<std::string, std::shared_ptr<Texture>> textures_;
public:
    std::shared_ptr<Texture> LoadTexture(const std::string& path) {
        auto it = textures_.find(path);
        if (it != textures_.end()) return it->second;
        
        auto texture = std::make_shared<Texture>(path);
        textures_[path] = texture;
        return texture;
    }
};
```

**Concepts for Type Safety (C++20):**
```cpp
template<typename T>
concept Component = requires(T t) {
    { t.GetType() } -> std::convertible_to<ComponentType>;
    { t.Update(0.0f) } -> std::same_as<void>;
};

template<Component T>
void RegisterComponent() { /* ... */ }
```

### Common Pitfalls to Avoid

- Overusing std::shared_ptr when std::unique_ptr suffices
- Not using move semantics for large objects
- Ignoring RAII principles for resource management
- Using raw pointers for ownership
- Overcomplicating with unnecessary template metaprogramming

### Recommended Implementation Approaches

- Use unique_ptr for single ownership, shared_ptr only when needed
- Implement move constructors and assignment operators for heavy objects
- Use const correctness throughout the codebase
- Apply RAII for all resource types (textures, buffers, contexts)
- Leverage type traits for compile-time optimizations

### Performance Considerations

- Prefer stack allocation over heap when possible
- Use std::move for expensive-to-copy objects
- Consider small object optimization for frequently allocated types
- Use noexcept specifiers for move operations
- Profile memory allocations and minimize heap usage in hot paths

## 4. Scene Graph Implementation

### Key Best Practices

**Node Hierarchy Design:**
```cpp
class SceneNode {
    std::vector<std::unique_ptr<SceneNode>> children_;
    SceneNode* parent_ = nullptr;
    Transform local_transform_;
    mutable Transform world_transform_;
    mutable bool world_dirty_ = true;

public:
    void AddChild(std::unique_ptr<SceneNode> child) {
        child->parent_ = this;
        child->MarkWorldDirty();
        children_.push_back(std::move(child));
    }
    
    const Transform& GetWorldTransform() const {
        if (world_dirty_) {
            if (parent_) {
                world_transform_ = parent_->GetWorldTransform() * local_transform_;
            } else {
                world_transform_ = local_transform_;
            }
            world_dirty_ = false;
        }
        return world_transform_;
    }
    
    void SetLocalTransform(const Transform& transform) {
        local_transform_ = transform;
        MarkWorldDirty();
    }
};
```

**Component Attachment System:**
```cpp
class GameObject {
    std::unordered_map<std::type_index, std::unique_ptr<Component>> components_;
    SceneNode* scene_node_;

public:
    template<typename T, typename... Args>
    T* AddComponent(Args&&... args) {
        static_assert(std::is_base_of_v<Component, T>);
        auto component = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = component.get();
        components_[std::type_index(typeid(T))] = std::move(component);
        return ptr;
    }
    
    template<typename T>
    T* GetComponent() {
        auto it = components_.find(std::type_index(typeid(T)));
        return (it != components_.end()) ? 
               static_cast<T*>(it->second.get()) : nullptr;
    }
};
```

### Common Pitfalls to Avoid

- Deep recursion in transform calculations (use iterative approaches for large hierarchies)
- Updating all world transforms every frame instead of lazy evaluation
- Circular references in parent-child relationships
- Not invalidating child transforms when parent changes
- Poor cache locality from scattered node allocations

### Recommended Implementation Approaches

- Use dirty flags for lazy transform computation
- Implement depth-first traversal for rendering and updates
- Separate spatial partitioning from logical hierarchy when needed
- Use object pools for frequently created/destroyed nodes
- Consider data-oriented design for performance-critical systems

### Performance Considerations

- Cache world transforms and only update when dirty
- Minimize virtual function calls in hot paths
- Use iterative algorithms instead of recursion for deep trees
- Consider parallel updates for independent subtrees
- Implement spatial partitioning (octrees, etc.) for culling

## 5. Cross-Platform C++ Build Systems

### Key Best Practices using GNU Make

**Optimized Makefile Structure:**
```makefile
# Compiler and flags
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -pedantic
INCLUDES := -Isrc -Iinclude -I$(SDL2_PATH)/include -I$(GLM_PATH)

# Platform detection
UNAME := $(shell uname -s)
ifeq ($(UNAME), Linux)
    LIBS := -lSDL2 -lGL -ldl
    CXXFLAGS += -DLINUX
endif
ifeq ($(UNAME), Darwin)
    LIBS := -lSDL2 -framework OpenGL
    CXXFLAGS += -DMACOS
endif

# Build configurations
DEBUG_FLAGS := -g -DDEBUG -O0
RELEASE_FLAGS := -O3 -DNDEBUG -march=native

# Source and object files
SRCDIR := src
BUILDDIR := build
SOURCES := $(shell find $(SRCDIR) -name '*.cpp')
OBJECTS := $(SOURCES:$(SRCDIR)/%.cpp=$(BUILDDIR)/%.o)
DEPS := $(OBJECTS:.o=.d)

# Targets
TARGET := game_engine
.PHONY: all clean debug release

all: debug

debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: $(TARGET)

release: CXXFLAGS += $(RELEASE_FLAGS)
release: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $^ -o $@ $(LIBS)

# Automatic dependency generation
$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -MMD -MP -c $< -o $@

-include $(DEPS)

clean:
	rm -rf $(BUILDDIR) $(TARGET)
```

### Common Pitfalls to Avoid

- Not using automatic dependency generation (-MMD -MP)
- Hardcoding paths instead of using variables
- Missing platform-specific compiler flags and libraries
- Not separating debug and release builds
- Poor handling of header dependencies

### Recommended Implementation Approaches

- Use separate build directories for different configurations
- Implement automatic source file discovery
- Set up proper include paths and library linking
- Use conditional compilation for platform-specific code
- Include clean and install targets

### Performance Considerations

- Use parallel compilation with make -j
- Implement incremental builds with proper dependencies
- Use ccache for faster recompilation
- Optimize compiler flags for target platform
- Consider using precompiled headers for large projects

## Essential Patterns for Learning Developers

### 1. RAII Resource Management
Always wrap resources in RAII classes to ensure proper cleanup.

### 2. Component-Entity Systems
Separate data (components) from behavior (systems) for flexible game object design.

### 3. State Machines
Use enums and switch statements or state pattern for game state management.

### 4. Object Pooling
Pre-allocate objects to avoid garbage collection and improve performance.

### 5. Dirty Flag Pattern
Mark objects as needing updates only when they change, not every frame.

### 6. Observer Pattern
Implement event systems for loose coupling between game systems.

### 7. Command Pattern
Use for undoable actions, input handling, and AI decision making.

These patterns form the foundation of well-architected game engines that are both performant and maintainable.

**Date**: 2026-03-03  
**Feature**: [Bootstrap Basic Project](../spec.md)  
**Phase**: 0 - Outline & Research  

## Research Questions Resolved

### 1. C++ Testing Framework Selection

**Decision**: Doctest ⭐ **CONFIRMED CHOICE**

**Comprehensive Rationale**:

**Why Doctest is Perfect for Learning-Focused Game Engine:**
- **True single-header**: Just `#include "doctest.h"` - zero build complexity
- **Fastest compilation**: Orders of magnitude faster than alternatives (20-40x)
- **Educational focus**: Designed for rapid iteration and learning cycles
- **Modern C++17+**: Full compatibility with project requirements
- **Excellent error messages**: Clear feedback helps students understand failures
- **Flexible testing**: Supports both unit tests and integration scenarios
- **BDD-style syntax**: Natural, readable test structure aids learning
- **Zero dependencies**: No external libraries or complex setup required

**Detailed Alternatives Analysis**:

**Google Test**: 
- ❌ Complex build requirements (CMake-centric)
- ❌ Slow compilation time hurts learning iteration
- ❌ Over-engineered for educational context
- ❌ Make integration requires complex dependency management
- ✅ Industry standard (but not needed for learning)

**Catch2 v3**: 
- ❌ No longer single-header (major regression from v2)
- ❌ Requires library compilation and linking
- ❌ More complex Make integration than doctest
- ✅ Good documentation and community
- ✅ Natural syntax (similar to doctest)

**utest.h**: 
- ✅ Ultra-minimal single header
- ❌ Too basic - lacks features needed for comprehensive game engine testing
- ❌ Limited assertion types and error messages
- ❌ Doesn't leverage modern C++ effectively

**Supporting Benchmarks**: Doctest compiles 20-40x faster than Google Test and Catch2, making it ideal for the rapid learning iteration cycles essential to educational game engine development.

### 2. Scene Graph Architecture Pattern

**Decision**: Hierarchical Scene Graph with Component Nodes

**Rationale**:

- Scene graph provides automatic transform inheritance essential for 3D graphics
- Nodes can contain components (transform, render) combining both architectural patterns  
- Hierarchical structure teaches parent-child relationships critical in game engines
- Matrix propagation scales efficiently for complex scenes
- Clear learning progression from basic nodes to advanced scene management
- Compatible with component-driven principles at the node level

**Alternatives considered**:

- Flat Component-Entity System: Lacks hierarchical transforms and automatic inheritance
- Pure inheritance-based hierarchy: Doesn't demonstrate component composition patterns
- Full ECS (EnTT): Too complex for initial learning phase and lacks scene hierarchy

### 3. Memory Management Strategy

**Decision**: Smart Pointer RAII Approach

**Rationale**:

- `std::unique_ptr` for clear ownership (Scene owns Nodes, Nodes own child Nodes)
- `std::shared_ptr` for shared resources (meshes, textures, materials)
- `std::weak_ptr` for parent references to prevent circular dependencies
- `std::weak_ptr` to prevent circular references
- Eliminates manual memory management complexity
- Teaches modern C++ safety practices

**Alternatives considered**:

- Manual memory management: Educational value but error-prone and not modern practice
- Garbage collection: Not available in C++, would require external library

### 4. Public API Design

**Decision**: Modular Header Organization with Namespace Isolation  

**Rationale**:

- Clear separation between public API (`include/`) and implementation (`src/`)
- Namespace organization prevents symbol pollution
- Modular structure supports incremental learning
- Professional patterns that scale to larger projects

**Architecture**:

```
engine/include/AIEngine/
├── core/ (Engine, Entity, Component interfaces)
├── graphics/ (Renderer, Mesh, Shader)  
├── math/ (Transform utilities)
└── AIEngine.hpp (convenience header)
```

### 5. Performance Strategy for 60fps Target

**Decision**: Data-Oriented Design for Hot Paths

**Rationale**:

- Structure of Arrays (SoA) for transform operations improves cache performance
- Pre-allocated containers minimize garbage collection pressure
- Clear separation between cold startup code and hot game loop code
- Teaches performance-conscious programming patterns

**Key patterns**:

- Batch processing for component updates
- Minimize allocations in render loop
- Cache-friendly data layouts for frequently accessed data

## Implementation Dependencies

Based on research, the technical context dependencies are:

**Graphics**: OpenGL 3.3+ with SDL2 (windowing/input) and GLAD (function loading)
**Testing**: Doctest (single-header, no external dependencies)  
**Build System**: GNU Make (already available)
**Math Library**: GLM (header-only linear algebra)
**Platform Support**: Windows 11 primary, designed for cross-platform expansion

## Learning Objectives Alignment

This research supports the constitution's educational goals:

1. **Modern C++ First**: Smart pointers, RAII, templates, move semantics
2. **Cross-Platform Compatibility**: SDL2+OpenGL graphics API, platform abstraction patterns  
3. **Component-Driven Architecture**: Scene graph with component nodes, hierarchical composition, automatic inheritance
4. **Performance-Conscious Development**: Efficient transform propagation, cache-friendly node updates
5. **Test-Driven Learning**: Doctest framework integration, unit testing patterns

All decisions prioritize educational value while maintaining professional development practices suitable for scene graph-based game engine architecture.
