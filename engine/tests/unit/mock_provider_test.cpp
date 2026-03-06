/**
 * mock_provider_test.cpp - Mock Provider Implementation Unit Tests
 *
 * Comprehensive testing of mock provider implementations for robust testing
 * infrastructure and validation of provider contract compliance.
 *
 * Tests User Story 3: Mock provider implementations for testing scenarios
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
#include <string>
#include <thread>
#include <vector>

#include "doctest.h"

namespace AIEngine {

// Configurable mock provider with detailed behavior control
class ConfigurableMockProvider : public ISubsystemProvider {
   public:
    struct Config {
        bool failOnInitialize = false;
        bool failOnStart = false;
        bool throwOnStop = false;
        bool throwOnDestroy = false;
        int initializeDelayMs = 0;
        int startDelayMs = 0;
        std::string providerName = "Configurable Mock Provider";
        std::string providerId = "configurable_mock";
        bool logOperations = false;
    };

    explicit ConfigurableMockProvider(const Config& config = Config{}) : config(config) {}

    bool Initialize() override {
        if (config.logOperations) {
            operationLog.push_back("Initialize called");
        }

        if (config.initializeDelayMs > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(config.initializeDelayMs));
        }

        if (config.failOnInitialize) {
            failureReason = "Configured to fail on initialize";
            return false;
        }

        initialized = true;
        initializeCallCount++;

        if (config.logOperations) {
            operationLog.push_back("Initialize completed successfully");
        }
        return true;
    }

    void Start() override {
        if (config.logOperations) {
            operationLog.push_back("Start called");
        }

        if (!initialized) {
            throw std::runtime_error("Cannot start uninitialized mock provider");
        }

        if (config.startDelayMs > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(config.startDelayMs));
        }

        if (config.failOnStart) {
            throw std::runtime_error("Configured to fail on start");
        }

        running = true;
        startCallCount++;

        if (config.logOperations) {
            operationLog.push_back("Start completed successfully");
        }
    }

    void Stop() override {
        if (config.logOperations) {
            operationLog.push_back("Stop called");
        }

        if (config.throwOnStop) {
            throw std::runtime_error("Configured to throw on stop");
        }

        if (running) {
            running = false;
            stopCallCount++;
        }

        if (config.logOperations) {
            operationLog.push_back("Stop completed");
        }
    }

    void Destroy() override {
        if (config.logOperations) {
            operationLog.push_back("Destroy called");
        }

        if (config.throwOnDestroy) {
            throw std::runtime_error("Configured to throw on destroy");
        }

        if (running) {
            Stop();
        }

        initialized = false;
        destroyCallCount++;

        if (config.logOperations) {
            operationLog.push_back("Destroy completed");
        }
    }

    bool IsInitialized() const override { return initialized; }
    bool IsRunning() const override { return running; }

    const std::string& GetProviderName() const override { return config.providerName; }
    const std::string& GetProviderId() const override { return config.providerId; }

    // Test accessors and controls
    int GetInitializeCallCount() const { return initializeCallCount; }
    int GetStartCallCount() const { return startCallCount; }
    int GetStopCallCount() const { return stopCallCount; }
    int GetDestroyCallCount() const { return destroyCallCount; }

    const std::string& GetFailureReason() const { return failureReason; }
    const std::vector<std::string>& GetOperationLog() const { return operationLog; }

    void ResetCallCounts() {
        initializeCallCount = startCallCount = stopCallCount = destroyCallCount = 0;
        failureReason.clear();
        operationLog.clear();
    }

    void UpdateConfig(const Config& newConfig) { config = newConfig; }

   private:
    Config config;
    bool initialized = false;
    bool running = false;
    int initializeCallCount = 0;
    int startCallCount = 0;
    int stopCallCount = 0;
    int destroyCallCount = 0;
    std::string failureReason;
    std::vector<std::string> operationLog;
};

// State tracking mock provider for detailed state validation
class StateTrackingMockProvider : public ISubsystemProvider {
   public:
    enum class State {
        Created,
        Initializing,
        Initialized,
        Starting,
        Running,
        Stopping,
        Stopped,
        Destroying,
        Destroyed
    };

    StateTrackingMockProvider() = default;

    bool Initialize() override {
        if (state != State::Created && state != State::Stopped) {
            throw std::runtime_error("Invalid state for initialize: " + StateToString(state));
        }

        state = State::Initializing;
        stateHistory.push_back(state);

        // Simulate initialization work
        initializeCallCount++;

        state = State::Initialized;
        stateHistory.push_back(state);
        return true;
    }

    void Start() override {
        if (state != State::Initialized) {
            throw std::runtime_error("Invalid state for start: " + StateToString(state));
        }

        state = State::Starting;
        stateHistory.push_back(state);

        startCallCount++;

        state = State::Running;
        stateHistory.push_back(state);
    }

    void Stop() override {
        if (state != State::Running) {
            return;  // Allow stop to be called multiple times
        }

        state = State::Stopping;
        stateHistory.push_back(state);

        stopCallCount++;

        state = State::Stopped;
        stateHistory.push_back(state);
    }

    void Destroy() override {
        if (state == State::Running) {
            Stop();
        }

        state = State::Destroying;
        stateHistory.push_back(state);

        destroyCallCount++;

        state = State::Destroyed;
        stateHistory.push_back(state);
    }

    bool IsInitialized() const override {
        return state == State::Initialized || state == State::Starting || state == State::Running ||
               state == State::Stopping || state == State::Stopped;
    }

    bool IsRunning() const override { return state == State::Running; }

    const std::string& GetProviderName() const override {
        static const std::string name = "State Tracking Mock Provider";
        return name;
    }

    const std::string& GetProviderId() const override {
        static const std::string id = "state_tracking_mock";
        return id;
    }

    // State tracking accessors
    State GetCurrentState() const { return state; }
    const std::vector<State>& GetStateHistory() const { return stateHistory; }

    std::string StateToString(State s) const {
        switch (s) {
            case State::Created:
                return "Created";
            case State::Initializing:
                return "Initializing";
            case State::Initialized:
                return "Initialized";
            case State::Starting:
                return "Starting";
            case State::Running:
                return "Running";
            case State::Stopping:
                return "Stopping";
            case State::Stopped:
                return "Stopped";
            case State::Destroying:
                return "Destroying";
            case State::Destroyed:
                return "Destroyed";
            default:
                return "Unknown";
        }
    }

    bool HasTransitionedThroughStates(const std::vector<State>& expectedStates) const {
        if (expectedStates.size() > stateHistory.size()) return false;

        size_t historyIndex = 0;
        for (State expectedState : expectedStates) {
            bool found = false;
            while (historyIndex < stateHistory.size()) {
                if (stateHistory[historyIndex] == expectedState) {
                    found = true;
                    historyIndex++;
                    break;
                }
                historyIndex++;
            }
            if (!found) return false;
        }
        return true;
    }

    int GetInitializeCallCount() const { return initializeCallCount; }
    int GetStartCallCount() const { return startCallCount; }
    int GetStopCallCount() const { return stopCallCount; }
    int GetDestroyCallCount() const { return destroyCallCount; }

   private:
    State state = State::Created;
    std::vector<State> stateHistory = {State::Created};
    int initializeCallCount = 0;
    int startCallCount = 0;
    int stopCallCount = 0;
    int destroyCallCount = 0;
};

// Performance testing mock provider
class PerformanceMockProvider : public ISubsystemProvider {
   public:
    PerformanceMockProvider() = default;

    bool Initialize() override {
        auto start = std::chrono::high_resolution_clock::now();

        // Simulate some initialization work
        volatile int sum = 0;
        for (int i = 0; i < 1000; ++i) {
            sum += i;
        }

        auto end = std::chrono::high_resolution_clock::now();
        lastInitializeDuration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

        initialized = true;
        return true;
    }

    void Start() override {
        auto start = std::chrono::high_resolution_clock::now();

        if (!initialized) {
            throw std::runtime_error("Cannot start uninitialized performance provider");
        }

        running = true;

        auto end = std::chrono::high_resolution_clock::now();
        lastStartDuration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    }

    void Stop() override {
        auto start = std::chrono::high_resolution_clock::now();

        running = false;

        auto end = std::chrono::high_resolution_clock::now();
        lastStopDuration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    }

    void Destroy() override {
        auto start = std::chrono::high_resolution_clock::now();

        if (running) Stop();
        initialized = false;

        auto end = std::chrono::high_resolution_clock::now();
        lastDestroyDuration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    }

    bool IsInitialized() const override { return initialized; }
    bool IsRunning() const override { return running; }

    const std::string& GetProviderName() const override {
        static const std::string name = "Performance Mock Provider";
        return name;
    }

    const std::string& GetProviderId() const override {
        static const std::string id = "performance_mock";
        return id;
    }

    // Performance metrics
    std::chrono::microseconds GetLastInitializeDuration() const { return lastInitializeDuration; }
    std::chrono::microseconds GetLastStartDuration() const { return lastStartDuration; }
    std::chrono::microseconds GetLastStopDuration() const { return lastStopDuration; }
    std::chrono::microseconds GetLastDestroyDuration() const { return lastDestroyDuration; }

   private:
    bool initialized = false;
    bool running = false;
    std::chrono::microseconds lastInitializeDuration{0};
    std::chrono::microseconds lastStartDuration{0};
    std::chrono::microseconds lastStopDuration{0};
    std::chrono::microseconds lastDestroyDuration{0};
};

// Test subsystem that uses mock providers
class MockTestSubsystem : public IEngineSubsystem {
   public:
    MockTestSubsystem() = default;

    bool Initialize() override { return provider && provider->Initialize(); }

    void Start() override {
        if (provider && provider->IsInitialized()) {
            provider->Start();
        } else {
            throw std::runtime_error("Cannot start: provider not ready");
        }
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
        return EngineSystemDescriptor::Create<IEngineSubsystem, MockTestSubsystem,
                                              ConfigurableMockProvider>("MockTest");
    }

    const std::string& GetSubsystemName() const override {
        static const std::string name = "Mock Test Subsystem";
        return name;
    }

   private:
    std::unique_ptr<ISubsystemProvider> provider;
};

}  // namespace AIEngine

using namespace AIEngine;

TEST_SUITE("Mock Provider Implementation Tests") {
    TEST_CASE("ConfigurableMockProvider Basic Functionality") {
        SUBCASE("Default configuration successful lifecycle") {
            ConfigurableMockProvider provider;

            // Initial state
            CHECK_FALSE(provider.IsInitialized());
            CHECK_FALSE(provider.IsRunning());
            CHECK(provider.GetInitializeCallCount() == 0);

            // Initialize
            CHECK(provider.Initialize());
            CHECK(provider.IsInitialized());
            CHECK_FALSE(provider.IsRunning());
            CHECK(provider.GetInitializeCallCount() == 1);

            // Start
            provider.Start();
            CHECK(provider.IsRunning());
            CHECK(provider.GetStartCallCount() == 1);

            // Stop
            provider.Stop();
            CHECK_FALSE(provider.IsRunning());
            CHECK(provider.IsInitialized());
            CHECK(provider.GetStopCallCount() == 1);

            // Destroy
            provider.Destroy();
            CHECK_FALSE(provider.IsInitialized());
            CHECK(provider.GetDestroyCallCount() == 1);
        }

        SUBCASE("Configured initialization failure") {
            ConfigurableMockProvider::Config config;
            config.failOnInitialize = true;
            ConfigurableMockProvider provider(config);

            CHECK_FALSE(provider.Initialize());
            CHECK_FALSE(provider.IsInitialized());
            CHECK_FALSE(provider.GetFailureReason().empty());
            CHECK(provider.GetFailureReason() == "Configured to fail on initialize");
        }

        SUBCASE("Configured start failure") {
            ConfigurableMockProvider::Config config;
            config.failOnStart = true;
            ConfigurableMockProvider provider(config);

            // Initialize successfully
            CHECK(provider.Initialize());
            CHECK(provider.IsInitialized());

            // Start should throw
            CHECK_THROWS_AS(provider.Start(), std::runtime_error);
            CHECK_FALSE(provider.IsRunning());
        }

        SUBCASE("Operation logging") {
            ConfigurableMockProvider::Config config;
            config.logOperations = true;
            ConfigurableMockProvider provider(config);

            provider.Initialize();
            provider.Start();
            provider.Stop();
            provider.Destroy();

            auto& log = provider.GetOperationLog();
            CHECK(log.size() >= 6);  // Each operation logs at least entry and completion
            CHECK(log[0] == "Initialize called");
            CHECK(log[1] == "Initialize completed successfully");
        }

        SUBCASE("Custom provider metadata") {
            ConfigurableMockProvider::Config config;
            config.providerName = "Custom Test Provider";
            config.providerId = "custom_test_123";
            ConfigurableMockProvider provider(config);

            CHECK(provider.GetProviderName() == "Custom Test Provider");
            CHECK(provider.GetProviderId() == "custom_test_123");
        }

        SUBCASE("Runtime configuration update") {
            ConfigurableMockProvider provider;

            // Initially should work
            CHECK(provider.Initialize());

            // Update config to fail on start
            ConfigurableMockProvider::Config config;
            config.failOnStart = true;
            provider.UpdateConfig(config);

            // Start should now fail
            CHECK_THROWS_AS(provider.Start(), std::runtime_error);
        }
    }

    TEST_CASE("StateTrackingMockProvider State Management") {
        SUBCASE("State transition validation") {
            StateTrackingMockProvider provider;

            // Initial state
            CHECK(provider.GetCurrentState() == StateTrackingMockProvider::State::Created);

            // Initialize
            provider.Initialize();
            CHECK(provider.GetCurrentState() == StateTrackingMockProvider::State::Initialized);
            CHECK(provider.IsInitialized());
            CHECK_FALSE(provider.IsRunning());

            // Start
            provider.Start();
            CHECK(provider.GetCurrentState() == StateTrackingMockProvider::State::Running);
            CHECK(provider.IsRunning());

            // Stop
            provider.Stop();
            CHECK(provider.GetCurrentState() == StateTrackingMockProvider::State::Stopped);
            CHECK_FALSE(provider.IsRunning());
            CHECK(provider.IsInitialized());

            // Destroy
            provider.Destroy();
            CHECK(provider.GetCurrentState() == StateTrackingMockProvider::State::Destroyed);
            CHECK_FALSE(provider.IsInitialized());
        }

        SUBCASE("State history tracking") {
            StateTrackingMockProvider provider;

            // Full lifecycle
            provider.Initialize();
            provider.Start();
            provider.Stop();
            provider.Destroy();

            auto expectedStates = {StateTrackingMockProvider::State::Created,
                                   StateTrackingMockProvider::State::Initializing,
                                   StateTrackingMockProvider::State::Initialized,
                                   StateTrackingMockProvider::State::Starting,
                                   StateTrackingMockProvider::State::Running,
                                   StateTrackingMockProvider::State::Stopping,
                                   StateTrackingMockProvider::State::Stopped,
                                   StateTrackingMockProvider::State::Destroying,
                                   StateTrackingMockProvider::State::Destroyed};

            CHECK(provider.HasTransitionedThroughStates(
                std::vector<StateTrackingMockProvider::State>(expectedStates)));
        }

        SUBCASE("Invalid state transition handling") {
            StateTrackingMockProvider provider;

            // Try to start without initializing
            CHECK_THROWS_AS(provider.Start(), std::runtime_error);
            CHECK(provider.GetCurrentState() == StateTrackingMockProvider::State::Created);
        }

        SUBCASE("Multiple stop calls allowed") {
            StateTrackingMockProvider provider;

            provider.Initialize();
            provider.Start();
            provider.Stop();

            // Multiple stop calls should not throw
            provider.Stop();
            provider.Stop();

            CHECK(provider.GetCurrentState() == StateTrackingMockProvider::State::Stopped);
            CHECK(provider.GetStopCallCount() == 1);  // Only first stop actually does work
        }
    }

    TEST_CASE("PerformanceMockProvider Performance Metrics") {
        SUBCASE("Operation timing measurement") {
            PerformanceMockProvider provider;

            // Measure initialize time
            provider.Initialize();
            auto initTime = provider.GetLastInitializeDuration();
            CHECK(initTime.count() > 0);  // Should take some measurable time

            // Measure start time
            provider.Start();
            auto startTime = provider.GetLastStartDuration();
            CHECK(startTime.count() >= 0);

            // Measure stop time
            provider.Stop();
            auto stopTime = provider.GetLastStopDuration();
            CHECK(stopTime.count() >= 0);

            // Measure destroy time
            provider.Destroy();
            auto destroyTime = provider.GetLastDestroyDuration();
            CHECK(destroyTime.count() >= 0);
        }

        SUBCASE("Performance consistency") {
            PerformanceMockProvider provider;

            // Multiple initialize/destroy cycles
            std::vector<std::chrono::microseconds> initTimes;

            for (int i = 0; i < 3; ++i) {
                provider.Initialize();
                initTimes.push_back(provider.GetLastInitializeDuration());
                provider.Destroy();
            }

            // All times should be reasonable (not zero and not excessive)
            for (auto time : initTimes) {
                CHECK(time.count() > 0);
                CHECK(time.count() < 10000);  // Less than 10ms
            }
        }
    }

    TEST_CASE("Mock Provider Integration with Subsystem") {
        SUBCASE("Configurable provider with subsystem") {
            MockTestSubsystem subsystem;

            ConfigurableMockProvider::Config config;
            config.logOperations = true;
            auto provider = std::make_unique<ConfigurableMockProvider>(config);

            subsystem.SetSubsystemProvider(std::move(provider));

            // Full lifecycle through subsystem
            CHECK(subsystem.Initialize());
            subsystem.Start();
            subsystem.Stop();
            subsystem.Destroy();

            // Verify operations were logged
            auto* mockProvider =
                static_cast<ConfigurableMockProvider*>(subsystem.GetSubsystemProvider());
            auto& log = mockProvider->GetOperationLog();
            CHECK(log.size() > 0);
        }

        SUBCASE("State tracking provider with subsystem") {
            MockTestSubsystem subsystem;
            auto provider = std::make_unique<StateTrackingMockProvider>();
            auto* providerPtr = provider.get();

            subsystem.SetSubsystemProvider(std::move(provider));

            // Initialize through subsystem
            subsystem.Initialize();
            CHECK(providerPtr->GetCurrentState() == StateTrackingMockProvider::State::Initialized);

            // Start through subsystem
            subsystem.Start();
            CHECK(providerPtr->GetCurrentState() == StateTrackingMockProvider::State::Running);

            // Verify state history
            CHECK(providerPtr->HasTransitionedThroughStates(
                {StateTrackingMockProvider::State::Initializing,
                 StateTrackingMockProvider::State::Initialized,
                 StateTrackingMockProvider::State::Starting,
                 StateTrackingMockProvider::State::Running}));
        }
    }

    TEST_CASE("Mock Provider Error Scenarios") {
        SUBCASE("Provider failure propagation") {
            MockTestSubsystem subsystem;

            ConfigurableMockProvider::Config config;
            config.failOnInitialize = true;
            auto provider = std::make_unique<ConfigurableMockProvider>(config);

            subsystem.SetSubsystemProvider(std::move(provider));

            // Subsystem initialization should fail due to provider failure
            CHECK_FALSE(subsystem.Initialize());
            CHECK_FALSE(subsystem.IsInitialized());
        }

        SUBCASE("Exception handling in mock provider") {
            MockTestSubsystem subsystem;

            ConfigurableMockProvider::Config config;
            config.throwOnStop = true;
            auto provider = std::make_unique<ConfigurableMockProvider>(config);

            subsystem.SetSubsystemProvider(std::move(provider));

            // Initialize and start successfully
            CHECK(subsystem.Initialize());
            subsystem.Start();

            // Stop should throw
            CHECK_THROWS_AS(subsystem.Stop(), std::runtime_error);
        }
    }

    TEST_CASE("Mock Provider Factory and Creation") {
        SUBCASE("Create provider with specific configuration") {
            auto createFailureProvider = []() {
                ConfigurableMockProvider::Config config;
                config.failOnInitialize = true;
                config.providerName = "Failure Test Provider";
                return std::make_unique<ConfigurableMockProvider>(config);
            };

            auto provider = createFailureProvider();
            CHECK(provider->GetProviderName() == "Failure Test Provider");
            CHECK_FALSE(provider->Initialize());
        }

        SUBCASE("Provider type polymorphism") {
            std::vector<std::unique_ptr<ISubsystemProvider>> providers;

            providers.push_back(std::make_unique<ConfigurableMockProvider>());
            providers.push_back(std::make_unique<StateTrackingMockProvider>());
            providers.push_back(std::make_unique<PerformanceMockProvider>());

            // All should be valid ISubsystemProvider instances
            for (auto& provider : providers) {
                CHECK(provider != nullptr);
                CHECK_FALSE(provider->GetProviderName().empty());
                CHECK_FALSE(provider->GetProviderId().empty());
            }
        }
    }
}