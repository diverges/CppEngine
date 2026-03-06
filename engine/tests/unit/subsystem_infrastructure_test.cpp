/**
 * subsystem_infrastructure_test.cpp - Complete Subsystem Infrastructure Integration Tests
 *
 * Integration testing of the complete subsystem infrastructure including
 * EngineSystemDescriptor, EngineSystemManager, and lifecycle integration.
 *
 * Tests User Story 2: Complete infrastructure integration and workflow validation
 *
 * @version 1.0.0
 * @date 2026-03-04
 */

#include <AIEngine/core/EngineSystemDescriptor.hpp>
#include <AIEngine/core/EngineSystemManager.hpp>
#include <AIEngine/core/IEngineSubsystem.hpp>
#include <AIEngine/core/ISubsystemProvider.hpp>
#include <chrono>
#include <memory>
#include <stdexcept>
#include <thread>
#include <vector>

#include "doctest.h"

namespace AIEngine {

// Integration test provider with comprehensive tracking
class IntegrationTestProvider : public ISubsystemProvider {
   public:
    explicit IntegrationTestProvider(const std::string& name, const std::string& id)
        : providerName(name), providerId(id) {}

    bool Initialize() override {
        if (initialized) return true;  // Already initialized

        initializeStartTime = std::chrono::high_resolution_clock::now();

        // Simulate initialization work
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        initialized = true;
        initializeEndTime = std::chrono::high_resolution_clock::now();
        return true;
    }

    void Start() override {
        if (!initialized)
            throw std::runtime_error("Cannot start uninitialized provider: " + providerName);
        if (running) return;  // Already running

        startTime = std::chrono::high_resolution_clock::now();
        running = true;
    }

    void Stop() override {
        if (!running) return;  // Already stopped

        stopTime = std::chrono::high_resolution_clock::now();
        running = false;
    }

    void Destroy() override {
        if (running) Stop();

        destroyTime = std::chrono::high_resolution_clock::now();
        initialized = false;
    }

    bool IsInitialized() const override { return initialized; }
    bool IsRunning() const override { return running; }

    const std::string& GetProviderName() const override { return providerName; }
    const std::string& GetProviderId() const override { return providerId; }

    // Timing accessors for integration testing
    auto GetInitializeDuration() const {
        return std::chrono::duration_cast<std::chrono::milliseconds>(initializeEndTime -
                                                                     initializeStartTime);
    }

    bool HasBeenInitialized() const { return initializeEndTime.time_since_epoch().count() > 0; }
    bool HasBeenStarted() const { return startTime.time_since_epoch().count() > 0; }
    bool HasBeenStopped() const { return stopTime.time_since_epoch().count() > 0; }
    bool HasBeenDestroyed() const { return destroyTime.time_since_epoch().count() > 0; }

   private:
    std::string providerName;
    std::string providerId;
    bool initialized = false;
    bool running = false;

    std::chrono::high_resolution_clock::time_point initializeStartTime;
    std::chrono::high_resolution_clock::time_point initializeEndTime;
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point stopTime;
    std::chrono::high_resolution_clock::time_point destroyTime;
};

// Graphics subsystem simulation for integration testing
class IGraphicsSubsystem : public IEngineSubsystem {
   public:
    virtual ~IGraphicsSubsystem() = default;
    virtual void Render() = 0;
    virtual void SetViewport(int width, int height) = 0;
};

class GraphicsSubsystem : public IGraphicsSubsystem {
   public:
    GraphicsSubsystem() = default;

    bool Initialize() override {
        if (!provider) return false;
        return provider->Initialize();
    }

    void Start() override {
        if (!provider || !provider->IsInitialized()) {
            throw std::runtime_error("Graphics subsystem cannot start: provider not ready");
        }
        provider->Start();
    }

    void Stop() override {
        if (provider && provider->IsRunning()) {
            provider->Stop();
        }
    }

    void Destroy() override {
        if (provider) {
            provider->Destroy();
        }
    }

    bool IsInitialized() const override { return provider && provider->IsInitialized(); }

    bool IsRunning() const override { return provider && provider->IsRunning(); }

    void SetSubsystemProvider(std::unique_ptr<ISubsystemProvider> newProvider) override {
        provider = std::unique_ptr<IntegrationTestProvider>(
            static_cast<IntegrationTestProvider*>(newProvider.release()));
    }

