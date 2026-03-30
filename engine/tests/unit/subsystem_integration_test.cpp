#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <chrono>
#include <memory>

#include "AIEngine/core/Engine.hpp"
#include "AIEngine/core/EngineSystemDescriptor.hpp"
#include "AIEngine/core/EngineSystemManager.hpp"
#include "AIEngine/core/IEngineSubsystem.hpp"
#include "AIEngine/core/ISubsystemProvider.hpp"
#include "doctest.h"

namespace AIEngine {

// Test provider for integration testing
class IntegrationTestProvider : public ISubsystemProvider {
   public:
    IntegrationTestProvider() = default;

    bool Initialize() override {
        initialized = true;
        return true;
    }

    void Start() override {
        if (initialized) running = true;
    }

    void Stop() override { running = false; }

    void Destroy() override {
        initialized = false;
        running = false;
    }

    bool IsInitialized() const override { return initialized; }
    bool IsRunning() const override { return running; }

    const std::string& GetProviderName() const override {
        static const std::string name = "Integration Test Provider";
        return name;
    }

    const std::string& GetProviderId() const override {
        static const std::string id = "integration-test-provider";
        return id;
    }

   private:
    bool initialized = false;
    bool running = false;
};

// Test subsystem interface
class IIntegrationTestSubsystem : public IEngineSubsystem {
   public:
    virtual void DoTestOperation() = 0;
    virtual int GetTestValue() const = 0;
};

// Test subsystem implementation
class IntegrationTestSubsystem : public IIntegrationTestSubsystem {
   public:
    IntegrationTestSubsystem() = default;

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

    void SetSubsystemProvider(std::unique_ptr<ISubsystemProvider> p) override {
        provider = std::unique_ptr<IntegrationTestProvider>(
            static_cast<IntegrationTestProvider*>(p.release()));
    }

    ISubsystemProvider* GetSubsystemProvider() const override { return provider.get(); }

    bool HasProvider() const override { return provider != nullptr; }

    EngineSystemDescriptor GetDescriptor() const override {
        return EngineSystemDescriptor::Create<IIntegrationTestSubsystem, IntegrationTestSubsystem,
                                              IntegrationTestProvider>("Integration Test");
    }

    const std::string& GetSubsystemName() const override {
        static const std::string name = "Integration Test Subsystem";
        return name;
    }

    // Test-specific functionality
    void DoTestOperation() override {
        if (IsRunning()) {
            testValue = 42;
        }
    }

    int GetTestValue() const override { return testValue; }

   private:
    std::unique_ptr<IntegrationTestProvider> provider;
    int testValue = 0;
};

}  // namespace AIEngine

TEST_CASE("Full Subsystem Integration Workflow") {
    // Clean state before test
    auto& manager = AIEngine::EngineSystemManager::GetInstance();
    manager.DestroyAllSystems();

    SUBCASE("Complete registration, creation, lifecycle workflow") {
        // 1. Create descriptor
        auto descriptor =
            AIEngine::EngineSystemDescriptor::Create<AIEngine::IIntegrationTestSubsystem,
                                                     AIEngine::IntegrationTestSubsystem,
                                                     AIEngine::IntegrationTestProvider>(
                "Integration Test System");

        // 2. Register descriptor with manager
        manager.RegisterSystemDescriptor(descriptor);

        // 3. Create subsystem through manager
        auto* subsystem = manager.CreateSystem<AIEngine::IIntegrationTestSubsystem>(descriptor);
        REQUIRE(subsystem != nullptr);
        CHECK(subsystem->HasProvider());

        // 4. Verify initial state
        CHECK_FALSE(subsystem->IsInitialized());
        CHECK_FALSE(subsystem->IsRunning());

        // 5. Initialize subsystem
        CHECK(subsystem->Initialize());
        CHECK(subsystem->IsInitialized());
        CHECK_FALSE(subsystem->IsRunning());

        // 6. Start subsystem
        subsystem->Start();
        CHECK(subsystem->IsInitialized());
        CHECK(subsystem->IsRunning());

        // 7. Use subsystem functionality
        subsystem->DoTestOperation();
        CHECK(subsystem->GetTestValue() == 42);

        // 8. Stop subsystem
        subsystem->Stop();
        CHECK(subsystem->IsInitialized());
        CHECK_FALSE(subsystem->IsRunning());

        // 9. Destroy subsystem
        subsystem->Destroy();
        CHECK_FALSE(subsystem->IsInitialized());
        CHECK_FALSE(subsystem->IsRunning());

        // 10. Cleanup through manager
        manager.DestroySystem<AIEngine::IIntegrationTestSubsystem>();
        CHECK_FALSE(manager.HasSystem<AIEngine::IIntegrationTestSubsystem>());
    }
}

