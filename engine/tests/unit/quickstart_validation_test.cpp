#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <fstream>
#include <memory>
#include <sstream>

#include "AIEngine/core/EngineSystemDescriptor.hpp"
#include "AIEngine/core/EngineSystemManager.hpp"
#include "AIEngine/core/IEngineSubsystem.hpp"
#include "AIEngine/core/ISubsystemProvider.hpp"
#include "doctest.h"

namespace AIEngine {

// ============================================================================
// Implementation of the complete logging example from quickstart.md
// ============================================================================

class ILoggingProvider : public ISubsystemProvider {
   public:
    virtual ~ILoggingProvider() = default;
    virtual void WriteLog(const std::string& level, const std::string& message) = 0;
    virtual void FlushLogs() = 0;
};

class ILoggingSubsystem : public IEngineSubsystem {
   public:
    virtual ~ILoggingSubsystem() = default;
    virtual void LogInfo(const std::string& message) = 0;
    virtual void LogError(const std::string& message) = 0;

    ILoggingProvider* GetLoggingProvider() const {
        return static_cast<ILoggingProvider*>(GetSubsystemProvider());
    }
};

// File logging provider implementation from quickstart guide
class FileLoggingProvider : public ILoggingProvider {
   private:
    bool initialized = false;
    bool running = false;
    std::unique_ptr<std::stringstream> logStream;  // Use stringstream for testing instead of file

   public:
    bool Initialize() override {
        logStream = std::make_unique<std::stringstream>();
        initialized = true;
        return true;
    }

    void Start() override {
        if (initialized) running = true;
    }

    void Stop() override {
        FlushLogs();
        running = false;
    }

    void Destroy() override {
        if (logStream) {
            logStream.reset();
        }
        initialized = false;
        running = false;
    }

    bool IsInitialized() const override { return initialized; }
    bool IsRunning() const override { return running; }

    const std::string& GetProviderName() const override {
        static const std::string name = "File Logging Provider";
        return name;
    }

    const std::string& GetProviderId() const override {
        static const std::string id = "file-logging";
        return id;
    }

    void WriteLog(const std::string& level, const std::string& message) override {
        if (logStream && running) {
            *logStream << "[" << level << "] " << message << std::endl;
        }
    }

    void FlushLogs() override {
        // For testing, we don't need to flush the stringstream
    }

    // Test helper method
    std::string GetLogContents() const { return logStream ? logStream->str() : ""; }
};

// Logging subsystem implementation from quickstart guide
class LoggingSubsystem : public ILoggingSubsystem {
   private:
    std::unique_ptr<ILoggingProvider> provider;

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

    void SetSubsystemProvider(std::unique_ptr<ISubsystemProvider> p) override {
        provider = std::unique_ptr<ILoggingProvider>(static_cast<ILoggingProvider*>(p.release()));
    }

    ISubsystemProvider* GetSubsystemProvider() const override { return provider.get(); }

    bool HasProvider() const override { return provider != nullptr; }

    EngineSystemDescriptor GetDescriptor() const override {
        return EngineSystemDescriptor::Create<ILoggingSubsystem, LoggingSubsystem,
                                              FileLoggingProvider>("Logging");
    }

    const std::string& GetSubsystemName() const override {
        static const std::string name = "Logging Subsystem";
        return name;
    }

    void LogInfo(const std::string& message) override {
        if (auto* loggingProvider = GetLoggingProvider()) {
            loggingProvider->WriteLog("INFO", message);
        }
    }

    void LogError(const std::string& message) override {
        if (auto* loggingProvider = GetLoggingProvider()) {
            loggingProvider->WriteLog("ERROR", message);
        }
    }
};

// ============================================================================
// Simple graphics example for cross-platform provider pattern from quickstart.md
// ============================================================================

class IGraphicsProvider : public ISubsystemProvider {
   public:
    virtual ~IGraphicsProvider() = default;
    virtual void InitializeAPI() = 0;
    virtual void ShutdownAPI() = 0;
    virtual bool CreateContext() = 0;
    virtual std::string GetRendererInfo() const = 0;
    virtual std::string GetPlatformName() const = 0;
};

class IGraphicsSubsystem : public IEngineSubsystem {
   public:
    virtual ~IGraphicsSubsystem() = default;
    virtual void Render() = 0;
    virtual void SetViewport(int width, int height) = 0;