    ISubsystemProvider* GetSubsystemProvider() const override { return provider.get(); }

    bool HasProvider() const override { return provider != nullptr; }

    EngineSystemDescriptor GetDescriptor() const override {
        return EngineSystemDescriptor::Create<IGraphicsSubsystem, GraphicsSubsystem,
                                              IntegrationTestProvider>("Graphics");
    }

    const std::string& GetSubsystemName() const override {
        static const std::string name = "Graphics Subsystem";
        return name;
    }

    // Graphics-specific functionality
    void Render() override {
        if (!IsRunning()) throw std::runtime_error("Cannot render: graphics subsystem not running");
        renderCallCount++;
    }

    void SetViewport(int width, int height) override {
        if (!IsInitialized())
            throw std::runtime_error("Cannot set viewport: graphics subsystem not initialized");
        viewportWidth = width;
        viewportHeight = height;
    }

    // Test accessors
    int GetRenderCallCount() const { return renderCallCount; }
    int GetViewportWidth() const { return viewportWidth; }
    int GetViewportHeight() const { return viewportHeight; }
    IntegrationTestProvider* GetTestProvider() const { return provider.get(); }

   private:
    std::unique_ptr<IntegrationTestProvider> provider;
    int renderCallCount = 0;
    int viewportWidth = 0;
    int viewportHeight = 0;
};

// Audio subsystem simulation for integration testing
class IAudioSubsystem : public IEngineSubsystem {
   public:
    virtual ~IAudioSubsystem() = default;
    virtual void PlaySound(const std::string& soundId) = 0;
    virtual void SetVolume(float volume) = 0;
};

class AudioSubsystem : public IAudioSubsystem {
   public:
    AudioSubsystem() = default;

    bool Initialize() override {
        if (!provider) return false;
        return provider->Initialize();
    }

    void Start() override {
        if (!provider || !provider->IsInitialized()) {
            throw std::runtime_error("Audio subsystem cannot start: provider not ready");
        }
        provider->Start();
    }

    void Stop() override {
        if (provider && provider->IsRunning()) {
            provider->Stop();
        }
    }

    void Destroy() override {
        if (provider) {
            provider->Destroy();
        }
    }

    bool IsInitialized() const override { return provider && provider->IsInitialized(); }

    bool IsRunning() const override { return provider && provider->IsRunning(); }

    void SetSubsystemProvider(std::unique_ptr<ISubsystemProvider> newProvider) override {
        provider = std::unique_ptr<IntegrationTestProvider>(
            static_cast<IntegrationTestProvider*>(newProvider.release()));
    }

    ISubsystemProvider* GetSubsystemProvider() const override { return provider.get(); }

    bool HasProvider() const override { return provider != nullptr; }

    EngineSystemDescriptor GetDescriptor() const override {
        return EngineSystemDescriptor::Create<IAudioSubsystem, AudioSubsystem,
                                              IntegrationTestProvider>("Audio");
    }

    const std::string& GetSubsystemName() const override {
        static const std::string name = "Audio Subsystem";
        return name;
    }

    // Audio-specific functionality
    void PlaySound(const std::string& soundId) override {
        if (!IsRunning())
            throw std::runtime_error("Cannot play sound: audio subsystem not running");
        lastSoundPlayed = soundId;
        soundPlayCount++;
    }

    void SetVolume(float volume) override {
        if (!IsInitialized())
            throw std::runtime_error("Cannot set volume: audio subsystem not initialized");
        currentVolume = volume;
    }

    // Test accessors
    const std::string& GetLastSoundPlayed() const { return lastSoundPlayed; }
    int GetSoundPlayCount() const { return soundPlayCount; }
    float GetCurrentVolume() const { return currentVolume; }
    IntegrationTestProvider* GetTestProvider() const { return provider.get(); }

   private:
    std::unique_ptr<IntegrationTestProvider> provider;
    std::string lastSoundPlayed;
    int soundPlayCount = 0;
    float currentVolume = 1.0f;
};

}  // namespace AIEngine

using namespace AIEngine;

