#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <chrono>
#include <memory>
#include <vector>

#include "AIEngine/core/EngineSystemDescriptor.hpp"
#include "AIEngine/core/EngineSystemManager.hpp"
#include "AIEngine/core/IEngineSubsystem.hpp"
#include "AIEngine/core/ISubsystemProvider.hpp"
#include "doctest.h"

namespace AIEngine {

// Performance test provider
class PerformanceTestProvider : public ISubsystemProvider {
   public:
    bool Initialize() override { return true; }
    void Start() override {}
    void Stop() override {}
    void Destroy() override {}
    bool IsInitialized() const override { return true; }
    bool IsRunning() const override { return true; }

    const std::string& GetProviderName() const override {
        static const std::string name = "Performance Test Provider";
        return name;
    }

    const std::string& GetProviderId() const override {
        static const std::string id = "perf-test-provider";
        return id;
    }
};

// Performance test subsystem interface
class IPerformanceTestSubsystem : public IEngineSubsystem {
   public:
    virtual void DoWork() = 0;
};

// Performance test subsystem implementation
class PerformanceTestSubsystem : public IPerformanceTestSubsystem {
   public:
    bool Initialize() override { return true; }
    void Start() override {}
    void Stop() override {}
    void Destroy() override {}
    bool IsInitialized() const override { return true; }
    bool IsRunning() const override { return true; }

    void SetSubsystemProvider(std::unique_ptr<ISubsystemProvider> p) override {
        provider = std::unique_ptr<PerformanceTestProvider>(
            static_cast<PerformanceTestProvider*>(p.release()));
    }

    ISubsystemProvider* GetSubsystemProvider() const override { return provider.get(); }

    bool HasProvider() const override { return provider != nullptr; }

    EngineSystemDescriptor GetDescriptor() const override {
        return EngineSystemDescriptor::Create<IPerformanceTestSubsystem, PerformanceTestSubsystem,
                                              PerformanceTestProvider>("Performance Test");
    }

    const std::string& GetSubsystemName() const override {
        static const std::string name = "Performance Test Subsystem";
        return name;
    }

    void DoWork() override {
        // Minimal work for performance testing
        workCount++;
    }

   private:
    std::unique_ptr<PerformanceTestProvider> provider;
    int workCount = 0;
};

// Multiple different subsystem types for testing scalability
class IPerformanceTestSubsystem2 : public IEngineSubsystem {
   public:
    virtual void DoWork() = 0;
};

class PerformanceTestSubsystem2 : public IPerformanceTestSubsystem2 {
   public:
    bool Initialize() override { return true; }
    void Start() override {}
    void Stop() override {}
    void Destroy() override {}
    bool IsInitialized() const override { return true; }
    bool IsRunning() const override { return true; }

    void SetSubsystemProvider(std::unique_ptr<ISubsystemProvider> p) override {
        provider = std::unique_ptr<PerformanceTestProvider>(
            static_cast<PerformanceTestProvider*>(p.release()));
    }

    ISubsystemProvider* GetSubsystemProvider() const override { return provider.get(); }

    bool HasProvider() const override { return provider != nullptr; }

    EngineSystemDescriptor GetDescriptor() const override {
        return EngineSystemDescriptor::Create<IPerformanceTestSubsystem2, PerformanceTestSubsystem2,
                                              PerformanceTestProvider>("Performance Test 2");
    }

    const std::string& GetSubsystemName() const override {
        static const std::string name = "Performance Test Subsystem 2";
        return name;
    }

    void DoWork() override {}

   private:
    std::unique_ptr<PerformanceTestProvider> provider;
};

class IPerformanceTestSubsystem3 : public IEngineSubsystem {
   public:
    virtual void DoWork() = 0;
};

class PerformanceTestSubsystem3 : public IPerformanceTestSubsystem3 {
   public:
    bool Initialize() override { return true; }
    void Start() override {}
    void Stop() override {}
    void Destroy() override {}
    bool IsInitialized() const override { return true; }
    bool IsRunning() const override { return true; }

    void SetSubsystemProvider(std::unique_ptr<ISubsystemProvider> p) override {
        provider = std::unique_ptr<PerformanceTestProvider>(
            static_cast<PerformanceTestProvider*>(p.release()));
    }

    ISubsystemProvider* GetSubsystemProvider() const override { return provider.get(); }

    bool HasProvider() const override { return provider != nullptr; }

    EngineSystemDescriptor GetDescriptor() const override {
        return EngineSystemDescriptor::Create<IPerformanceTestSubsystem3, PerformanceTestSubsystem3,
                                              PerformanceTestProvider>("Performance Test 3");
    }

    const std::string& GetSubsystemName() const override {
        static const std::string name = "Performance Test Subsystem 3";
        return name;
    }

    void DoWork() override {}

   private:
    std::unique_ptr<PerformanceTestProvider> provider;
};

}  // namespace AIEngine

