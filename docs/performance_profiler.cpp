/**
 * @file performance_profiler.cpp
 * @brief Performance profiling utilities for AIEngine
 * @version 1.0.0
 * @date 2026-03-04
 * 
 * This file provides comprehensive performance profiling tools for:
 * - Frame timing and FPS analysis
 * - Memory usage monitoring  
 * - Component system performance
 * - Rendering pipeline bottlenecks
 * - Scene graph traversal optimization
 * 
 * Compile with: g++ -std=c++17 -O2 performance_profiler.cpp -L../engine/lib -lAIEngine
 */

#include <chrono>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <memory>
#include <algorithm>
#include <iomanip>

/**
 * High-resolution timer for performance measurements
 */
class PerformanceTimer {
private:
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = std::chrono::time_point<Clock>;
    using Duration = std::chrono::duration<double, std::milli>;
    
    TimePoint m_startTime;
    std::string m_name;
    bool m_running = false;
    
public:
    explicit PerformanceTimer(const std::string& name) : m_name(name) {}
    
    void Start() {
        m_startTime = Clock::now();
        m_running = true;
    }
    
    double Stop() {
        if (!m_running) return 0.0;
        
        auto endTime = Clock::now();
        Duration elapsed = endTime - m_startTime;
        m_running = false;
        
        return elapsed.count();
    }
    
    void PrintElapsed() {
        double elapsed = Stop();
        std::cout << m_name << ": " << std::fixed << std::setprecision(3) 
                  << elapsed << " ms" << std::endl;
    }
};

/**
 * RAII scoped timer for automatic timing
 */
class ScopedTimer {
private:
    PerformanceTimer m_timer;
    
public:
    explicit ScopedTimer(const std::string& name) : m_timer(name) {
        m_timer.Start();
    }
    
    ~ScopedTimer() {
        m_timer.PrintElapsed();
    }
};

/**
 * FPS counter and frame timing analyzer
 */
class FPSProfiler {
private:
    std::vector<double> m_frameTimes;
    size_t m_maxSamples;
    size_t m_currentIndex = 0;
    double m_totalTime = 0.0;
    
public:
    explicit FPSProfiler(size_t maxSamples = 1000) : m_maxSamples(maxSamples) {
        m_frameTimes.reserve(maxSamples);
    }
    
    void RecordFrame(double frameTimeMs) {
        if (m_frameTimes.size() < m_maxSamples) {
            m_frameTimes.push_back(frameTimeMs);
        } else {
            m_totalTime -= m_frameTimes[m_currentIndex];
            m_frameTimes[m_currentIndex] = frameTimeMs;
            m_currentIndex = (m_currentIndex + 1) % m_maxSamples;
        }
        m_totalTime += frameTimeMs;
    }
    
    double GetAverageFPS() const {
        if (m_frameTimes.empty()) return 0.0;
        double avgFrameTime = m_totalTime / m_frameTimes.size();
        return 1000.0 / avgFrameTime;  // Convert ms to FPS
    }
    
    double GetMinFPS() const {
        if (m_frameTimes.empty()) return 0.0;
        auto maxTime = *std::max_element(m_frameTimes.begin(), m_frameTimes.end());
        return 1000.0 / maxTime;
    }
    
    double GetMaxFPS() const {
        if (m_frameTimes.empty()) return 0.0;
        auto minTime = *std::min_element(m_frameTimes.begin(), m_frameTimes.end());
        return 1000.0 / minTime;
    }
    
    double GetFrameTimeP95() const {
        if (m_frameTimes.empty()) return 0.0;
        
        std::vector<double> sorted = m_frameTimes;
        std::sort(sorted.begin(), sorted.end());
        
        size_t p95Index = static_cast<size_t>(sorted.size() * 0.95);
        return sorted[std::min(p95Index, sorted.size() - 1)];
    }
    
    void PrintReport() const {
        std::cout << "\n=== FPS Performance Report ===" << std::endl;
        std::cout << "Samples: " << m_frameTimes.size() << std::endl;
        std::cout << "Average FPS: " << std::fixed << std::setprecision(1) 
                  << GetAverageFPS() << std::endl;
        std::cout << "Min FPS: " << GetMinFPS() << std::endl;  
        std::cout << "Max FPS: " << GetMaxFPS() << std::endl;
        std::cout << "95th percentile frame time: " << std::setprecision(3) 
                  << GetFrameTimeP95() << " ms" << std::endl;
        
        // Performance classification
        double avgFPS = GetAverageFPS();
        if (avgFPS >= 120) {
            std::cout << "Performance: ✅ EXCELLENT (120+ FPS)" << std::endl;
        } else if (avgFPS >= 60) {
            std::cout << "Performance: ✅ GOOD (60+ FPS)" << std::endl;
        } else if (avgFPS >= 30) {
            std::cout << "Performance: ⚠️  ACCEPTABLE (30+ FPS)" << std::endl;
        } else {
            std::cout << "Performance: ❌ POOR (<30 FPS)" << std::endl;
        }
        std::cout << "==============================\n" << std::endl;
    }
    