    IGraphicsProvider* GetGraphicsProvider() const {
        return static_cast<IGraphicsProvider*>(GetSubsystemProvider());
    }
};

// Mock OpenGL provider from quickstart example
class OpenGLGraphicsProvider : public IGraphicsProvider {
   private:
    bool initialized = false;
    bool running = false;
    bool contextCreated = false;

   public:
    bool Initialize() override {
        InitializeAPI();
        contextCreated = CreateContext();
        initialized = contextCreated;
        return initialized;
    }

    void Start() override {
        if (initialized) running = true;
    }

    void Stop() override { running = false; }

    void Destroy() override {
        if (initialized) {
            ShutdownAPI();
            contextCreated = false;
            initialized = false;
        }
    }

    bool IsInitialized() const override { return initialized; }
    bool IsRunning() const override { return running; }

    const std::string& GetProviderName() const override {
        static const std::string name = "OpenGL Graphics Provider";
        return name;
    }

    const std::string& GetProviderId() const override {
        static const std::string id = "opengl-graphics";
        return id;
    }

    void InitializeAPI() override {
        // Mock OpenGL initialization
    }

    void ShutdownAPI() override {
        // Mock OpenGL shutdown
    }

    bool CreateContext() override {
        // Mock OpenGL context creation
        return true;
    }

    std::string GetRendererInfo() const override { return "Mock OpenGL 4.6"; }

    std::string GetPlatformName() const override { return "OpenGL"; }
};

// Graphics subsystem implementation
class GraphicsSubsystem : public IGraphicsSubsystem {
   private:
    std::unique_ptr<IGraphicsProvider> provider;
    int viewportWidth = 0;
    int viewportHeight = 0;

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

    void SetSubsystemProvider(std::unique_ptr<ISubsystemProvider> p) override {
        provider = std::unique_ptr<IGraphicsProvider>(static_cast<IGraphicsProvider*>(p.release()));
    }

    ISubsystemProvider* GetSubsystemProvider() const override { return provider.get(); }

    bool HasProvider() const override { return provider != nullptr; }

    EngineSystemDescriptor GetDescriptor() const override {
        return EngineSystemDescriptor::Create<IGraphicsSubsystem, GraphicsSubsystem,
                                              OpenGLGraphicsProvider>("Graphics");
    }

    const std::string& GetSubsystemName() const override {
        static const std::string name = "Graphics Subsystem";
        return name;
    }

    void Render() override {
        // Mock rendering
    }

    void SetViewport(int width, int height) override {
        viewportWidth = width;
        viewportHeight = height;
    }

    // Test helpers
    int GetViewportWidth() const { return viewportWidth; }
    int GetViewportHeight() const { return viewportHeight; }
};

}  // namespace AIEngine

// ============================================================================
// QuickStart Guide Validation Tests
// ============================================================================

TEST_CASE("QuickStart Guide - Complete Logging Example Validation") {
    auto& manager = AIEngine::EngineSystemManager::GetInstance();
    manager.DestroyAllSystems();

    SUBCASE("Step-by-step logging example from quickstart guide") {
        // Step 1: Create descriptor (from quickstart guide)
        auto loggingDescriptor = AIEngine::EngineSystemDescriptor::Create<
            AIEngine::ILoggingSubsystem, AIEngine::LoggingSubsystem, AIEngine::FileLoggingProvider>(
            "Logging");

        // Step 2: Register descriptor
        manager.RegisterSystemDescriptor(loggingDescriptor);

        // Step 3: Create subsystem instance
        auto* logging = manager.CreateSystem<AIEngine::ILoggingSubsystem>(loggingDescriptor);
        REQUIRE(logging != nullptr);
        CHECK(logging->HasProvider());

        // Step 4: Verify lifecycle works as documented
        CHECK(logging->Initialize());
        CHECK(logging->IsInitialized());

        logging->Start();
        CHECK(logging->IsRunning());

        // Step 5: Test logging functionality
        logging->LogInfo("Game started successfully!");
        logging->LogError("Example error message");

        // Verify logs were written
        auto* provider =
            static_cast<AIEngine::FileLoggingProvider*>(logging->GetSubsystemProvider());
        std::string logContents = provider->GetLogContents();
        CHECK(logContents.find("[INFO] Game started successfully!") != std::string::npos);
        CHECK(logContents.find("[ERROR] Example error message") != std::string::npos);

        // Step 6: Test convenience macro access
        auto* sameLogging = GetEngineSubsystem(AIEngine::ILoggingSubsystem);
        CHECK(sameLogging == logging);

        // Step 7: Test shutdown
        logging->Stop();
        CHECK_FALSE(logging->IsRunning());
        CHECK(logging->IsInitialized());

        logging->Destroy();
        CHECK_FALSE(logging->IsInitialized());
        CHECK_FALSE(logging->IsRunning());
    }
}