TEST_CASE("Engine Integration with Subsystem Manager") {
    SUBCASE("Engine properly manages subsystem lifecycle") {
        // Clean state
        auto& manager = AIEngine::EngineSystemManager::GetInstance();
        manager.DestroyAllSystems();

        // Register test subsystem before engine initialization
        auto descriptor =
            AIEngine::EngineSystemDescriptor::Create<AIEngine::IIntegrationTestSubsystem,
                                                     AIEngine::IntegrationTestSubsystem,
                                                     AIEngine::IntegrationTestProvider>(
                "Engine Integration Test");
        manager.RegisterSystemDescriptor(descriptor);

        // Create and initialize engine
        AIEngine::EngineConfig config;
        config.windowTitle = "Integration Test";
        config.windowWidth = 800;
        config.windowHeight = 600;
        config.targetFrameRate = 60.0;

        AIEngine::Engine engine(config);

        // Engine initialization should initialize all registered subsystems
        bool engineInitialized = engine.Initialize();
        CHECK(engineInitialized);

        // Create the subsystem after engine init to verify it gets managed
        auto* subsystem = manager.CreateSystem<AIEngine::IIntegrationTestSubsystem>(descriptor);
        REQUIRE(subsystem != nullptr);

        // Subsystem should be automatically initialized and started by engine
        CHECK(subsystem->IsInitialized());
        CHECK(subsystem->IsRunning());

        // Test subsystem functionality
        subsystem->DoTestOperation();
        CHECK(subsystem->GetTestValue() == 42);

        // Engine shutdown should properly cleanup subsystems
        engine.Shutdown();

        // After engine shutdown, subsystem should be cleaned up
        CHECK_FALSE(subsystem->IsRunning());
        CHECK_FALSE(subsystem->IsInitialized());
    }
}

TEST_CASE("Multiple Subsystems Integration") {
    // Clean state
    auto& manager = AIEngine::EngineSystemManager::GetInstance();
    manager.DestroyAllSystems();

    SUBCASE("Manager lifecycle operations work with multiple subsystems") {
        // Create multiple different subsystem descriptors
        auto descriptor1 =
            AIEngine::EngineSystemDescriptor::Create<AIEngine::IIntegrationTestSubsystem,
                                                     AIEngine::IntegrationTestSubsystem,
                                                     AIEngine::IntegrationTestProvider>(
                "Test System 1");

        // Register descriptors
        manager.RegisterSystemDescriptor(descriptor1);

        // Create multiple subsystems
        auto* subsystem1 = manager.CreateSystem<AIEngine::IIntegrationTestSubsystem>(descriptor1);

        REQUIRE(subsystem1 != nullptr);

        // Test manager-level lifecycle operations
        CHECK(manager.InitializeAllSystems());
        CHECK(subsystem1->IsInitialized());

        manager.StartAllSystems();
        CHECK(subsystem1->IsRunning());

        // Test functionality
        subsystem1->DoTestOperation();
        CHECK(subsystem1->GetTestValue() == 42);

        manager.StopAllSystems();
        CHECK_FALSE(subsystem1->IsRunning());
        CHECK(subsystem1->IsInitialized());  // Should still be initialized

        manager.DestroyAllSystems();
        CHECK_FALSE(subsystem1->IsInitialized());
        CHECK_FALSE(manager.HasSystem<AIEngine::IIntegrationTestSubsystem>());
    }
}

TEST_CASE("GetEngineSubsystem Macro Integration") {
    // Clean state
    auto& manager = AIEngine::EngineSystemManager::GetInstance();
    manager.DestroyAllSystems();

    SUBCASE("Convenience macro provides correct subsystem access") {
        // Setup
        auto descriptor =
            AIEngine::EngineSystemDescriptor::Create<AIEngine::IIntegrationTestSubsystem,
                                                     AIEngine::IntegrationTestSubsystem,
                                                     AIEngine::IntegrationTestProvider>(
                "Macro Test");
        manager.RegisterSystemDescriptor(descriptor);
        manager.CreateSystem<AIEngine::IIntegrationTestSubsystem>(descriptor);

        // Test macro access
        auto* subsystem = GetEngineSubsystem(AIEngine::IIntegrationTestSubsystem);
        REQUIRE(subsystem != nullptr);

        // Initialize and test functionality through macro
        CHECK(subsystem->Initialize());
        subsystem->Start();
        subsystem->DoTestOperation();
        CHECK(subsystem->GetTestValue() == 42);

        // Verify it's the same instance as direct manager access
        auto* directAccess = manager.GetSystem<AIEngine::IIntegrationTestSubsystem>();
        CHECK(subsystem == directAccess);

        // Cleanup
        manager.DestroyAllSystems();
    }
}