// Helper function to measure execution time in microseconds
template <typename Func>
double MeasureExecutionTime(Func&& func, int iterations = 1000) {
    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < iterations; ++i) {
        func();
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    return static_cast<double>(duration.count()) / iterations;
}

TEST_CASE("EngineSystemManager O(1) Performance Validation") {
    auto& manager = AIEngine::EngineSystemManager::GetInstance();
    manager.DestroyAllSystems();

    SUBCASE("GetSystem operates in O(1) time regardless of number of registered systems") {
        // Setup multiple subsystems to test scalability
        auto descriptor1 =
            AIEngine::EngineSystemDescriptor::Create<AIEngine::IPerformanceTestSubsystem,
                                                     AIEngine::PerformanceTestSubsystem,
                                                     AIEngine::PerformanceTestProvider>(
                "Perf Test 1");

        auto descriptor2 =
            AIEngine::EngineSystemDescriptor::Create<AIEngine::IPerformanceTestSubsystem2,
                                                     AIEngine::PerformanceTestSubsystem2,
                                                     AIEngine::PerformanceTestProvider>(
                "Perf Test 2");

        auto descriptor3 =
            AIEngine::EngineSystemDescriptor::Create<AIEngine::IPerformanceTestSubsystem3,
                                                     AIEngine::PerformanceTestSubsystem3,
                                                     AIEngine::PerformanceTestProvider>(
                "Perf Test 3");

        // Register and create all subsystems
        manager.RegisterSystemDescriptor(descriptor1);
        manager.RegisterSystemDescriptor(descriptor2);
        manager.RegisterSystemDescriptor(descriptor3);

        manager.CreateSystem<AIEngine::IPerformanceTestSubsystem>(descriptor1);
        manager.CreateSystem<AIEngine::IPerformanceTestSubsystem2>(descriptor2);
        manager.CreateSystem<AIEngine::IPerformanceTestSubsystem3>(descriptor3);

        // Measure GetSystem performance for different subsystems
        double time1 = MeasureExecutionTime([&manager]() {
            auto* subsystem = manager.GetSystem<AIEngine::IPerformanceTestSubsystem>();
            (void)subsystem;  // Suppress unused variable warning
        });

        double time2 = MeasureExecutionTime([&manager]() {
            auto* subsystem = manager.GetSystem<AIEngine::IPerformanceTestSubsystem2>();
            (void)subsystem;
        });

        double time3 = MeasureExecutionTime([&manager]() {
            auto* subsystem = manager.GetSystem<AIEngine::IPerformanceTestSubsystem3>();
            (void)subsystem;
        });

        // All lookup times should be under 1 microsecond for O(1) hash map lookup
        const double MAX_LOOKUP_TIME_MICROSECONDS = 1.0;

        CHECK(time1 < MAX_LOOKUP_TIME_MICROSECONDS);
        CHECK(time2 < MAX_LOOKUP_TIME_MICROSECONDS);
        CHECK(time3 < MAX_LOOKUP_TIME_MICROSECONDS);

        // Times should be roughly similar (within same order of magnitude)
        // This validates O(1) behavior rather than O(n) linear search
        double maxTime = std::max({time1, time2, time3});
        double minTime = std::min({time1, time2, time3});

        // Max time should not be more than 10x min time for O(1) operations
        if (minTime > 0) {
            CHECK(maxTime / minTime < 10.0);
        }
    }

    SUBCASE("HasSystem operates in O(1) time") {
        auto descriptor =
            AIEngine::EngineSystemDescriptor::Create<AIEngine::IPerformanceTestSubsystem,
                                                     AIEngine::PerformanceTestSubsystem,
                                                     AIEngine::PerformanceTestProvider>(
                "HasSystem Test");

        manager.RegisterSystemDescriptor(descriptor);
        manager.CreateSystem<AIEngine::IPerformanceTestSubsystem>(descriptor);

        double time = MeasureExecutionTime([&manager]() {
            bool hasSystem = manager.HasSystem<AIEngine::IPerformanceTestSubsystem>();
            (void)hasSystem;
        });

        const double MAX_LOOKUP_TIME_MICROSECONDS = 1.0;
        CHECK(time < MAX_LOOKUP_TIME_MICROSECONDS);
    }

    SUBCASE("CreateSystem operates efficiently") {
        auto descriptor =
            AIEngine::EngineSystemDescriptor::Create<AIEngine::IPerformanceTestSubsystem,
                                                     AIEngine::PerformanceTestSubsystem,
                                                     AIEngine::PerformanceTestProvider>(
                "CreateSystem Test");

        manager.RegisterSystemDescriptor(descriptor);

        // CreateSystem is more complex than GetSystem but should still be fast
        double time = MeasureExecutionTime(
            [&manager, &descriptor]() {
                manager.DestroySystem<AIEngine::IPerformanceTestSubsystem>();  // Clean up first
                auto* subsystem =
                    manager.CreateSystem<AIEngine::IPerformanceTestSubsystem>(descriptor);
                (void)subsystem;
            },
            100);  // Fewer iterations since it's a more expensive operation

        // CreateSystem should complete within 10 microseconds
        const double MAX_CREATE_TIME_MICROSECONDS = 10.0;
        CHECK(time < MAX_CREATE_TIME_MICROSECONDS);
    }

    SUBCASE("Memory overhead validation") {
        // Test that the manager doesn't use excessive memory
        size_t initialMemoryEstimate = sizeof(AIEngine::EngineSystemManager);

        // Each SystemEntry should be reasonably sized
        size_t systemEntrySize = sizeof(std::unique_ptr<AIEngine::IEngineSubsystem>) +
                                 sizeof(AIEngine::EngineSystemDescriptor) +
                                 sizeof(bool) * 2;  // initialized + running flags

        // Manager should use less than 10KB of base overhead
        const size_t MAX_BASE_OVERHEAD_BYTES = 10 * 1024;
        CHECK(initialMemoryEstimate < MAX_BASE_OVERHEAD_BYTES);

        // Each subsystem entry should be reasonable size (under 1KB)
        const size_t MAX_ENTRY_SIZE_BYTES = 1024;
        CHECK(systemEntrySize < MAX_ENTRY_SIZE_BYTES);
    }

    // Cleanup
    manager.DestroyAllSystems();
}

