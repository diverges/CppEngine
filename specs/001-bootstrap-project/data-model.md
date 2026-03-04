# Data Model: Bootstrap Basic Project

**Date**: 2026-03-03  
**Feature**: [Bootstrap Basic Project](../spec.md)  
**Phase**: 1 - Design & Contracts  
**Input**: Feature specification and research findings

## Core Entities

### Engine Library

**Purpose**: Core game engine functionality packaged as a reusable library component

**Key Attributes**:

- Scene graph interfaces and hierarchical node system
- Rendering abstractions for 3D graphics  
- Mathematics utilities for transformations and matrix propagation
- Platform abstraction layer for cross-platform support
- Public API through organized header structure

**Relationships**:

- Provides interfaces used by Test Game Application
- Contains Scene Graph implementation with hierarchical nodes
- Exposes Renderer used for scene traversal and rendering

**State Transitions**:

- Initialize → Ready → Shutdown
- Compilation state: Source → Compiled Library → Linked

**Validation Rules**:

- Must compile without errors on target platforms
- Must expose consistent public API through headers
- Core systems must initialize before game logic can run

---

### Scene Graph

**Purpose**: Hierarchical tree structure for managing 3D objects with automatic transform inheritance

**Key Attributes**:

- Root node for scene organization
- Tree traversal algorithms for rendering and updates
- Automatic world transform calculation through parent hierarchy
- Component attachment support at node level
- Efficient culling and rendering order determination

**Relationships**:

- Owned by Engine Library and exposed through API
- Contains Scene Nodes in hierarchical tree structure
- Used by Test Game Application for cube organization
- Interfaces with Renderer for efficient scene traversal

**State Transitions**:

- Create → Add Root → Build Hierarchy → Update Loop → Render Traversal → Cleanup
- Node updates: Local Change → Propagate to Children → World Transform Update → Render

**Validation Rules**:

- Must maintain valid tree structure (no cycles)
- Transform propagation must be accurate through hierarchy
- Node updates must efficiently propagate to affected children

---

### Test Game Application  

**Purpose**: Minimal game application that demonstrates engine usage and validates library integration

**Key Attributes**:

- Game loop implementation (Update/Render cycle)
- Engine library integration and linking
- 3D cube scene setup and management
- Application lifecycle (startup/shutdown)

**Relationships**:

- Depends on Engine Library for core functionality
- Creates and manages Scene Graph for display
- Uses Renderer through engine interfaces
- Contains exactly one Scene Graph with cube Node

**State Transitions**:

- Build → Link → Execute → Running → Shutdown
- Scene state: Initialize → Load Cube Node → Render Loop → Cleanup

**Validation Rules**:

- Must link successfully against engine library
- Must achieve 60fps performance target for cube scene
- Must run without runtime errors on clean startup

---

### Scene Node

**Purpose**: Hierarchical node in scene graph that can contain components and child nodes (initially the cube node)

**Key Attributes**:

- Unique identifier for scene graph management
- Hierarchical parent/child relationships
- Local and world transform matrices
- Component attachments (Transform, Render, etc.)
- Render state (mesh reference, material properties)

**Relationships**:

- Owned by Scene Graph or parent Scene Node (hierarchical composition)
- Contains child Scene Nodes (hierarchical structure)
- Contains Components through aggregation
- References 3D Geometry data for rendering through render components

**State Transitions**:

- Create → Add to Parent → Attach Components → Active → Update Loop → Remove from Parent → Destroy
- Transform updates: Modify Local Transform → Propagate to Children → World Matrix Calculation → Render

**Validation Rules**:

- Must have valid Transform for positioning in hierarchy
- World transform must be calculated from parent chain
- Component references must remain valid during node lifetime
- Parent-child relationships must form valid tree (no cycles)

---

### Component

**Purpose**: Modular functionality blocks that attach to Scene Nodes to provide behavior  

**Key Attributes**:

- Component type identification
- Data specific to component functionality (transform data, render data)
- Update behavior for per-frame processing
- Lifetime tied to parent Scene Node

**Component Specializations**:

**Transform Component**:

- Local position (x, y, z coordinates relative to parent)
- Local rotation (euler angles or quaternion)  
- Local scale (uniform or per-axis scaling)
- World transform matrix (calculated from parent hierarchy)

**Render Component**:

- Mesh identifier for geometry data
- Material properties for appearance
- Visibility state and render settings

**Relationships**:

- Owned by Entity through composition
- Implements common Component interface
- Processed by corresponding Systems during game loop

**State Transitions**:

- Attach → Initialize → Update Loop → Detach
- Transform: Modify → Calculate Matrix → Apply to Rendering

**Validation Rules**:

- Components must implement required interface methods
- Transform data must result in valid transformation matrices
- Render components must reference valid geometry and material data

---

### 3D Geometry

**Purpose**: Vertex and index data that defines the shape and structure of the cube mesh

**Key Attributes**:

- Vertex positions (3D coordinates)
- Vertex attributes (normals, texture coordinates if applicable)
- Index data for triangle construction
- Buffer objects for GPU memory management

**Relationships**:

- Referenced by Render Components for drawing
- Managed by Renderer for GPU upload and binding
- Static data for initial cube implementation

**State Transitions**:

- Define → Upload to GPU → Bind for Rendering → Draw → Unbind
- Memory: CPU data → GPU buffer → Active binding → Render commands

**Validation Rules**:

- Vertex data must define valid cube geometry
- Index data must reference valid vertex indices  
- GPU buffers must be created successfully for rendering
- Geometry must result in visible cube when rendered

---

### Renderer

**Purpose**: Graphics system responsible for drawing 3D objects to the screen using OpenGL

**Key Attributes**:

- OpenGL context management
- Shader compilation and management
- Vertex buffer object (VBO) handling
- Matrix transformations for 3D projection
- Frame timing for performance monitoring

**Relationships**:

- Used by Test Game Application for drawing
- Traverses Scene Graph to process renderable nodes
- Manages 3D Geometry data on GPU
- Interfaces with platform windowing system via SDL2

**State Transitions**:

- Initialize OpenGL → Compile Shaders → Ready for Rendering
- Frame cycle: Begin → Traverse Scene Graph → Render Nodes → Present → Next Frame

**Validation Rules**:

- Must achieve 60fps performance target consistently
- OpenGL context must initialize without errors
- Must maintain correct 3D perspective for cube display
- Scene graph traversal must be efficient and accurate

## Data Flow Relationships

```
Test Game Application
    ↓ creates and manages
Scene Graph  
    ↓ contains hierarchical
Scene Node (Cube)
    ↓ has components  
Transform Component + Render Component
    ↓ references
3D Geometry (Cube mesh)
    ↓ traversed and rendered by
Renderer
    ↓ implements
Engine Library scene graph interfaces
```

## Validation Dependencies

- **Engine Library** must be built before **Test Game Application** can link
- **3D Geometry** must be loaded before **Render Component** can reference it
- **Transform hierarchy** calculations must complete before **Renderer** can apply world matrices
- **OpenGL context** must be initialized before any **3D Geometry** can be uploaded to GPU
- **Scene Graph** must be properly constructed before rendering traversal begins

## Performance Characteristics

- **Scene graph traversal**: O(n) where n = number of nodes in hierarchy (initially just 1 cube)
- **Transform propagation**: O(d) where d = depth of hierarchy for affected subtrees  
- **Component processing**: O(c) where c = number of components per node
- **Geometry rendering**: O(1) for static cube mesh
- **Frame timing**: Target 16.67ms per frame for 60fps consistency

This data model supports the learning objectives by demonstrating scene graph hierarchies, automatic transform inheritance, component attachment at node level, and performance-conscious design patterns while maintaining educational simplicity appropriate for initial engine development.