    void SaveToFile(const std::string& filename) const {
        std::ofstream file(filename);
        if (!file) return;
        
        file << "frame_index,frame_time_ms,fps\n";
        for (size_t i = 0; i < m_frameTimes.size(); ++i) {
            double frameTime = m_frameTimes[i];
            double fps = 1000.0 / frameTime;
            file << i << "," << frameTime << "," << fps << "\n";
        }
    }
};

/**
 * Memory usage profiler
 */
class MemoryProfiler {
private:
    struct MemorySnapshot {
        size_t totalAllocated = 0;
        size_t peakAllocated = 0;
        size_t currentAllocated = 0;
        size_t allocCount = 0;
        size_t deallocCount = 0;
    };
    
    MemorySnapshot m_snapshot;
    
public:
    void RecordAllocation(size_t bytes) {
        m_snapshot.totalAllocated += bytes;
        m_snapshot.currentAllocated += bytes;
        m_snapshot.allocCount++;
        
        if (m_snapshot.currentAllocated > m_snapshot.peakAllocated) {
            m_snapshot.peakAllocated = m_snapshot.currentAllocated;
        }
    }
    
    void RecordDeallocation(size_t bytes) {
        m_snapshot.currentAllocated = (m_snapshot.currentAllocated >= bytes) ? 
                                     m_snapshot.currentAllocated - bytes : 0;
        m_snapshot.deallocCount++;
    }
    
    void PrintReport() const {
        std::cout << "\n=== Memory Usage Report ===" << std::endl;
        std::cout << "Total Allocated: " << (m_snapshot.totalAllocated / 1024) << " KB" << std::endl;
        std::cout << "Peak Usage: " << (m_snapshot.peakAllocated / 1024) << " KB" << std::endl; 
        std::cout << "Current Usage: " << (m_snapshot.currentAllocated / 1024) << " KB" << std::endl;
        std::cout << "Allocations: " << m_snapshot.allocCount << std::endl;
        std::cout << "Deallocations: " << m_snapshot.deallocCount << std::endl;
        
        if (m_snapshot.currentAllocated > 0) {
            std::cout << "⚠️  Memory Leak Detected: " << m_snapshot.currentAllocated << " bytes" << std::endl;
        } else {
            std::cout << "✅ No Memory Leaks Detected" << std::endl;
        }
        std::cout << "===========================\n" << std::endl;
    }
};

/**
 * Component system performance profiler
 */
class ComponentProfiler {
private:
    struct ComponentStats {
        std::string name;
        size_t updateCount = 0;
        double totalUpdateTime = 0.0;
        double maxUpdateTime = 0.0;
        double minUpdateTime = std::numeric_limits<double>::max();
    };
    
    std::unordered_map<std::string, ComponentStats> m_componentStats;
    
public:
    void RecordComponentUpdate(const std::string& componentName, double updateTimeMs) {
        auto& stats = m_componentStats[componentName];
        stats.name = componentName;
        stats.updateCount++;
        stats.totalUpdateTime += updateTimeMs;
        stats.maxUpdateTime = std::max(stats.maxUpdateTime, updateTimeMs);
        stats.minUpdateTime = std::min(stats.minUpdateTime, updateTimeMs);
    }
    
    void PrintReport() const {
        std::cout << "\n=== Component Performance Report ===" << std::endl;
        std::cout << std::setw(20) << "Component" 
                  << std::setw(8) << "Updates"
                  << std::setw(12) << "Avg (ms)"
                  << std::setw(12) << "Max (ms)"
                  << std::setw(12) << "Min (ms)" << std::endl;
        std::cout << std::string(64, '-') << std::endl;
        
        for (const auto& [name, stats] : m_componentStats) {
            double avgTime = stats.totalUpdateTime / stats.updateCount;
            double minTime = (stats.minUpdateTime == std::numeric_limits<double>::max()) ? 0.0 : stats.minUpdateTime;
            
            std::cout << std::setw(20) << name
                      << std::setw(8) << stats.updateCount
                      << std::setw(12) << std::fixed << std::setprecision(4) << avgTime
                      << std::setw(12) << std::setprecision(4) << stats.maxUpdateTime
                      << std::setw(12) << std::setprecision(4) << minTime << std::endl;
        }
        std::cout << "=====================================\n" << std::endl;
    }
};

/**
 * Scene graph performance profiler
 */
