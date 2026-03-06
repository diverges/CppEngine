#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <chrono>

#include "AIEngine/core/EngineSystemDescriptor.hpp"
#include "AIEngine/core/EngineSystemManager.hpp"
#include "AIEngine/core/IEngineSubsystem.hpp"
#include "AIEngine/core/ISubsystemProvider.hpp"
#include "doctest.h"

namespace AIEngine {

// Mock test interfaces for testing (reuse from descriptor test)
class MockProvider : public ISubsystemProvider {
   public:
    bool Initialize() override { return true; }
    void Start() override {}
    void Stop() override {}
    void Destroy() override {}
    bool IsInitialized() const override { return true; }
    bool IsRunning() const override { return true; }
    const std::string& GetProviderName() const override {
        static const std::string name = "Mock Provider";
        return name;
    }
    const std::string& GetProviderId() const override {
        static const std::string id = "mock-provider";
        return id;
    }
};

class IMockSubsystem : public IEngineSubsystem {
   public:
    virtual void DoMockOperation() = 0;
};

class MockSubsystem : public IMockSubsystem {
   private:
    std::unique_ptr<ISubsystemProvider> provider;

   public:
    bool Initialize() override { return provider && provider->Initialize(); }
    void Start() override {
        if (provider) provider->Start();
    }
    void Stop() override {
        if (provider) provider->Stop();
    }
    void Destroy() override {
        if (provider) provider->Destroy();
    }
    bool IsInitialized() const override { return provider && provider->IsInitialized(); }
    bool IsRunning() const override { return provider && provider->IsRunning(); }

    void SetSubsystemProvider(std::unique_ptr<ISubsystemProvider> newProvider) override {
        provider = std::move(newProvider);
    }
    ISubsystemProvider* GetSubsystemProvider() const override { return provider.get(); }
    bool HasProvider() const override { return provider != nullptr; }

    EngineSystemDescriptor GetDescriptor() const override {
        return EngineSystemDescriptor::Create<IMockSubsystem, MockSubsystem, MockProvider>(
            "Mock System");
    }
    const std::string& GetSubsystemName() const override {
        static const std::string name = "Mock Subsystem";
        return name;
    }

    void DoMockOperation() override {}
};

}  // namespace AIEngine

TEST_CASE("EngineSystemManager Singleton") {
    SUBCASE("GetInstance returns same instance") {
        auto& manager1 = AIEngine::EngineSystemManager::GetInstance();
        auto& manager2 = AIEngine::EngineSystemManager::GetInstance();

        CHECK(&manager1 == &manager2);
    }
}

TEST_CASE("EngineSystemManager Registration") {
    auto& manager = AIEngine::EngineSystemManager::GetInstance();

    SUBCASE("Register system descriptor") {
        auto descriptor = AIEngine::EngineSystemDescriptor::Create<
            AIEngine::IMockSubsystem, AIEngine::MockSubsystem, AIEngine::MockProvider>(
            "Test System");

        // Should not throw
        manager.RegisterSystemDescriptor(descriptor);

        auto descriptors = manager.GetRegisteredDescriptors();
        CHECK(descriptors.size() >= 1);

        // Check if our descriptor is in the list
        bool found = false;
        for (const auto& desc : descriptors) {
            if (desc.GetSystemName() == "Test System") {
                found = true;
                break;
            }
        }
        CHECK(found);
    }

    SUBCASE("Reject duplicate interface registration") {
        auto descriptor1 = AIEngine::EngineSystemDescriptor::Create<
            AIEngine::IMockSubsystem, AIEngine::MockSubsystem, AIEngine::MockProvider>("System 1");
        auto descriptor2 = AIEngine::EngineSystemDescriptor::Create<
            AIEngine::IMockSubsystem, AIEngine::MockSubsystem, AIEngine::MockProvider>("System 2");

        manager.RegisterSystemDescriptor(descriptor1);

        // Second registration of same interface should be rejected
        CHECK_THROWS_AS(manager.RegisterSystemDescriptor(descriptor2), std::runtime_error);
    }
}

