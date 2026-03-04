# AIEngine Performance Optimization Guide

**Version**: 1.0.0  
**Date**: March 4, 2026  
**Target**: 60+ FPS with efficient memory usage  

## Overview

This guide provides comprehensive optimization strategies for maximizing AIEngine performance across rendering, component systems, memory management, and scene graph traversal.

## Performance Targets

### Baseline Requirements

| Metric | Minimum | Good | Excellent |
|--------|---------|------|-----------|
| **FPS** | 30 | 60 | 120+ |
| **Frame Time** | <33ms | <16.7ms | <8.3ms |
| **Memory Usage** | <50MB | <25MB | <15MB |
| **Scene Nodes** | 100+ | 500+ | 1000+ |

### Measurement Tools

```cpp
// Include performance profiler
#include "docs/performance_profiler.cpp"

// In your game loop
PerformanceProfilerSuite profiler;
profiler.StartProfiling();

// Your game code...

profiler.GenerateFullReport();
```

## 🚀 Rendering Pipeline Optimizations

### 1. OpenGL State Management

**Problem**: Excessive state changes cause GPU stalls  
**Solution**: Batch operations and cache state

```cpp
class OptimizedRenderer {
private:
    GLuint m_currentShader = 0;
    GLuint m_currentTexture = 0;
    
public:
    void UseShader(GLuint shader) {
        if (m_currentShader != shader) {
            glUseProgram(shader);
            m_currentShader = shader;
        }
    }
    
    void BindTexture(GLuint texture) {
        if (m_currentTexture != texture) {
            glBindTexture(GL_TEXTURE_2D, texture);
            m_currentTexture = texture;
        }
    }
};
```

### 2. Geometry Batching

**Problem**: Too many draw calls per frame  
**Solution**: Combine similar geometry into batches

```cpp
class BatchRenderer {
private:
    struct RenderBatch {
        GLuint VAO;
        size_t vertexCount;
        glm::mat4 transforms[MAX_INSTANCES];
        size_t instanceCount = 0;
    };
    
    std::vector<RenderBatch> m_batches;
    
public:
    void AddInstance(const Mesh& mesh, const glm::mat4& transform) {
        // Find or create batch for this mesh type
        auto& batch = FindBatch(mesh.GetType());
        
        if (batch.instanceCount < MAX_INSTANCES) {
            batch.transforms[batch.instanceCount++] = transform;
        } else {
            FlushBatch(batch);  // Render full batch
            batch.transforms[0] = transform;
            batch.instanceCount = 1;
        }
    }
    
    void FlushAll() {
        for (auto& batch : m_batches) {
            FlushBatch(batch);
        }
    }
};
```

### 3. Frustum Culling

**Problem**: Rendering objects outside camera view  
**Solution**: Cull invisible objects before rendering

```cpp
class FrustumCuller {
private:
    glm::mat4 m_viewProjection;
    
public:
    void UpdateFrustum(const glm::mat4& viewProj) {
        m_viewProjection = viewProj;
    }
    
    bool IsVisible(const BoundingBox& bbox) {
        // Quick sphere test first
        glm::vec3 center = bbox.GetCenter();
        float radius = bbox.GetRadius();
        
        // Transform center to clip space
        glm::vec4 clipPos = m_viewProjection * glm::vec4(center, 1.0f);
        
        // Check if sphere intersects view frustum
        return abs(clipPos.x) <= clipPos.w + radius &&
               abs(clipPos.y) <= clipPos.w + radius &&
               clipPos.z >= -clipPos.w - radius &&
               clipPos.z <= clipPos.w + radius;
    }
};
```

### 4. Level of Detail (LOD)

**Problem**: Complex geometry at all distances  
**Solution**: Use simpler models for distant objects

```cpp
class LODManager {
private:
    struct LODLevel {
        float distance;
        std::shared_ptr<Mesh> mesh;
    };
    
    std::vector<LODLevel> m_lodLevels;
    
public:
    std::shared_ptr<Mesh> GetLOD(float distanceToCamera) {
        for (const auto& lod : m_lodLevels) {
            if (distanceToCamera <= lod.distance) {
                return lod.mesh;
            }
        }
        return m_lodLevels.back().mesh;  // Fallback to lowest LOD
    }
};
```

## 🧩 Component System Optimizations

### 1. Component Pooling

**Problem**: Frequent component allocation/deallocation  
**Solution**: Pre-allocate component pools