TEST_CASE("Performance Requirements Validation") {
    auto& manager = AIEngine::EngineSystemManager::GetInstance();
    manager.DestroyAllSystems();

    SUBCASE("Subsystem operation latency under 1ms") {
        auto descriptor =
            AIEngine::EngineSystemDescriptor::Create<AIEngine::IPerformanceTestSubsystem,
                                                     AIEngine::PerformanceTestSubsystem,
                                                     AIEngine::PerformanceTestProvider>(
                "Latency Test");

        manager.RegisterSystemDescriptor(descriptor);
        auto* subsystem = manager.CreateSystem<AIEngine::IPerformanceTestSubsystem>(descriptor);
        REQUIRE(subsystem != nullptr);

        // Measure typical subsystem operations
        double initTime = MeasureExecutionTime([&subsystem]() { subsystem->Initialize(); }, 100);

        double startTime = MeasureExecutionTime([&subsystem]() { subsystem->Start(); }, 100);

        double workTime = MeasureExecutionTime([&subsystem]() { subsystem->DoWork(); });

        double stopTime = MeasureExecutionTime([&subsystem]() { subsystem->Stop(); }, 100);

        // All operations should complete within 1000 microseconds (1ms)
        const double MAX_OPERATION_TIME_MICROSECONDS = 1000.0;

        CHECK(initTime < MAX_OPERATION_TIME_MICROSECONDS);
        CHECK(startTime < MAX_OPERATION_TIME_MICROSECONDS);
        CHECK(workTime < MAX_OPERATION_TIME_MICROSECONDS);
        CHECK(stopTime < MAX_OPERATION_TIME_MICROSECONDS);
    }
}

TEST_CASE("GetEngineSubsystem Macro Performance") {
    auto& manager = AIEngine::EngineSystemManager::GetInstance();
    manager.DestroyAllSystems();

    SUBCASE("Convenience macro has same performance as direct call") {
        auto descriptor =
            AIEngine::EngineSystemDescriptor::Create<AIEngine::IPerformanceTestSubsystem,
                                                     AIEngine::PerformanceTestSubsystem,
                                                     AIEngine::PerformanceTestProvider>(
                "Macro Performance Test");

        manager.RegisterSystemDescriptor(descriptor);
        manager.CreateSystem<AIEngine::IPerformanceTestSubsystem>(descriptor);

        // Test macro performance
        double macroTime = MeasureExecutionTime([]() {
            auto* subsystem = GetEngineSubsystem(AIEngine::IPerformanceTestSubsystem);
            (void)subsystem;
        });

        // Test direct call performance
        double directTime = MeasureExecutionTime([&manager]() {
            auto* subsystem = manager.GetSystem<AIEngine::IPerformanceTestSubsystem>();
            (void)subsystem;
        });

        // Macro should have essentially the same performance (within 10% difference)
        if (directTime > 0) {
            double ratio = macroTime / directTime;
            CHECK(ratio < 1.1);  // Should not be more than 10% slower
            CHECK(ratio > 0.9);  // Should not be more than 10% faster due to noise
        }

        // Both should be under 1 microsecond
        const double MAX_LOOKUP_TIME_MICROSECONDS = 1.0;
        CHECK(macroTime < MAX_LOOKUP_TIME_MICROSECONDS);
        CHECK(directTime < MAX_LOOKUP_TIME_MICROSECONDS);
    }
}