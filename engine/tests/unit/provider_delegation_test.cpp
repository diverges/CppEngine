/**
 * provider_delegation_test.cpp - Provider Delegation Pattern Unit Tests
 *
 * Comprehensive testing of provider delegation patterns for cross-platform
 * compatibility including provider assignment, lifecycle delegation, and
 * type-safe provider access patterns.
 *
 * Tests User Story 3: Cross-Platform Compatibility through provider pattern
 *
 * @version 1.0.0
 * @date 2026-03-04
 */

#include <AIEngine/core/EngineSystemDescriptor.hpp>
#include <AIEngine/core/EngineSystemManager.hpp>
#include <AIEngine/core/IEngineSubsystem.hpp>
#include <AIEngine/core/ISubsystemProvider.hpp>
#include <memory>
#include <stdexcept>
#include <string>

#include "doctest.h"

namespace AIEngine {

// Platform-specific provider interfaces for testing cross-platform scenarios
class ICrossPlatformProvider : public ISubsystemProvider {
   public:
    virtual ~ICrossPlatformProvider() = default;
    virtual std::string GetPlatformName() const = 0;
    virtual bool SupportsPlatformFeature(const std::string& feature) const = 0;
};

// Windows provider simulation
class WindowsProvider : public ICrossPlatformProvider {
   public:
    explicit WindowsProvider(bool shouldFailInit = false)
        : shouldFailInitialization(shouldFailInit) {}

    bool Initialize() override {
        if (shouldFailInitialization) return false;
        initialized = true;
        initializeCallCount++;
        return true;
    }

    void Start() override {
        if (!initialized) throw std::runtime_error("Windows provider not initialized");
        running = true;
        startCallCount++;
    }

    void Stop() override {
        if (running) {
            running = false;
            stopCallCount++;
        }
    }

    void Destroy() override {
        if (running) Stop();
        initialized = false;
        destroyCallCount++;
    }

    bool IsInitialized() const override { return initialized; }
    bool IsRunning() const override { return running; }

    const std::string& GetProviderName() const override {
        static const std::string name = "Windows Provider";
        return name;
    }

    const std::string& GetProviderId() const override {
        static const std::string id = "windows_provider";
        return id;
    }

    std::string GetPlatformName() const override { return "Windows"; }

    bool SupportsPlatformFeature(const std::string& feature) const override {
        return feature == "DirectX" || feature == "Win32API" || feature == "MSVC";
    }

    // Test accessors
    int GetInitializeCallCount() const { return initializeCallCount; }
    int GetStartCallCount() const { return startCallCount; }
    int GetStopCallCount() const { return stopCallCount; }
    int GetDestroyCallCount() const { return destroyCallCount; }
    void ResetCallCounts() {
        initializeCallCount = startCallCount = stopCallCount = destroyCallCount = 0;
    }

   private:
    bool initialized = false;
    bool running = false;
    bool shouldFailInitialization = false;
    int initializeCallCount = 0;
    int startCallCount = 0;
    int stopCallCount = 0;
    int destroyCallCount = 0;
};

// Linux provider simulation
class LinuxProvider : public ICrossPlatformProvider {
   public:
    LinuxProvider() = default;

    bool Initialize() override {
        initialized = true;
        return true;
    }

    void Start() override {
        if (!initialized) throw std::runtime_error("Linux provider not initialized");
        running = true;
    }

    void Stop() override { running = false; }

    void Destroy() override {
        if (running) Stop();
        initialized = false;
    }

    bool IsInitialized() const override { return initialized; }
    bool IsRunning() const override { return running; }

    const std::string& GetProviderName() const override {
        static const std::string name = "Linux Provider";
        return name;
    }

    const std::string& GetProviderId() const override {
        static const std::string id = "linux_provider";
        return id;
    }

    std::string GetPlatformName() const override { return "Linux"; }

    bool SupportsPlatformFeature(const std::string& feature) const override {
        return feature == "OpenGL" || feature == "X11" || feature == "GCC";
    }

   private:
    bool initialized = false;
    bool running = false;
};

// Cross-platform subsystem that uses providers for platform-specific functionality
class ICrossPlatformSubsystem : public IEngineSubsystem {
   public:
    virtual ~ICrossPlatformSubsystem() = default;
    virtual std::string GetCurrentPlatform() const = 0;
    virtual bool SupportsFeature(const std::string& feature) const = 0;
    virtual ICrossPlatformProvider* GetCrossPlatformProvider() const = 0;
};

class CrossPlatformSubsystem : public ICrossPlatformSubsystem {
   public:
    CrossPlatformSubsystem() = default;