```cpp
template<typename T>
class ComponentPool {
private:
    std::vector<T> m_pool;
    std::queue<size_t> m_available;
    size_t m_capacity;
    
public:
    explicit ComponentPool(size_t capacity) : m_capacity(capacity) {
        m_pool.resize(capacity);
        for (size_t i = 0; i < capacity; ++i) {
            m_available.push(i);
        }
    }
    
    T* Acquire() {
        if (m_available.empty()) return nullptr;
        
        size_t index = m_available.front();
        m_available.pop();
        return &m_pool[index];
    }
    
    void Release(T* component) {
        size_t index = component - m_pool.data();
        if (index < m_capacity) {
            m_available.push(index);
            component->~T();  // Explicit destructor call
            new(component) T();  // Reset to default state
        }
    }
};
```

### 2. Cache-Friendly Component Storage

**Problem**: Poor cache locality during component updates  
**Solution**: Structure of Arrays (SoA) layout

```cpp
// Instead of Array of Structures (AoS)
struct TransformComponent {
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;
};
std::vector<TransformComponent> transforms;  // AoS - poor cache usage

// Use Structure of Arrays (SoA)
class TransformSystem {
private:
    std::vector<glm::vec3> m_positions;   // Contiguous positions
    std::vector<glm::quat> m_rotations;   // Contiguous rotations  
    std::vector<glm::vec3> m_scales;      // Contiguous scales
    
public:
    void UpdatePositions(float deltaTime) {
        // Excellent cache locality - all positions in sequential memory
        for (size_t i = 0; i < m_positions.size(); ++i) {
            m_positions[i] += velocity[i] * deltaTime;
        }
    }
};
```

### 3. Dirty Flagging

**Problem**: Updating components that haven't changed  
**Solution**: Track dirty state and update only when needed

```cpp
class TransformComponent {
private:
    glm::vec3 m_position{0.0f};
    glm::quat m_rotation{1.0f, 0.0f, 0.0f, 0.0f};
    glm::vec3 m_scale{1.0f};
    
    mutable glm::mat4 m_worldMatrix{1.0f};
    mutable bool m_isDirty = true;
    
public:
    void SetPosition(const glm::vec3& pos) {
        if (m_position != pos) {
            m_position = pos;
            m_isDirty = true;
        }
    }
    
    const glm::mat4& GetWorldMatrix() const {
        if (m_isDirty) {
            m_worldMatrix = glm::translate(glm::mat4(1.0f), m_position) *
                           glm::mat4_cast(m_rotation) *
                           glm::scale(glm::mat4(1.0f), m_scale);
            m_isDirty = false;
        }
        return m_worldMatrix;
    }
};
```

## 🌲 Scene Graph Optimizations

### 1. Spatial Partitioning

**Problem**: Traversing entire scene graph for visibility checks  
**Solution**: Spatial subdivision (Quadtree/Octree)

```cpp
class Octree {
private:
    struct OctreeNode {
        BoundingBox bounds;
        std::vector<SceneNode*> objects;
        std::array<std::unique_ptr<OctreeNode>, 8> children;
        bool isLeaf = true;
        
        static constexpr size_t MAX_OBJECTS = 16;
        static constexpr int MAX_DEPTH = 8;
    };
    
    std::unique_ptr<OctreeNode> m_root;
    
public:
    void Insert(SceneNode* node) {
        InsertRecursive(m_root.get(), node, 0);
    }
    
    std::vector<SceneNode*> Query(const BoundingBox& region) {
        std::vector<SceneNode*> results;
        QueryRecursive(m_root.get(), region, results);
        return results;
    }
    
private:
    void InsertRecursive(OctreeNode* node, SceneNode* object, int depth) {
        if (node->isLeaf && node->objects.size() < OctreeNode::MAX_OBJECTS) {
            node->objects.push_back(object);
        } else {
            if (node->isLeaf && depth < OctreeNode::MAX_DEPTH) {
                Subdivide(node);
            }
            
            // Insert into appropriate child
            int childIndex = GetChildIndex(node->bounds, object->GetBounds());
            InsertRecursive(node->children[childIndex].get(), object, depth + 1);
        }
    }
};
```

### 2. Transform Hierarchy Optimization

**Problem**: Recalculating transforms down entire hierarchy chains  
**Solution**: Cache world transforms and propagate only when dirty