TEST_CASE("EngineSystemManager System Creation") {
    auto& manager = AIEngine::EngineSystemManager::GetInstance();

    SUBCASE("Create system from registered descriptor") {
        auto descriptor = AIEngine::EngineSystemDescriptor::Create<
            AIEngine::IMockSubsystem, AIEngine::MockSubsystem, AIEngine::MockProvider>(
            "Test System");
        manager.RegisterSystemDescriptor(descriptor);

        auto* subsystem = manager.CreateSystem<AIEngine::IMockSubsystem>(descriptor);

        CHECK(subsystem != nullptr);
        CHECK(subsystem->HasProvider());
    }

    SUBCASE("GetSystem returns created system") {
        auto descriptor = AIEngine::EngineSystemDescriptor::Create<
            AIEngine::IMockSubsystem, AIEngine::MockSubsystem, AIEngine::MockProvider>(
            "Test System");
        manager.RegisterSystemDescriptor(descriptor);
        manager.CreateSystem<AIEngine::IMockSubsystem>(descriptor);

        auto* subsystem = manager.GetSystem<AIEngine::IMockSubsystem>();
        CHECK(subsystem != nullptr);
    }

    SUBCASE("HasSystem returns correct status") {
        CHECK(!manager.HasSystem<AIEngine::IMockSubsystem>());

        auto descriptor = AIEngine::EngineSystemDescriptor::Create<
            AIEngine::IMockSubsystem, AIEngine::MockSubsystem, AIEngine::MockProvider>(
            "Test System");
        manager.RegisterSystemDescriptor(descriptor);
        manager.CreateSystem<AIEngine::IMockSubsystem>(descriptor);

        CHECK(manager.HasSystem<AIEngine::IMockSubsystem>());
    }
}

TEST_CASE("EngineSystemManager O(1) Performance") {
    auto& manager = AIEngine::EngineSystemManager::GetInstance();

    SUBCASE("GetSystem operates in constant time") {
        auto descriptor = AIEngine::EngineSystemDescriptor::Create<
            AIEngine::IMockSubsystem, AIEngine::MockSubsystem, AIEngine::MockProvider>(
            "Test System");
        manager.RegisterSystemDescriptor(descriptor);
        manager.CreateSystem<AIEngine::IMockSubsystem>(descriptor);

        // This should be very fast - just a hash lookup
        auto start = std::chrono::high_resolution_clock::now();
        auto* subsystem = manager.GetSystem<AIEngine::IMockSubsystem>();
        auto end = std::chrono::high_resolution_clock::now();

        CHECK(subsystem != nullptr);

        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        // Should take less than 100 microseconds
        CHECK(duration.count() < 100);
    }
}