    // IEngineSubsystem implementation - delegates completely to provider
    bool Initialize() override {
        if (!provider) {
            lastError = "No provider set for cross-platform subsystem";
            return false;
        }

        bool success = provider->Initialize();
        if (!success) {
            lastError = "Provider initialization failed: " + provider->GetProviderName();
        } else {
            lastError.clear();
        }

        initializeCallCount++;
        return success;
    }

    void Start() override {
        if (!provider) {
            throw std::runtime_error("Cannot start: no provider set");
        }
        if (!provider->IsInitialized()) {
            throw std::runtime_error("Cannot start: provider not initialized");
        }

        provider->Start();
        startCallCount++;
    }

    void Stop() override {
        if (provider && provider->IsRunning()) {
            provider->Stop();
        }
        stopCallCount++;
    }

    void Destroy() override {
        if (provider) {
            provider->Destroy();
        }
        destroyCallCount++;
    }

    bool IsInitialized() const override { return provider && provider->IsInitialized(); }

    bool IsRunning() const override { return provider && provider->IsRunning(); }

    void SetSubsystemProvider(std::unique_ptr<ISubsystemProvider> newProvider) override {
        provider = std::unique_ptr<ICrossPlatformProvider>(
            static_cast<ICrossPlatformProvider*>(newProvider.release()));
    }

    ISubsystemProvider* GetSubsystemProvider() const override { return provider.get(); }

    bool HasProvider() const override { return provider != nullptr; }

    EngineSystemDescriptor GetDescriptor() const override {
        return EngineSystemDescriptor::Create<ICrossPlatformSubsystem, CrossPlatformSubsystem,
                                              WindowsProvider>("CrossPlatform");
    }

    const std::string& GetSubsystemName() const override {
        static const std::string name = "Cross-Platform Subsystem";
        return name;
    }

    // Cross-platform specific functionality - delegates to provider
    std::string GetCurrentPlatform() const override {
        if (!provider) return "Unknown";
        return provider->GetPlatformName();
    }

    bool SupportsFeature(const std::string& feature) const override {
        if (!provider) return false;
        return provider->SupportsPlatformFeature(feature);
    }

    ICrossPlatformProvider* GetCrossPlatformProvider() const override { return provider.get(); }

    // Test accessors
    const std::string& GetLastError() const { return lastError; }
    int GetInitializeCallCount() const { return initializeCallCount; }
    int GetStartCallCount() const { return startCallCount; }
    int GetStopCallCount() const { return stopCallCount; }
    int GetDestroyCallCount() const { return destroyCallCount; }
    void ResetCallCounts() {
        initializeCallCount = startCallCount = stopCallCount = destroyCallCount = 0;
    }

   private:
    std::unique_ptr<ICrossPlatformProvider> provider;
    std::string lastError;
    int initializeCallCount = 0;
    int startCallCount = 0;
    int stopCallCount = 0;
    int destroyCallCount = 0;
};

}  // namespace AIEngine

using namespace AIEngine;