```cpp
class OptimizedSceneNode {
private:
    glm::mat4 m_localTransform{1.0f};
    mutable glm::mat4 m_worldTransform{1.0f};
    mutable bool m_worldDirty = true;
    mutable bool m_childrenDirty = false;
    
    std::vector<std::unique_ptr<OptimizedSceneNode>> m_children;
    OptimizedSceneNode* m_parent = nullptr;
    
public:
    void SetLocalTransform(const glm::mat4& transform) {
        m_localTransform = transform;
        MarkWorldDirty();
    }
    
    const glm::mat4& GetWorldTransform() const {
        if (m_worldDirty) {
            if (m_parent) {
                m_worldTransform = m_parent->GetWorldTransform() * m_localTransform;
            } else {
                m_worldTransform = m_localTransform;
            }
            m_worldDirty = false;
        }
        return m_worldTransform;
    }
    
private:
    void MarkWorldDirty() {
        m_worldDirty = true;
        m_childrenDirty = true;
        
        // Propagate dirty flag to children
        for (auto& child : m_children) {
            if (!child->m_worldDirty) {
                child->MarkWorldDirty();
            }
        }
    }
};
```

## 💾 Memory Management Optimizations

### 1. Object Pooling

**Problem**: Frequent allocation/deallocation causing fragmentation  
**Solution**: Pre-allocate objects in pools

```cpp
template<typename T>
class ObjectPool {
private:
    std::vector<std::unique_ptr<T>> m_objects;
    std::queue<T*> m_available;
    
public:
    explicit ObjectPool(size_t initialSize) {
        for (size_t i = 0; i < initialSize; ++i) {
            auto obj = std::make_unique<T>();
            m_available.push(obj.get());
            m_objects.push_back(std::move(obj));
        }
    }
    
    T* Acquire() {
        if (m_available.empty()) {
            // Expand pool if needed
            auto obj = std::make_unique<T>();
            T* ptr = obj.get();
            m_objects.push_back(std::move(obj));
            return ptr;
        }
        
        T* obj = m_available.front();
        m_available.pop();
        return obj;
    }
    
    void Release(T* obj) {
        obj->Reset();  // Assume T has a Reset method
        m_available.push(obj);
    }
};
```

### 2. Custom Memory Allocator

**Problem**: Default allocator not optimized for game patterns  
**Solution**: Stack allocator for temporary data

```cpp
class StackAllocator {
private:
    char* m_memory;
    size_t m_size;
    size_t m_offset = 0;
    
public:
    explicit StackAllocator(size_t size) : m_size(size) {
        m_memory = static_cast<char*>(std::aligned_alloc(64, size));
    }
    
    ~StackAllocator() {
        std::free(m_memory);
    }
    
    template<typename T>
    T* Allocate(size_t count = 1) {
        size_t bytes = sizeof(T) * count;
        size_t aligned = (bytes + alignof(T) - 1) & ~(alignof(T) - 1);
        
        if (m_offset + aligned > m_size) {
            return nullptr;  // Out of memory
        }
        
        T* result = reinterpret_cast<T*>(m_memory + m_offset);
        m_offset += aligned;
        return result;
    }
    
    void Reset() {
        m_offset = 0;  // Reset entire stack
    }
    
    size_t GetUsedBytes() const { return m_offset; }
    size_t GetAvailableBytes() const { return m_size - m_offset; }
};
```

## 📊 Profiling and Measurement

### 1. Frame Time Analysis

```cpp
class FrameProfiler {
private:
    std::chrono::high_resolution_clock::time_point m_frameStart;
    std::vector<double> m_frameTimes;
    
public:
    void BeginFrame() {
        m_frameStart = std::chrono::high_resolution_clock::now();
    }
    
    void EndFrame() {
        auto frameEnd = std::chrono::high_resolution_clock::now();
        auto frameDuration = std::chrono::duration<double, std::milli>(
            frameEnd - m_frameStart).count();
        
        m_frameTimes.push_back(frameDuration);
        
        // Keep only last 1000 frames
        if (m_frameTimes.size() > 1000) {
            m_frameTimes.erase(m_frameTimes.begin());
        }
    }
    
    void PrintStats() {
        if (m_frameTimes.empty()) return;
        
        double sum = std::accumulate(m_frameTimes.begin(), m_frameTimes.end(), 0.0);
        double avg = sum / m_frameTimes.size();
        double avgFPS = 1000.0 / avg;
        
        auto minMax = std::minmax_element(m_frameTimes.begin(), m_frameTimes.end());
        double minFPS = 1000.0 / *minMax.second;  // Max time = min FPS
        double maxFPS = 1000.0 / *minMax.first;   // Min time = max FPS
        
        std::cout << "Frame Stats - Avg: " << avgFPS << " FPS, "
                  << "Min: " << minFPS << " FPS, "  
                  << "Max: " << maxFPS << " FPS" << std::endl;
    }
};
```

### 2. GPU Performance Monitoring