TEST_CASE("QuickStart Guide - Cross-Platform Provider Pattern Validation") {
    auto& manager = AIEngine::EngineSystemManager::GetInstance();
    manager.DestroyAllSystems();

    SUBCASE("Graphics subsystem with platform provider from quickstart guide") {
        // Create descriptor using cross-platform pattern
        auto graphicsDescriptor =
            AIEngine::EngineSystemDescriptor::Create<AIEngine::IGraphicsSubsystem,
                                                     AIEngine::GraphicsSubsystem,
                                                     AIEngine::OpenGLGraphicsProvider>("Graphics");

        // Register and create
        manager.RegisterSystemDescriptor(graphicsDescriptor);
        auto* graphics = manager.CreateSystem<AIEngine::IGraphicsSubsystem>(graphicsDescriptor);

        REQUIRE(graphics != nullptr);
        CHECK(graphics->HasProvider());

        // Test platform-specific provider functionality
        auto* provider = graphics->GetGraphicsProvider();
        CHECK(provider->GetPlatformName() == "OpenGL");
        CHECK(provider->GetRendererInfo() == "Mock OpenGL 4.6");

        // Test subsystem functionality
        CHECK(graphics->Initialize());
        graphics->Start();

        graphics->SetViewport(1920, 1080);
        CHECK(static_cast<AIEngine::GraphicsSubsystem*>(graphics)->GetViewportWidth() == 1920);
        CHECK(static_cast<AIEngine::GraphicsSubsystem*>(graphics)->GetViewportHeight() == 1080);

        graphics->Render();  // Should not crash

        // Cleanup
        graphics->Stop();
        graphics->Destroy();
    }
}

TEST_CASE("QuickStart Guide - Registration and Usage Pattern Validation") {
    auto& manager = AIEngine::EngineSystemManager::GetInstance();
    manager.DestroyAllSystems();

    SUBCASE("Complete registration and usage workflow from quickstart") {
        // Simulate the RegisterEngineSubsystems() function from guide
        auto loggingDescriptor = AIEngine::EngineSystemDescriptor::Create<
            AIEngine::ILoggingSubsystem, AIEngine::LoggingSubsystem, AIEngine::FileLoggingProvider>(
            "Logging");

        auto graphicsDescriptor =
            AIEngine::EngineSystemDescriptor::Create<AIEngine::IGraphicsSubsystem,
                                                     AIEngine::GraphicsSubsystem,
                                                     AIEngine::OpenGLGraphicsProvider>("Graphics");

        // Register subsystems
        manager.RegisterSystemDescriptor(loggingDescriptor);
        manager.RegisterSystemDescriptor(graphicsDescriptor);

        // Create subsystem instances
        manager.CreateSystem<AIEngine::ILoggingSubsystem>(loggingDescriptor);
        manager.CreateSystem<AIEngine::IGraphicsSubsystem>(graphicsDescriptor);

        // Test manager-level lifecycle (simulating Engine::Initialize())
        CHECK(manager.InitializeAllSystems());
        manager.StartAllSystems();

        // Test usage pattern from guide
        auto* logging = GetEngineSubsystem(AIEngine::ILoggingSubsystem);
        auto* graphics = GetEngineSubsystem(AIEngine::IGraphicsSubsystem);

        REQUIRE(logging != nullptr);
        REQUIRE(graphics != nullptr);

        CHECK(logging->IsRunning());
        CHECK(graphics->IsRunning());

        // Use subsystems as documented
        logging->LogInfo("Subsystems initialized successfully!");
        graphics->SetViewport(800, 600);
        graphics->Render();

        // Verify logging worked
        auto* provider =
            static_cast<AIEngine::FileLoggingProvider*>(logging->GetSubsystemProvider());
        std::string logs = provider->GetLogContents();
        CHECK(logs.find("[INFO] Subsystems initialized successfully!") != std::string::npos);

        // Test shutdown workflow (simulating Engine::Shutdown())
        manager.StopAllSystems();
        CHECK_FALSE(logging->IsRunning());
        CHECK_FALSE(graphics->IsRunning());

        manager.DestroyAllSystems();
        CHECK_FALSE(logging->IsInitialized());
        CHECK_FALSE(graphics->IsInitialized());
    }
}