TEST_SUITE("Provider Delegation Pattern Tests") {
    // Test fixture for provider delegation testing
    struct ProviderDelegationFixture {
        std::unique_ptr<CrossPlatformSubsystem> subsystem;

        ProviderDelegationFixture() { subsystem = std::make_unique<CrossPlatformSubsystem>(); }

        void SetupWindowsProvider(bool shouldFail = false) {
            auto provider = std::make_unique<WindowsProvider>(shouldFail);
            subsystem->SetSubsystemProvider(std::move(provider));
        }

        void SetupLinuxProvider() {
            auto provider = std::make_unique<LinuxProvider>();
            subsystem->SetSubsystemProvider(std::move(provider));
        }
    };

    TEST_CASE_FIXTURE(ProviderDelegationFixture, "Provider Assignment and Validation") {
        SUBCASE("Subsystem without provider fails gracefully") {
            CHECK_FALSE(subsystem->HasProvider());
            CHECK_FALSE(subsystem->IsInitialized());
            CHECK_FALSE(subsystem->IsRunning());

            // Should fail initialization without provider
            CHECK_FALSE(subsystem->Initialize());
            CHECK(subsystem->GetLastError() == "No provider set for cross-platform subsystem");

            // Should throw on start without provider
            CHECK_THROWS_AS(subsystem->Start(), std::runtime_error);
        }

        SUBCASE("Provider assignment enables functionality") {
            SetupWindowsProvider();

            CHECK(subsystem->HasProvider());
            CHECK(subsystem->GetSubsystemProvider() != nullptr);

            // Should now be able to initialize
            CHECK(subsystem->Initialize());
            CHECK(subsystem->IsInitialized());
            CHECK(subsystem->GetLastError().empty());
        }

        SUBCASE("Type-safe provider access") {
            SetupWindowsProvider();

            auto* crossPlatformProvider = subsystem->GetCrossPlatformProvider();
            REQUIRE(crossPlatformProvider != nullptr);

            CHECK(crossPlatformProvider->GetPlatformName() == "Windows");
            CHECK(crossPlatformProvider->SupportsPlatformFeature("DirectX"));
            CHECK_FALSE(crossPlatformProvider->SupportsPlatformFeature("OpenGL"));
        }
    }

    TEST_CASE_FIXTURE(ProviderDelegationFixture, "Lifecycle Delegation") {
        SUBCASE("Initialization delegates to provider") {
            SetupWindowsProvider();
            auto* provider = static_cast<WindowsProvider*>(subsystem->GetSubsystemProvider());

            // Initial state
            CHECK_FALSE(subsystem->IsInitialized());
            CHECK_FALSE(provider->IsInitialized());
            CHECK(provider->GetInitializeCallCount() == 0);

            // Initialize subsystem
            CHECK(subsystem->Initialize());

            // Verify delegation occurred
            CHECK(subsystem->IsInitialized());
            CHECK(provider->IsInitialized());
            CHECK(provider->GetInitializeCallCount() == 1);
            CHECK(subsystem->GetInitializeCallCount() == 1);
        }

        SUBCASE("Start delegates to provider") {
            SetupWindowsProvider();
            auto* provider = static_cast<WindowsProvider*>(subsystem->GetSubsystemProvider());

            // Initialize first
            subsystem->Initialize();

            // Start subsystem
            subsystem->Start();

            // Verify delegation
            CHECK(subsystem->IsRunning());
            CHECK(provider->IsRunning());
            CHECK(provider->GetStartCallCount() == 1);
            CHECK(subsystem->GetStartCallCount() == 1);
        }

        SUBCASE("Stop delegates to provider") {
            SetupWindowsProvider();
            auto* provider = static_cast<WindowsProvider*>(subsystem->GetSubsystemProvider());

            // Initialize and start
            subsystem->Initialize();
            subsystem->Start();
            CHECK(subsystem->IsRunning());

            // Stop subsystem
            subsystem->Stop();

            // Verify delegation
            CHECK_FALSE(subsystem->IsRunning());
            CHECK_FALSE(provider->IsRunning());
            CHECK(provider->GetStopCallCount() == 1);
            CHECK(subsystem->GetStopCallCount() == 1);
        }

        SUBCASE("Destroy delegates to provider") {
            SetupWindowsProvider();
            auto* provider = static_cast<WindowsProvider*>(subsystem->GetSubsystemProvider());

            // Initialize and start
            subsystem->Initialize();
            subsystem->Start();

            // Destroy subsystem
            subsystem->Destroy();

            // Verify delegation
            CHECK_FALSE(subsystem->IsInitialized());
            CHECK_FALSE(provider->IsInitialized());
            CHECK(provider->GetDestroyCallCount() == 1);
            CHECK(subsystem->GetDestroyCallCount() == 1);
        }
    }

    TEST_CASE_FIXTURE(ProviderDelegationFixture, "Provider Failure Handling") {
        SUBCASE("Provider initialization failure propagates") {
            SetupWindowsProvider(true);  // Set to fail initialization

            // Attempt to initialize
            CHECK_FALSE(subsystem->Initialize());
            CHECK_FALSE(subsystem->IsInitialized());

            // Error message should indicate provider failure
            std::string error = subsystem->GetLastError();
            CHECK(error.find("Provider initialization failed") != std::string::npos);
            CHECK(error.find("Windows Provider") != std::string::npos);
        }

        SUBCASE("Provider start failure without initialization") {
            SetupWindowsProvider();

            // Try to start without initializing
            CHECK_THROWS_AS(subsystem->Start(), std::runtime_error);
        }

        SUBCASE("Provider lifecycle constraint enforcement") {
            SetupWindowsProvider();

            // Cannot start without initialization
            CHECK_THROWS_AS(subsystem->Start(), std::runtime_error);

            // Initialize, then start should work
            subsystem->Initialize();
            subsystem->Start();  // Should not throw

            CHECK(subsystem->IsRunning());
        }
    }

    TEST_CASE_FIXTURE(ProviderDelegationFixture, "Cross-Platform Provider Functionality") {
        SUBCASE("Windows provider platform-specific features") {
            SetupWindowsProvider();
            subsystem->Initialize();

            CHECK(subsystem->GetCurrentPlatform() == "Windows");
            CHECK(subsystem->SupportsFeature("DirectX"));
            CHECK(subsystem->SupportsFeature("Win32API"));
            CHECK(subsystem->SupportsFeature("MSVC"));
            CHECK_FALSE(subsystem->SupportsFeature("X11"));
            CHECK_FALSE(subsystem->SupportsFeature("GCC"));
        }

        SUBCASE("Linux provider platform-specific features") {
            SetupLinuxProvider();
            subsystem->Initialize();

            CHECK(subsystem->GetCurrentPlatform() == "Linux");
            CHECK(subsystem->SupportsFeature("OpenGL"));
            CHECK(subsystem->SupportsFeature("X11"));
            CHECK(subsystem->SupportsFeature("GCC"));
            CHECK_FALSE(subsystem->SupportsFeature("DirectX"));
            CHECK_FALSE(subsystem->SupportsFeature("Win32API"));
        }

        SUBCASE("Provider switching simulation") {
            // Start with Windows provider
            SetupWindowsProvider();
            subsystem->Initialize();

            CHECK(subsystem->GetCurrentPlatform() == "Windows");
            CHECK(subsystem->SupportsFeature("DirectX"));

            // Simulate switching to Linux provider (destroy and recreate)
            subsystem->Destroy();
            SetupLinuxProvider();
            subsystem->Initialize();

            CHECK(subsystem->GetCurrentPlatform() == "Linux");
            CHECK(subsystem->SupportsFeature("OpenGL"));
            CHECK_FALSE(subsystem->SupportsFeature("DirectX"));
        }
    }

    TEST_CASE_FIXTURE(ProviderDelegationFixture, "Provider Identification and Metadata") {
        SUBCASE("Provider identification through metadata") {
            SetupWindowsProvider();

            auto* provider = subsystem->GetSubsystemProvider();
            REQUIRE(provider != nullptr);

            CHECK(provider->GetProviderName() == "Windows Provider");
            CHECK(provider->GetProviderId() == "windows_provider");

            auto* crossPlatformProvider = subsystem->GetCrossPlatformProvider();
            CHECK(crossPlatformProvider->GetPlatformName() == "Windows");
        }

        SUBCASE("Different provider metadata") {
            SetupLinuxProvider();

            auto* provider = subsystem->GetSubsystemProvider();
            REQUIRE(provider != nullptr);

            CHECK(provider->GetProviderName() == "Linux Provider");
            CHECK(provider->GetProviderId() == "linux_provider");

            auto* crossPlatformProvider = subsystem->GetCrossPlatformProvider();
            CHECK(crossPlatformProvider->GetPlatformName() == "Linux");
        }
    }

    TEST_CASE_FIXTURE(ProviderDelegationFixture, "Provider Call Count Validation") {
        SUBCASE("Verify delegation call counts") {
            SetupWindowsProvider();
            auto* provider = static_cast<WindowsProvider*>(subsystem->GetSubsystemProvider());

            // Reset counters
            provider->ResetCallCounts();
            subsystem->ResetCallCounts();

            // Full lifecycle
            subsystem->Initialize();
            subsystem->Start();
            subsystem->Stop();
            subsystem->Destroy();

            // Verify each operation was delegated exactly once
            CHECK(provider->GetInitializeCallCount() == 1);
            CHECK(provider->GetStartCallCount() == 1);
            CHECK(provider->GetStopCallCount() == 1);
            CHECK(provider->GetDestroyCallCount() == 1);

            CHECK(subsystem->GetInitializeCallCount() == 1);
            CHECK(subsystem->GetStartCallCount() == 1);
            CHECK(subsystem->GetStopCallCount() == 1);
            CHECK(subsystem->GetDestroyCallCount() == 1);
        }

        SUBCASE("Multiple operations on same provider") {
            SetupWindowsProvider();
            auto* provider = static_cast<WindowsProvider*>(subsystem->GetSubsystemProvider());

            // Multiple start/stop cycles
            subsystem->Initialize();

            subsystem->Start();
            subsystem->Stop();
            subsystem->Start();
            subsystem->Stop();

            CHECK(provider->GetInitializeCallCount() == 1);
            CHECK(provider->GetStartCallCount() == 2);
            CHECK(provider->GetStopCallCount() == 2);
        }
    }
}