```cpp
class GPUProfiler {
private:
    std::vector<GLuint> m_queries;
    std::vector<std::string> m_labels;
    size_t m_currentQuery = 0;
    
public:
    void BeginQuery(const std::string& label) {
        if (m_currentQuery >= m_queries.size()) {
            GLuint query;
            glGenQueries(1, &query);
            m_queries.push_back(query);
            m_labels.push_back(label);
        }
        
        glBeginQuery(GL_TIME_ELAPSED, m_queries[m_currentQuery]);
    }
    
    void EndQuery() {
        glEndQuery(GL_TIME_ELAPSED);
        m_currentQuery++;
    }
    
    void PrintResults() {
        for (size_t i = 0; i < std::min(m_queries.size(), m_currentQuery); ++i) {
            GLuint64 timeElapsed;
            glGetQueryObjectui64v(m_queries[i], GL_QUERY_RESULT, &timeElapsed);
            
            double timeMs = timeElapsed / 1000000.0;  // Convert to milliseconds
            std::cout << m_labels[i] << ": " << timeMs << " ms" << std::endl;
        }
        m_currentQuery = 0;  // Reset for next frame
    }
};
```

## 🎯 Performance Checklist

### Pre-Optimization

- [ ] Profile in **release builds** with optimizations enabled
- [ ] Test on **target hardware** (not just development machines)
- [ ] Identify actual **bottlenecks** before optimizing
- [ ] Set specific **performance targets** (60 FPS, memory limits)

### Rendering Optimization

- [ ] Implement **frustum culling** for invisible objects
- [ ] **Batch similar** render calls together  
- [ ] Cache **OpenGL state** to avoid redundant calls
- [ ] Use **object pooling** for frequently created geometry
- [ ] Implement **Level of Detail (LOD)** for distant objects

### Component System Optimization  

- [ ] Use **component pools** to reduce allocation overhead
- [ ] Implement **dirty flagging** for expensive calculations
- [ ] Consider **Structure of Arrays (SoA)** for better cache locality
- [ ] **Limit component updates** to only active/visible objects

### Memory Optimization

- [ ] Use **object pooling** for temporary allocations
- [ ] Implement **custom allocators** for specific use cases  
- [ ] **Pre-allocate** containers with expected size
- [ ] **Profile memory usage** regularly to detect leaks

### Scene Graph Optimization

- [ ] Implement **spatial partitioning** (Octree/Quadtree) for large scenes
- [ ] **Cache world transforms** and recalculate only when dirty
- [ ] **Limit scene depth** to avoid deep recursion
- [ ] Use **flat arrays** for simple hierarchies when possible

## 🔧 Tools and Workflows

### Build Configuration

```bash
# Profile build with optimization
make profile

# Memory leak detection  
make memcheck

# Performance benchmarking
make benchmark

# Static analysis
make analyze
```

### IDE Integration

- **Visual Studio**: Use built-in profiler for frame analysis
- **CLion**: Integrate Valgrind for memory profiling
- **VSCode**: Use gprof extension for call graph analysis

### External Tools

- **gprof**: Function-level profiling
- **Valgrind**: Memory leak detection (Linux)
- **Dr. Memory**: Memory debugging (Windows)
- **Intel VTune**: Advanced performance profiling
- **NVIDIA Nsight**: GPU performance analysis

## 📈 Performance Monitoring

### Continuous Integration

```yaml
# .github/workflows/performance.yml
name: Performance Tests
on: [push, pull_request]
jobs:
  benchmark:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3
      - name: Build and Benchmark
        run: |
          make benchmark
          # Compare results with baseline
          python scripts/compare_benchmarks.py
```

### Runtime Monitoring

```cpp
// Add to your game loop
class RuntimeProfiler {
private:
    float m_frameTime = 0.0f;
    float m_memoryUsage = 0.0f;
    
public:
    void Update(float deltaTime) {
        m_frameTime = deltaTime * 1000.0f;  // Convert to ms
        
        // Check for performance issues
        if (m_frameTime > 33.0f) {  // Below 30 FPS
            LOG_WARNING("Performance issue: frame time " << m_frameTime << "ms");
        }
    }
    
    void Draw() {
        // Overlay performance stats on screen
        ImGui::Begin("Performance");
        ImGui::Text("Frame Time: %.2f ms", m_frameTime);
        ImGui::Text("FPS: %.1f", 1000.0f / m_frameTime);
        ImGui::Text("Memory: %.1f MB", m_memoryUsage / 1024.0f / 1024.0f);
        ImGui::End();
    }
};
```

---

## Summary

Effective performance optimization requires:

1. **Measure First**: Profile before optimizing to identify real bottlenecks
2. **Optimize Systematically**: Start with highest-impact changes first  
3. **Test Continuously**: Verify improvements don't break functionality
4. **Monitor Regularly**: Set up automated performance tracking

For detailed performance profiling utilities, see `docs/performance_profiler.cpp`.