TEST_CASE("QuickStart Guide - Error Handling Patterns Validation") {
    auto& manager = AIEngine::EngineSystemManager::GetInstance();
    manager.DestroyAllSystems();

    SUBCASE("Null pointer checking pattern from troubleshooting section") {
        // Test the recommended null check pattern
        auto* nonExistentSubsystem = GetEngineSubsystem(AIEngine::ILoggingSubsystem);
        CHECK(nonExistentSubsystem == nullptr);

        // Register and create subsystem
        auto descriptor = AIEngine::EngineSystemDescriptor::Create<
            AIEngine::ILoggingSubsystem, AIEngine::LoggingSubsystem, AIEngine::FileLoggingProvider>(
            "Logging");

        manager.RegisterSystemDescriptor(descriptor);
        manager.CreateSystem<AIEngine::ILoggingSubsystem>(descriptor);

        // Verify the safe access pattern works
        auto* logging = GetEngineSubsystem(AIEngine::ILoggingSubsystem);
        if (logging) {
            CHECK(logging->Initialize());
            logging->LogInfo("Safe access pattern works!");

            auto* provider =
                static_cast<AIEngine::FileLoggingProvider*>(logging->GetSubsystemProvider());
            std::string logs = provider->GetLogContents();
            CHECK(logs.find("Safe access pattern works!") != std::string::npos);
        } else {
            FAIL("Subsystem should be available after registration and creation");
        }
    }
}

TEST_CASE("QuickStart Guide - API Contract Validation") {
    SUBCASE("Verify all APIs mentioned in quickstart are available") {
        // Test that all classes and functions mentioned in the guide exist and work

        // 1. Required headers are accessible (compilation test)
        // Already verified by successful compilation

        // 2. Core interfaces exist and are usable
        static_assert(std::is_base_of_v<AIEngine::IEngineSubsystem, AIEngine::LoggingSubsystem>);
        static_assert(
            std::is_base_of_v<AIEngine::ISubsystemProvider, AIEngine::FileLoggingProvider>);

        // 3. EngineSystemDescriptor::Create works as documented
        auto descriptor = AIEngine::EngineSystemDescriptor::Create<
            AIEngine::ILoggingSubsystem, AIEngine::LoggingSubsystem, AIEngine::FileLoggingProvider>(
            "Test");

        CHECK(descriptor.GetSystemName() == "Test");
        CHECK(descriptor.GetInterfaceTypeName().find("ILoggingSubsystem") != std::string::npos);

        // 4. EngineSystemManager singleton access works
        auto& manager1 = AIEngine::EngineSystemManager::GetInstance();
        auto& manager2 = AIEngine::EngineSystemManager::GetInstance();
        CHECK(&manager1 == &manager2);

        // 5. GetEngineSubsystem macro is available
        manager1.DestroyAllSystems();
        manager1.RegisterSystemDescriptor(descriptor);
        manager1.CreateSystem<AIEngine::ILoggingSubsystem>(descriptor);

        auto* subsystem = GetEngineSubsystem(AIEngine::ILoggingSubsystem);
        CHECK(subsystem != nullptr);
    }
}