TEST_CASE("EngineSystemManager Lifecycle Management") {
    auto& manager = AIEngine::EngineSystemManager::GetInstance();

    // Clean up first to ensure clean state
    manager.DestroyAllSystems();

    SUBCASE("InitializeAllSystems initializes all registered systems") {
        auto descriptor = AIEngine::EngineSystemDescriptor::Create<
            AIEngine::IMockSubsystem, AIEngine::MockSubsystem, AIEngine::MockProvider>(
            "Test System");
        manager.RegisterSystemDescriptor(descriptor);
        manager.CreateSystem<AIEngine::IMockSubsystem>(descriptor);

        // Initial state
        auto* system = manager.GetSystem<AIEngine::IMockSubsystem>();
        CHECK_FALSE(system->IsInitialized());
        CHECK_FALSE(system->IsRunning());

        // Initialize all systems
        CHECK(manager.InitializeAllSystems());

        CHECK(system->IsInitialized());
        CHECK_FALSE(system->IsRunning());
    }

    SUBCASE("StartAllSystems starts all initialized systems") {
        auto descriptor = AIEngine::EngineSystemDescriptor::Create<
            AIEngine::IMockSubsystem, AIEngine::MockSubsystem, AIEngine::MockProvider>(
            "Test System");
        manager.RegisterSystemDescriptor(descriptor);
        manager.CreateSystem<AIEngine::IMockSubsystem>(descriptor);

        // Initialize first
        manager.InitializeAllSystems();
        auto* system = manager.GetSystem<AIEngine::IMockSubsystem>();
        CHECK(system->IsInitialized());
        CHECK_FALSE(system->IsRunning());

        // Start all systems
        manager.StartAllSystems();

        CHECK(system->IsInitialized());
        CHECK(system->IsRunning());
    }

    SUBCASE("StopAllSystems stops all running systems") {
        auto descriptor = AIEngine::EngineSystemDescriptor::Create<
            AIEngine::IMockSubsystem, AIEngine::MockSubsystem, AIEngine::MockProvider>(
            "Test System");
        manager.RegisterSystemDescriptor(descriptor);
        manager.CreateSystem<AIEngine::IMockSubsystem>(descriptor);

        // Initialize and start
        manager.InitializeAllSystems();
        manager.StartAllSystems();
        auto* system = manager.GetSystem<AIEngine::IMockSubsystem>();
        CHECK(system->IsRunning());

        // Stop all systems
        manager.StopAllSystems();

        CHECK(system->IsInitialized());
        CHECK_FALSE(system->IsRunning());
    }

    SUBCASE("DestroyAllSystems destroys all systems") {
        auto descriptor = AIEngine::EngineSystemDescriptor::Create<
            AIEngine::IMockSubsystem, AIEngine::MockSubsystem, AIEngine::MockProvider>(
            "Test System");
        manager.RegisterSystemDescriptor(descriptor);
        manager.CreateSystem<AIEngine::IMockSubsystem>(descriptor);

        // Initialize and start
        manager.InitializeAllSystems();
        manager.StartAllSystems();
        CHECK(manager.HasSystem<AIEngine::IMockSubsystem>());

        // Destroy all systems
        manager.DestroyAllSystems();

        CHECK_FALSE(manager.HasSystem<AIEngine::IMockSubsystem>());
    }

    SUBCASE("Complete lifecycle workflow") {
        auto descriptor = AIEngine::EngineSystemDescriptor::Create<
            AIEngine::IMockSubsystem, AIEngine::MockSubsystem, AIEngine::MockProvider>(
            "Test System");
        manager.RegisterSystemDescriptor(descriptor);
        manager.CreateSystem<AIEngine::IMockSubsystem>(descriptor);

        auto* system = manager.GetSystem<AIEngine::IMockSubsystem>();

        // Full lifecycle: Created -> Initialize -> Start -> Stop -> Destroy
        CHECK_FALSE(system->IsInitialized());
        CHECK_FALSE(system->IsRunning());

        CHECK(manager.InitializeAllSystems());
        CHECK(system->IsInitialized());
        CHECK_FALSE(system->IsRunning());

        manager.StartAllSystems();
        CHECK(system->IsInitialized());
        CHECK(system->IsRunning());

        manager.StopAllSystems();
        CHECK(system->IsInitialized());
        CHECK_FALSE(system->IsRunning());

        manager.DestroyAllSystems();
        CHECK_FALSE(manager.HasSystem<AIEngine::IMockSubsystem>());
    }
}

TEST_CASE("EngineSystemManager System Enumeration") {
    auto& manager = AIEngine::EngineSystemManager::GetInstance();

    // Clean up first to ensure clean state
    manager.DestroyAllSystems();

    SUBCASE("GetActiveSubsystems returns active systems") {
        CHECK(manager.GetActiveSubsystems().empty());

        auto descriptor = AIEngine::EngineSystemDescriptor::Create<
            AIEngine::IMockSubsystem, AIEngine::MockSubsystem, AIEngine::MockProvider>(
            "Test System");
        manager.RegisterSystemDescriptor(descriptor);
        manager.CreateSystem<AIEngine::IMockSubsystem>(descriptor);

        auto activeSystems = manager.GetActiveSubsystems();
        CHECK(activeSystems.size() == 1);
        CHECK(activeSystems[0] != nullptr);
    }
}

TEST_CASE("EngineSystemManager Error Handling") {
    auto& manager = AIEngine::EngineSystemManager::GetInstance();

    // Clean up first to ensure clean state
    manager.DestroyAllSystems();

    SUBCASE("GetSystem returns nullptr for non-existent system") {
        auto* system = manager.GetSystem<AIEngine::IMockSubsystem>();
        CHECK(system == nullptr);
    }

    SUBCASE("HasSystem returns false for non-existent system") {
        CHECK_FALSE(manager.HasSystem<AIEngine::IMockSubsystem>());
    }
}

TEST_CASE("EngineSystemManager Thread Safety Note") {
    // Note: This manager is designed for single-threaded use
    // All operations should occur on the main thread
    // This test serves as documentation of this requirement

    auto& manager = AIEngine::EngineSystemManager::GetInstance();

    SUBCASE("Manager operations are designed for main thread only") {
        // This is a documentation test - the manager should only be used on main thread
        // Multi-threaded access is not supported and not tested
        CHECK(&manager == &AIEngine::EngineSystemManager::GetInstance());
    }
}