class SceneGraphProfiler {
private:
    size_t m_nodeCount = 0;
    size_t m_traversalCount = 0;
    double m_totalTraversalTime = 0.0;
    double m_maxTraversalTime = 0.0;
    
public:
    void SetNodeCount(size_t count) { m_nodeCount = count; }
    
    void RecordTraversal(double traversalTimeMs) {
        m_traversalCount++;
        m_totalTraversalTime += traversalTimeMs;
        m_maxTraversalTime = std::max(m_maxTraversalTime, traversalTimeMs);
    }
    
    void PrintReport() const {
        std::cout << "\n=== Scene Graph Performance Report ===" << std::endl;
        std::cout << "Node Count: " << m_nodeCount << std::endl;
        std::cout << "Traversals: " << m_traversalCount << std::endl;
        
        if (m_traversalCount > 0) {
            double avgTime = m_totalTraversalTime / m_traversalCount;
            double nodesPerMs = (avgTime > 0) ? m_nodeCount / avgTime : 0;
            
            std::cout << "Avg Traversal Time: " << std::fixed << std::setprecision(4) 
                      << avgTime << " ms" << std::endl;
            std::cout << "Max Traversal Time: " << std::setprecision(4) 
                      << m_maxTraversalTime << " ms" << std::endl;
            std::cout << "Nodes per ms: " << std::setprecision(1) << nodesPerMs << std::endl;
            
            // Performance assessment
            if (avgTime < 0.1) {
                std::cout << "Traversal Performance: ✅ EXCELLENT" << std::endl;
            } else if (avgTime < 1.0) {
                std::cout << "Traversal Performance: ✅ GOOD" << std::endl;
            } else {
                std::cout << "Traversal Performance: ⚠️  NEEDS OPTIMIZATION" << std::endl;
            }
        }
        std::cout << "=======================================\n" << std::endl;
    }
};

/**
 * Comprehensive performance profiling suite
 */
class PerformanceProfilerSuite {
private:
    FPSProfiler m_fpsProfiler;
    MemoryProfiler m_memoryProfiler;
    ComponentProfiler m_componentProfiler;
    SceneGraphProfiler m_sceneGraphProfiler;
    
    bool m_isRunning = false;
    std::chrono::high_resolution_clock::time_point m_startTime;
    
public:
    PerformanceProfilerSuite() = default;
    
    void StartProfiling() {
        m_isRunning = true;
        m_startTime = std::chrono::high_resolution_clock::now();
        std::cout << "🔍 Performance profiling started..." << std::endl;
    }
    
    void StopProfiling() {
        m_isRunning = false;
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - m_startTime);
        
        std::cout << "🔍 Performance profiling stopped after " 
                  << duration.count() << "ms" << std::endl;
    }
    
    // Profiler accessors
    FPSProfiler& GetFPSProfiler() { return m_fpsProfiler; }
    MemoryProfiler& GetMemoryProfiler() { return m_memoryProfiler; }
    ComponentProfiler& GetComponentProfiler() { return m_componentProfiler; }
    SceneGraphProfiler& GetSceneGraphProfiler() { return m_sceneGraphProfiler; }
    
    void GenerateFullReport() {
        std::cout << "\n" << std::string(50, '=') << std::endl;
        std::cout << "         AIENGINE PERFORMANCE REPORT" << std::endl; 
        std::cout << std::string(50, '=') << std::endl;
        
        m_fpsProfiler.PrintReport();
        m_memoryProfiler.PrintReport();
        m_componentProfiler.PrintReport();
        m_sceneGraphProfiler.PrintReport();
        
        std::cout << std::string(50, '=') << std::endl;
        std::cout << "         END OF PERFORMANCE REPORT" << std::endl;
        std::cout << std::string(50, '=') << std::endl;
    }
    
    void SaveReports(const std::string& baseFilename) {
        m_fpsProfiler.SaveToFile(baseFilename + "_fps.csv");
        std::cout << "Reports saved to " << baseFilename << "_*.csv" << std::endl;
    }
};

/**
 * Macro for easy scoped timing
 */
#define PROFILE_SCOPE(name) ScopedTimer _timer(name)

/**
 * Example usage and benchmarking
 */
