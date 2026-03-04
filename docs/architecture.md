# AIEngine Architecture Documentation

**Version**: 1.0.0  
**Date**: March 4, 2026  
**Status**: Production Ready  

## Overview

AIEngine is a modern C++ game engine built around a scene graph architecture with a component-based design. The engine emphasizes educational value, performance, and modern C++ practices while providing a solid foundation for 3D graphics programming.

## Core Architecture

### Engine Design Philosophy

- **Scene Graph Centric**: Hierarchical tree structure for managing 3D objects with automatic transform inheritance
- **Component-Based**: Modular functionality through attachable components at the node level
- **Modern C++**: Extensive use of C++17 features, RAII, smart pointers, and const correctness
- **Platform Abstraction**: SDL2-based windowing with cross-platform OpenGL rendering

### High-Level System Overview

```text
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   Test Game     │───▶│   AIEngine      │───▶│  Platform APIs  │
│   Application   │    │   Core Library  │    │  (SDL2/OpenGL)  │
└─────────────────┘    └─────────────────┘    └─────────────────┘
        │                       │                       │
        │              ┌─────────────────┐              │
        └─────────────▶│  Scene Graph    │◀─────────────┘
                       │   Component     │
                       │    System       │
                       └─────────────────┘
```

## Component System Design 

### Scene Graph Architecture

The engine uses a hierarchical scene graph where each node can contain:

- **Transform Component**: Position, rotation, scale with automatic matrix inheritance
- **Render Component**: Mesh references, material properties, visibility
- **Child Nodes**: Hierarchical parent-child relationships
- **Custom Components**: Extensible component system for game-specific logic

#### SceneNode Structure

```cpp
class SceneNode {
    std::string name;                           // Debug identification
    SceneNode* parent;                          // Hierarchical parent
    std::vector<std::unique_ptr<SceneNode>> children;  // Child nodes
    std::unordered_map<ComponentType, std::unique_ptr<IComponent>> components;
    
    // Transform inheritance
    glm::mat4 localTransform;                   // Local transformation
    glm::mat4 worldTransform;                   // Computed world transform
};
```

#### Transform Inheritance

World transforms are automatically calculated through parent hierarchy:

```text
Root Transform (Identity)
├── Camera Node (View Matrix)
└── Object Node (Model Matrix)
    ├── Child A (Inherits Parent Transform)
    └── Child B (Inherits Parent Transform)
        └── Grandchild (Inherits B's Transform)
```

### Component Lifecycle

1. **Creation**: Components instantiated and attached to nodes
2. **Initialization**: Setup phase for component data and resources
3. **Update**: Per-frame updates during scene graph traversal
4. **Render**: Rendering-specific components processed by renderer
5. **Cleanup**: Automatic cleanup through RAII when node destroyed

## Rendering Pipeline

### Graphics System Overview

The rendering system follows a forward rendering pipeline optimized for educational clarity:

```text
Frame Start
    ↓
Clear Buffers (Color + Depth)
    ↓
Update Scene Graph (Transform Propagation)
    ↓
Depth-First Scene Traversal
    ├── Process Transform Component
    ├── Calculate World Matrix
    └── Submit to Renderer (if has RenderComponent)
    ↓
Render All Submitted Geometry
    ├── Bind Shader Program
    ├── Set MVP Matrices
    ├── Bind Vertex Array/Buffers
    └── Draw Indexed Geometry
    ↓
Swap Buffers
    ↓
Frame Complete
```

### OpenGL Integration

- **Context Management**: SDL2 creates OpenGL 3.3 Core context
- **Function Loading**: GLEW provides OpenGL function pointers
- **Buffer Management**: RAII-based VBO/VAO management
- **Shader System**: Compile-time GLSL shader compilation and linking

#### Shader Pipeline

```cpp
Vertex Shader (basic_vertex.glsl)
    Input: Position, Normal, UV coordinates
    Process: MVP transformation, lighting calculations
    Output: Clip space position, world normal, UV

Fragment Shader (basic_fragment.glsl)  
    Input: Interpolated vertex data
    Process: Phong lighting, texture sampling
    Output: Final pixel color
```

### Geometry Management

- **Mesh System**: Vertex/index buffer management with OpenGL VBO/VAO
- **Geometry Factory**: Procedural geometry generation (cubes, spheres, etc.)
- **Memory Management**: RAII ensures proper cleanup of GPU resources

