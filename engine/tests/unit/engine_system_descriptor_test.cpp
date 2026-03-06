#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <typeinfo>

#include "AIEngine/core/EngineSystemDescriptor.hpp"
#include "AIEngine/core/IEngineSubsystem.hpp"
#include "AIEngine/core/ISubsystemProvider.hpp"
#include "doctest.h"

namespace AIEngine {

// Mock test interfaces for testing
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

    void DoMockOperation() override {
        // Mock implementation
    }
};

}  // namespace AIEngine

TEST_CASE("EngineSystemDescriptor Creation") {
    SUBCASE("Create descriptor with template parameters") {
        auto descriptor = AIEngine::EngineSystemDescriptor::Create<
            AIEngine::IMockSubsystem, AIEngine::MockSubsystem, AIEngine::MockProvider>(
            "Test System");

        CHECK(descriptor.GetSystemName() == "Test System");
        CHECK(descriptor.GetInterfaceTypeName() == "IMockSubsystem");
        CHECK(descriptor.GetProviderTypeName() == "MockProvider");
        CHECK(descriptor.GetSystemId() > 0);
        CHECK(descriptor.GetInterfaceHash() != 0);
        CHECK(descriptor.GetProviderTypeHash() != 0);
    }

    SUBCASE("Type hash uniqueness") {
        auto descriptor1 = AIEngine::EngineSystemDescriptor::Create<
            AIEngine::IMockSubsystem, AIEngine::MockSubsystem, AIEngine::MockProvider>("System 1");
        auto descriptor2 = AIEngine::EngineSystemDescriptor::Create<
            AIEngine::IMockSubsystem, AIEngine::MockSubsystem, AIEngine::MockProvider>("System 2");

        // Same interface type should have same hash
        CHECK(descriptor1.GetInterfaceHash() == descriptor2.GetInterfaceHash());
        CHECK(descriptor1.GetProviderTypeHash() == descriptor2.GetProviderTypeHash());

        // But different system IDs
        CHECK(descriptor1.GetSystemId() != descriptor2.GetSystemId());
    }
}

TEST_CASE("EngineSystemDescriptor System Creation") {
    SUBCASE("CreateSystem returns valid subsystem instance") {
        auto descriptor = AIEngine::EngineSystemDescriptor::Create<
            AIEngine::IMockSubsystem, AIEngine::MockSubsystem, AIEngine::MockProvider>(
            "Test System");

        auto subsystem = descriptor.CreateSystem<AIEngine::IMockSubsystem>();

        CHECK(subsystem != nullptr);
        CHECK(subsystem->HasProvider());
        CHECK(subsystem->GetSubsystemName() == "Mock Subsystem");
    }

    SUBCASE("Created subsystem has proper provider") {
        auto descriptor = AIEngine::EngineSystemDescriptor::Create<
            AIEngine::IMockSubsystem, AIEngine::MockSubsystem, AIEngine::MockProvider>(
            "Test System");

        auto subsystem = descriptor.CreateSystem<AIEngine::IMockSubsystem>();
        auto* provider = subsystem->GetSubsystemProvider();

        CHECK(provider != nullptr);
        CHECK(provider->GetProviderName() == "Mock Provider");
        CHECK(provider->GetProviderId() == "mock-provider");
    }
}

TEST_CASE("EngineSystemDescriptor Equality") {
    SUBCASE("Equal descriptors compare as equal") {
        auto descriptor1 = AIEngine::EngineSystemDescriptor::Create<
            AIEngine::IMockSubsystem, AIEngine::MockSubsystem, AIEngine::MockProvider>("System");
        auto descriptor2 = AIEngine::EngineSystemDescriptor::Create<
            AIEngine::IMockSubsystem, AIEngine::MockSubsystem, AIEngine::MockProvider>("System");

        CHECK(descriptor1 == descriptor2);
    }
}

TEST_CASE("EngineSystemDescriptor Lifecycle Operations") {
    SUBCASE("Created subsystem lifecycle management") {
        auto descriptor = AIEngine::EngineSystemDescriptor::Create<
            AIEngine::IMockSubsystem, AIEngine::MockSubsystem, AIEngine::MockProvider>(
            "Test System");

        auto subsystem = descriptor.CreateSystem<AIEngine::IMockSubsystem>();

        // Initial state
        CHECK_FALSE(subsystem->IsInitialized());
        CHECK_FALSE(subsystem->IsRunning());

        // Initialize
        CHECK(subsystem->Initialize());
        CHECK(subsystem->IsInitialized());
        CHECK_FALSE(subsystem->IsRunning());

        // Start
        subsystem->Start();
        CHECK(subsystem->IsInitialized());
        CHECK(subsystem->IsRunning());

        // Stop
        subsystem->Stop();
        CHECK(subsystem->IsInitialized());
        CHECK_FALSE(subsystem->IsRunning());

        // Destroy
        subsystem->Destroy();
        CHECK_FALSE(subsystem->IsInitialized());
        CHECK_FALSE(subsystem->IsRunning());
    }

    SUBCASE("Multiple systems from same descriptor are independent") {
        auto descriptor = AIEngine::EngineSystemDescriptor::Create<
            AIEngine::IMockSubsystem, AIEngine::MockSubsystem, AIEngine::MockProvider>(
            "Test System");

        auto system1 = descriptor.CreateSystem<AIEngine::IMockSubsystem>();
        auto system2 = descriptor.CreateSystem<AIEngine::IMockSubsystem>();

        // Systems should be independent instances
        CHECK(system1 != system2);
        CHECK(system1->GetSubsystemProvider() != system2->GetSubsystemProvider());

        // Initialize only first system
        system1->Initialize();
        CHECK(system1->IsInitialized());
        CHECK_FALSE(system2->IsInitialized());
    }
}

TEST_CASE("EngineSystemDescriptor Edge Cases") {
    SUBCASE("Empty system name handling") {
        auto descriptor = AIEngine::EngineSystemDescriptor::Create<
            AIEngine::IMockSubsystem, AIEngine::MockSubsystem, AIEngine::MockProvider>("");

        CHECK(descriptor.GetSystemName() == "");

        // Should still create valid system
        auto subsystem = descriptor.CreateSystem<AIEngine::IMockSubsystem>();
        CHECK(subsystem != nullptr);
        CHECK(subsystem->HasProvider());
    }

    SUBCASE("System metadata consistency") {
        const std::string systemName = "Custom Test System";
        auto descriptor = AIEngine::EngineSystemDescriptor::Create<
            AIEngine::IMockSubsystem, AIEngine::MockSubsystem, AIEngine::MockProvider>(systemName);

        CHECK(descriptor.GetSystemName() == systemName);
        CHECK(descriptor.GetImplementationId() == "MockSubsystem_MockProvider");
        CHECK(descriptor.GetInterfaceHash() == typeid(AIEngine::IMockSubsystem).hash_code());
        CHECK(descriptor.GetProviderTypeHash() == typeid(AIEngine::MockProvider).hash_code());
    }
}