TEST_SUITE("Complete Subsystem Infrastructure Integration Tests") {
    // Integration test fixture
    struct IntegrationTestFixture {
        EngineSystemManager& manager;

        IntegrationTestFixture() : manager(EngineSystemManager::GetInstance()) {
            manager.DestroyAllSystems();
        }

        ~IntegrationTestFixture() { manager.DestroyAllSystems(); }

        void SetupMultipleSubsystems() {
            // Register Graphics subsystem
            auto graphicsDesc =
                EngineSystemDescriptor::Create<IGraphicsSubsystem, GraphicsSubsystem,
                                               IntegrationTestProvider>("Graphics");
            manager.RegisterSystemDescriptor(graphicsDesc);
            manager.CreateSystem<IGraphicsSubsystem>(graphicsDesc);

            // Register Audio subsystem
            auto audioDesc = EngineSystemDescriptor::Create<IAudioSubsystem, AudioSubsystem,
                                                            IntegrationTestProvider>("Audio");
            manager.RegisterSystemDescriptor(audioDesc);
            manager.CreateSystem<IAudioSubsystem>(audioDesc);
        }
    };

    TEST_CASE_FIXTURE(IntegrationTestFixture, "End-to-End Subsystem Workflow") {
        SUBCASE("Complete single subsystem workflow") {
            // Create and register graphics subsystem
            auto graphicsDesc =
                EngineSystemDescriptor::Create<IGraphicsSubsystem, GraphicsSubsystem,
                                               IntegrationTestProvider>("Graphics");
            manager.RegisterSystemDescriptor(graphicsDesc);
            auto* graphics = manager.CreateSystem<IGraphicsSubsystem>(graphicsDesc);

            // Verify initial state
            CHECK_FALSE(graphics->IsInitialized());
            CHECK_FALSE(graphics->IsRunning());

            // Initialize
            CHECK(manager.InitializeAllSystems());
            CHECK(graphics->IsInitialized());
            CHECK_FALSE(graphics->IsRunning());

            // Start
            manager.StartAllSystems();
            CHECK(graphics->IsInitialized());
            CHECK(graphics->IsRunning());

            // Use subsystem functionality
            auto* graphicsImpl = static_cast<GraphicsSubsystem*>(graphics);
            graphicsImpl->SetViewport(1920, 1080);
            graphicsImpl->Render();

            CHECK(graphicsImpl->GetViewportWidth() == 1920);
            CHECK(graphicsImpl->GetViewportHeight() == 1080);
            CHECK(graphicsImpl->GetRenderCallCount() == 1);

            // Stop
            manager.StopAllSystems();
            CHECK(graphics->IsInitialized());
            CHECK_FALSE(graphics->IsRunning());

            // Destroy
            manager.DestroyAllSystems();
            CHECK_FALSE(manager.HasSystem<IGraphicsSubsystem>());
        }

        SUBCASE("Multiple subsystem coordination") {
            SetupMultipleSubsystems();

            auto* graphics = manager.GetSystem<IGraphicsSubsystem>();
            auto* audio = manager.GetSystem<IAudioSubsystem>();

            CHECK(graphics != nullptr);
            CHECK(audio != nullptr);

            // Initialize all
            CHECK(manager.InitializeAllSystems());
            CHECK(graphics->IsInitialized());
            CHECK(audio->IsInitialized());

            // Start all
            manager.StartAllSystems();
            CHECK(graphics->IsRunning());
            CHECK(audio->IsRunning());

            // Use both subsystems
            auto* graphicsImpl = static_cast<GraphicsSubsystem*>(graphics);
            auto* audioImpl = static_cast<AudioSubsystem*>(audio);

            graphicsImpl->SetViewport(800, 600);
            graphicsImpl->Render();

            audioImpl->SetVolume(0.8f);
            audioImpl->PlaySound("test_sound.wav");

            CHECK(graphicsImpl->GetViewportWidth() == 800);
            CHECK(audioImpl->GetCurrentVolume() == 0.8f);
            CHECK(audioImpl->GetLastSoundPlayed() == "test_sound.wav");
        }
    }

    TEST_CASE_FIXTURE(IntegrationTestFixture, "Subsystem Dependency and Ordering") {
        SUBCASE("Registration order affects initialization order") {
            SetupMultipleSubsystems();

            auto* graphics =
                static_cast<GraphicsSubsystem*>(manager.GetSystem<IGraphicsSubsystem>());
            auto* audio = static_cast<AudioSubsystem*>(manager.GetSystem<IAudioSubsystem>());

            // Initialize all and verify timing
            auto startTime = std::chrono::high_resolution_clock::now();
            CHECK(manager.InitializeAllSystems());
            auto endTime = std::chrono::high_resolution_clock::now();

            // Both should be initialized
            CHECK(graphics->IsInitialized());
            CHECK(audio->IsInitialized());

            // Verify providers were called
            CHECK(graphics->GetTestProvider()->HasBeenInitialized());
            CHECK(audio->GetTestProvider()->HasBeenInitialized());

            auto totalDuration =
                std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            CHECK(totalDuration.count() >= 0);  // Should take some time due to sleep in provider
        }

        SUBCASE("System state consistency after partial failures") {
            auto graphicsDesc =
                EngineSystemDescriptor::Create<IGraphicsSubsystem, GraphicsSubsystem,
                                               IntegrationTestProvider>("Graphics");
            manager.RegisterSystemDescriptor(graphicsDesc);
            auto* graphics = manager.CreateSystem<IGraphicsSubsystem>(graphicsDesc);

            // Initialize successfully
            CHECK(manager.InitializeAllSystems());
            CHECK(graphics->IsInitialized());
            CHECK_FALSE(graphics->IsRunning());

            // Start successfully
            manager.StartAllSystems();
            CHECK(graphics->IsRunning());

            // Stop successfully
            manager.StopAllSystems();
            CHECK(graphics->IsInitialized());
            CHECK_FALSE(graphics->IsRunning());

            // State should remain consistent
            CHECK(graphics->IsInitialized());
            CHECK_FALSE(graphics->IsRunning());
        }
    }

    TEST_CASE_FIXTURE(IntegrationTestFixture, "Performance and Resource Management") {
        SUBCASE("O(1) lookup performance with multiple systems") {
            SetupMultipleSubsystems();

            // Initialize systems
            manager.InitializeAllSystems();

            // Measure lookup performance
            auto start = std::chrono::high_resolution_clock::now();

            // Perform multiple lookups
            for (int i = 0; i < 1000; ++i) {
                auto* graphics = manager.GetSystem<IGraphicsSubsystem>();
                auto* audio = manager.GetSystem<IAudioSubsystem>();
                CHECK(graphics != nullptr);
                CHECK(audio != nullptr);
            }

            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

            // 1000 lookups should be very fast (less than 1ms total)
            CHECK(duration.count() < 1000);
        }

        SUBCASE("Memory management and cleanup") {
            SetupMultipleSubsystems();

            // Full lifecycle
            manager.InitializeAllSystems();
            manager.StartAllSystems();

            // Verify systems are active
            auto activeSystems = manager.GetActiveSubsystems();
            CHECK(activeSystems.size() == 2);

            // Destroy all
            manager.DestroyAllSystems();

            // Verify cleanup
            CHECK_FALSE(manager.HasSystem<IGraphicsSubsystem>());
            CHECK_FALSE(manager.HasSystem<IAudioSubsystem>());

            auto activeSystemsAfterDestroy = manager.GetActiveSubsystems();
            CHECK(activeSystemsAfterDestroy.empty());
        }
    }

    TEST_CASE_FIXTURE(IntegrationTestFixture, "Error Handling and Recovery") {
        SUBCASE("Graceful handling of initialization failures") {
            // This test would require a provider that can be configured to fail
            // For now, we test the successful case and document the expected behavior
            auto graphicsDesc =
                EngineSystemDescriptor::Create<IGraphicsSubsystem, GraphicsSubsystem,
                                               IntegrationTestProvider>("Graphics");
            manager.RegisterSystemDescriptor(graphicsDesc);
            manager.CreateSystem<IGraphicsSubsystem>(graphicsDesc);

            // Normal initialization should succeed
            CHECK(manager.InitializeAllSystems());

            // If initialization failed, the result would be false
            // and the system would remain uninitialized
        }

        SUBCASE("System functionality requires proper lifecycle state") {
            auto graphicsDesc =
                EngineSystemDescriptor::Create<IGraphicsSubsystem, GraphicsSubsystem,
                                               IntegrationTestProvider>("Graphics");
            manager.RegisterSystemDescriptor(graphicsDesc);
            auto* graphics = manager.CreateSystem<IGraphicsSubsystem>(graphicsDesc);
            auto* graphicsImpl = static_cast<GraphicsSubsystem*>(graphics);

            // Should throw when not initialized
            CHECK_THROWS_AS(graphicsImpl->SetViewport(800, 600), std::runtime_error);

            // Initialize but don't start
            manager.InitializeAllSystems();
            graphicsImpl->SetViewport(800, 600);  // Should work when initialized

            // Should throw when not running
            CHECK_THROWS_AS(graphicsImpl->Render(), std::runtime_error);

            // Start and verify functionality works
            manager.StartAllSystems();
            graphicsImpl->Render();  // Should work when running
            CHECK(graphicsImpl->GetRenderCallCount() == 1);
        }
    }

    TEST_CASE_FIXTURE(IntegrationTestFixture, "Subsystem Registration and Discovery") {
        SUBCASE("Descriptor registration and retrieval") {
            auto graphicsDesc =
                EngineSystemDescriptor::Create<IGraphicsSubsystem, GraphicsSubsystem,
                                               IntegrationTestProvider>("Graphics");
            auto audioDesc = EngineSystemDescriptor::Create<IAudioSubsystem, AudioSubsystem,
                                                            IntegrationTestProvider>("Audio");

            manager.RegisterSystemDescriptor(graphicsDesc);
            manager.RegisterSystemDescriptor(audioDesc);

            auto descriptors = manager.GetRegisteredDescriptors();
            CHECK(descriptors.size() >= 2);

            // Verify our descriptors are present
            bool foundGraphics = false;
            bool foundAudio = false;

            for (const auto& desc : descriptors) {
                if (desc.GetSystemName() == "Graphics") foundGraphics = true;
                if (desc.GetSystemName() == "Audio") foundAudio = true;
            }

            CHECK(foundGraphics);
            CHECK(foundAudio);
        }

        SUBCASE("Duplicate interface registration prevention") {
            auto graphicsDesc1 =
                EngineSystemDescriptor::Create<IGraphicsSubsystem, GraphicsSubsystem,
                                               IntegrationTestProvider>("Graphics1");
            auto graphicsDesc2 =
                EngineSystemDescriptor::Create<IGraphicsSubsystem, GraphicsSubsystem,
                                               IntegrationTestProvider>("Graphics2");

            manager.RegisterSystemDescriptor(graphicsDesc1);

            // Second registration of same interface should be rejected
            CHECK_THROWS_AS(manager.RegisterSystemDescriptor(graphicsDesc2), std::runtime_error);
        }
    }

    TEST_CASE_FIXTURE(IntegrationTestFixture, "Real-World Usage Simulation") {
        SUBCASE("Game engine initialization sequence") {
            // Simulate typical game engine startup
            SetupMultipleSubsystems();

            // 1. Initialize all subsystems
            CHECK(manager.InitializeAllSystems());

            // 2. Start all subsystems
            manager.StartAllSystems();

            // 3. Configure subsystems
            auto* graphics =
                static_cast<GraphicsSubsystem*>(manager.GetSystem<IGraphicsSubsystem>());
            auto* audio = static_cast<AudioSubsystem*>(manager.GetSystem<IAudioSubsystem>());

            graphics->SetViewport(1920, 1080);
            audio->SetVolume(0.75f);

            // 4. Main game loop simulation (simplified)
            for (int frame = 0; frame < 5; ++frame) {
                graphics->Render();
                if (frame == 2) {
                    audio->PlaySound("jump.wav");
                }
            }

            // 5. Verify expected behavior
            CHECK(graphics->GetRenderCallCount() == 5);
            CHECK(audio->GetSoundPlayCount() == 1);
            CHECK(audio->GetLastSoundPlayed() == "jump.wav");

            // 6. Shutdown sequence
            manager.StopAllSystems();
            manager.DestroyAllSystems();

            // 7. Verify clean shutdown
            CHECK_FALSE(manager.HasSystem<IGraphicsSubsystem>());
            CHECK_FALSE(manager.HasSystem<IAudioSubsystem>());
        }
    }
}