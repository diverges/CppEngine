# Research Findings: Input-Controlled Camera Component

**Date**: March 4, 2026  
**Feature**: Input-Controlled Camera Component  
**Research Scope**: Scale, scope, and performance limits for educational game engine

## Scale & Scope Requirements

### Camera Count Limits

**Decision**: Support up to 4 simultaneous input-controlled cameras  
**Rationale**: Covers splitscreen gaming (2-4 players), debug camera scenarios, and picture-in-picture without overwhelming educational codebase  
**Alternatives considered**: Single camera (too limiting), unlimited cameras (performance concerns for educational hardware)

### Scene Complexity Targets

**Decision**: 10,000 triangles, 50 draw calls, 100 scene nodes maximum per frame  
**Rationale**: Conservative limits for 60fps on educational hardware (integrated graphics, older laptops), provides meaningful 3D scenes without overwhelming rendering  
**Alternatives considered**: Higher limits (hardware requirements too demanding), lower limits (scenes too simple for learning)

### Input Event Throughput  

**Decision**: Process up to 1000 input events per second (typical SDL2 polling at 60Hz with burst capacity)  
**Rationale**: SDL2 event queue handles typical user input with headroom for rapid mouse movement, burst key combinations  
**Alternatives considered**: Unlimited (can overwhelm component system), lower limits (restricts responsive input)

### Memory Constraints

**Decision**: <1MB total memory allocation for input subsystem and all camera components combined  
**Rationale**: Minimal impact on educational projects, ensures efficient data structures, prevents memory leaks in learning environment  
**Alternatives considered**: Larger allocations (unnecessary for scope), smaller allocations (too restrictive for multiple cameras)

## Technical Research Findings

### SDL2 Input Handling

- **Event polling**: SDL2 supports 60Hz+ polling with minimal CPU overhead
- **Input lag**: <5ms typical latency on modern systems via SDL_PollEvent
- **Event queue**: SDL2 internal queue handles 1000+ events without overflow
- **Platform consistency**: Input handling identical across Windows/macOS/Linux

### Component System Performance

- **Component access**: O(1) lookup via type-indexed containers typical in modern engines
- **Event dispatch**: Observer pattern scales linearly with subscriber count
- **Memory locality**: Component arrays enable cache-friendly iteration for multiple cameras

### OpenGL Rendering Pipeline

- **Matrix calculations**: GLM quaternion/matrix operations optimized for real-time use
- **State changes**: Minimal overhead for camera view matrix updates (single glUniform call)
- **Multiple viewports**: OpenGL supports 4+ simultaneous viewports with minimal performance impact

### Education-Specific Constraints  

- **Complexity**: Limits ensure understandable code for learning purposes
- **Hardware**: Conservative targets accommodate typical academic hardware
- **Debugging**: Multiple camera support enables enhanced learning through comparison and debugging scenarios

## Recommendations

1. **Implement camera pooling** to avoid dynamic allocation during runtime
2. **Use fixed-size event queues** for predictable memory usage  
3. **Batch input processing** at frame boundaries for consistent performance
4. **Profile on integrated graphics** to validate 60fps targets early in development
5. **Document performance characteristics** for educational value in understanding engine optimization