## Performance Characteristics

### Current Benchmarks

- **Target Performance**: 60+ FPS for basic scenes
- **Achieved Performance**: 144+ FPS (3D rotating cube)
- **Memory Usage**: Minimal heap allocations during runtime
- **Scene Complexity**: Optimized for 100+ nodes in scene graph

### Optimization Strategies

1. **Component Locality**: Components stored contiguously per node
2. **Transform Caching**: World transforms cached until hierarchy changes
3. **Culling**: Frustum culling planned for complex scenes
4. **Batching**: Geometry batching for similar render objects (future)

## Platform Abstraction

### Window System (SDL2)

```cpp
class Window {
    SDL_Window* sdl_window;     // SDL window handle
    SDL_GLContext gl_context;   // OpenGL context
    bool vsync_enabled;         // VSync state
    WindowConfig config;        // Window properties
};
```

### Input Handling (SDL2)

- **Event-driven**: SDL event polling for input
- **Extensible**: Input system designed for multiple input types
- **Platform Support**: Mouse, keyboard, gamepad support planned

## Building and Dependencies

### Build System

- **Primary**: GNU Make with Windows MinGW-w64 support  
- **Dependencies**: MSYS2 package management for cross-platform libraries
- **Configurations**: Debug/Release builds with appropriate optimization flags

### External Dependencies

| Library | Purpose | Version |
|---------|---------|---------|
| SDL2 | Windowing, Input, Context Creation | 2.30.10+ |
| GLEW | OpenGL Function Loading | 2.2.0+ |
| GLM | Mathematics (Vectors, Matrices) | 1.0.1+ |
| Doctest | Unit Testing Framework | 2.4+ |

## Educational Value

### Learning Objectives

1. **3D Graphics Programming**: Understand MVP matrices, rendering pipelines
2. **Component Architecture**: Learn modular game object design  
3. **Memory Management**: Modern C++ RAII and smart pointer usage
4. **Performance Analysis**: Frame timing, profiling, optimization techniques
5. **Cross-Platform Development**: Platform abstraction layer concepts

### Code Quality Features

- **Documentation**: Comprehensive inline documentation
- **Error Handling**: Proper exception handling and error reporting
- **Testing**: Unit and integration tests for key systems
- **Modern C++**: Extensive use of C++17 features and best practices

## Extension Points

### Adding New Components

```cpp
class CustomComponent : public IComponent {
public:
    void Update(float deltaTime) override;
    void Initialize() override;
    // Custom component logic
};

// Register with scene node
node->AddComponent<CustomComponent>();
```

### Custom Rendering

- **Shader Variations**: Add new vertex/fragment shader pairs
- **Material System**: Extend material properties for different render effects
- **Post-Processing**: Add framebuffer-based post-processing effects

### Platform Extensions

- **Audio System**: Integrate OpenAL or SDL2 audio
- **Physics**: Add physics engine integration (Bullet, PhysX)
- **Threading**: Implement multi-threaded scene updates

## Troubleshooting

### Common Issues

**Build Errors**: Ensure MSYS2 dependencies are installed correctly
**Runtime Crashes**: Check OpenGL context creation and driver support
**Performance Issues**: Profile with frame timing and GPU debuggers
**Memory Leaks**: Use RAII patterns and validate resource cleanup

### Debug Features

- **Frame Timing**: Built-in FPS counter and delta time monitoring
- **Scene Graph Inspection**: Node hierarchy visualization
- **OpenGL Debugging**: Error checking after OpenGL calls
- **Component Status**: Per-component update timing and status

## Future Roadmap

### Planned Features

- **Asset Loading**: 3D model loading (GLTF/OBJ)
- **Animation System**: Skeletal and keyframe animation
- **Physics Integration**: Collision detection and response
- **Audio System**: 3D positional audio
- **Scripting**: Lua or JavaScript integration for game logic

### Architectural Improvements

- **ECS Migration**: Optional Entity-Component-System architecture
- **Multi-threading**: Parallel scene graph updates
- **Vulkan Backend**: Modern graphics API support
- **Editor Integration**: Real-time scene editing tools

---

**Contributors**: AIEngine Development Team  
**License**: MIT License  
**Repository**: https://github.com/your-org/AIEngine