void DemoPerformanceProfiling() {
    std::cout << "=== AIEngine Performance Profiler Demo ===" << std::endl;
    
    PerformanceProfilerSuite profiler;
    profiler.StartProfiling();
    
    // Simulate frame timing
    auto& fps = profiler.GetFPSProfiler();
    auto& memory = profiler.GetMemoryProfiler();
    auto& components = profiler.GetComponentProfiler();
    auto& sceneGraph = profiler.GetSceneGraphProfiler();
    
    std::cout << "Simulating performance data..." << std::endl;
    
    // Simulate 60 frames of data
    sceneGraph.SetNodeCount(25);  // 25 nodes in scene
    
    for (int frame = 0; frame < 60; ++frame) {
        // Simulate variable frame times (16.67ms = 60fps)
        double frameTime = 16.67 + (rand() % 5 - 2);  // 14-19ms variation
        fps.RecordFrame(frameTime);
        
        // Simulate memory allocations/deallocations
        memory.RecordAllocation(1024 + rand() % 2048);
        if (frame % 5 == 0) {
            memory.RecordDeallocation(512);
        }
        
        // Simulate component updates
        components.RecordComponentUpdate("TransformComponent", 0.1 + (rand() % 10) / 100.0);
        components.RecordComponentUpdate("RenderComponent", 0.05 + (rand() % 5) / 100.0);
        components.RecordComponentUpdate("CustomComponent", 0.2 + (rand() % 15) / 100.0);
        
        // Simulate scene graph traversal
        double traversalTime = 0.5 + (rand() % 20) / 100.0;  // 0.5-0.7ms
        sceneGraph.RecordTraversal(traversalTime);
    }
    
    profiler.StopProfiling();
    profiler.GenerateFullReport();
    profiler.SaveReports("aiengine_perf");
    
    std::cout << "\n✅ Performance profiling demo complete!" << std::endl;
    std::cout << "Check aiengine_perf_fps.csv for detailed frame data." << std::endl;
}

/**
 * Performance optimization recommendations
 */
void PrintOptimizationGuide() {
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "           AIENGINE OPTIMIZATION GUIDE" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    
    std::cout << "\n🚀 RENDERING OPTIMIZATIONS:" << std::endl;
    std::cout << "  • Use object pooling for frequently created/destroyed objects" << std::endl;
    std::cout << "  • Batch similar render calls together" << std::endl;
    std::cout << "  • Implement frustum culling for off-screen objects" << std::endl;
    std::cout << "  • Use instanced rendering for multiple similar objects" << std::endl;
    std::cout << "  • Cache transformation matrices when possible" << std::endl;
    
    std::cout << "\n🧩 COMPONENT OPTIMIZATIONS:" << std::endl;
    std::cout << "  • Use component pools to reduce allocation overhead" << std::endl;
    std::cout << "  • Keep component data cache-friendly (SoA vs AoS)" << std::endl;
    std::cout << "  • Avoid virtual function calls in hot paths" << std::endl;
    std::cout << "  • Update only dirty/changed components each frame" << std::endl;
    
    std::cout << "\n🌲 SCENE GRAPH OPTIMIZATIONS:" << std::endl;
    std::cout << "  • Limit scene graph depth to avoid deep recursion" << std::endl;
    std::cout << "  • Use spatial partitioning for large scenes" << std::endl;
    std::cout << "  • Cache world transforms, recalculate only when dirty" << std::endl;
    std::cout << "  • Consider flat arrays for simple hierarchies" << std::endl;
    
    std::cout << "\n💾 MEMORY OPTIMIZATIONS:" << std::endl;
    std::cout << "  • Use memory pools for fixed-size allocations" << std::endl;
    std::cout << "  • Minimize dynamic allocations during runtime" << std::endl;
    std::cout << "  • Use move semantics to avoid unnecessary copies" << std::endl;
    std::cout << "  • Profile memory usage regularly to detect leaks" << std::endl;
    
    std::cout << "\n📊 PROFILING BEST PRACTICES:" << std::endl;
    std::cout << "  • Profile in release builds for accurate performance" << std::endl;
    std::cout << "  • Test on target hardware configurations" << std::endl;
    std::cout << "  • Profile both CPU and GPU performance" << std::endl;
    std::cout << "  • Use frame time instead of FPS for measurements" << std::endl;
    
    std::cout << "\n" << std::string(60, '=') << std::endl;
}

int main() {
    // Demo the performance profiling system
    DemoPerformanceProfiling();
    
    // Show optimization recommendations
    PrintOptimizationGuide();
    
    return 0;
}

/**
 * Integration example for AIEngine:
 * 
 * // In your game loop:
 * PerformanceProfilerSuite profiler;
 * profiler.StartProfiling();
 * 
 * while (!engine.ShouldClose()) {
 *     auto frameStart = std::chrono::high_resolution_clock::now();
 *     
 *     // Your game logic here
 *     engine.Update(deltaTime);
 *     engine.Render();
 *     
 *     auto frameEnd = std::chrono::high_resolution_clock::now();
 *     auto frameTime = std::chrono::duration<double, std::milli>(frameEnd - frameStart).count();
 *     
 *     profiler.GetFPSProfiler().RecordFrame(frameTime);
 * }
 * 
 * profiler.StopProfiling();
 * profiler.GenerateFullReport();
 */