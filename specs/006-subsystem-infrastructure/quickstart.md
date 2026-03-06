# QuickStart Guide: Engine Subsystem Infrastructure

**Purpose**: Get started using the core engine subsystem infrastructure  
**Audience**: Engine developers implementing new subsystems  
**Prerequisites**: Basic C++17 knowledge, familiarity with AIEngine architecture  

## Overview

The Engine Subsystem Infrastructure provides a standardized way to add new capabilities to the AIEngine without modifying core engine code. It uses a provider pattern for platform abstraction and automatic lifecycle management through the Engine class.

## 5-Minute Quick Start

### 1. Include Required Headers

```cpp
#include \"AIEngine/core/IEngineSubsystem.hpp\"
#include \"AIEngine/core/ISubsystemProvider.hpp\"
#include \"AIEngine/core/EngineSystemDescriptor.hpp\"
#include \"AIEngine/core/EngineSystemManager.hpp\"
```

### 2. Define Your Subsystem Interface

```cpp
namespace AIEngine {
    
    // Your subsystem interface
    class ILoggingSubsystem : public IEngineSubsystem {
    public:
        virtual ~ILoggingSubsystem() = default;
        
        // Your subsystem's specific methods
        virtual void LogInfo(const std::string& message) = 0;
        virtual void LogError(const std::string& message) = 0;
        virtual void SetLogLevel(int level) = 0;
        
        // Type-safe provider access
        ILoggingProvider* GetLoggingProvider() const {
            return static_cast<ILoggingProvider*>(GetSubsystemProvider());
        }
    };
    
    // Your provider interface  
    class ILoggingProvider : public ISubsystemProvider {
    public:
        virtual ~ILoggingProvider() = default;
        
        // Platform-specific logging methods
        virtual void WriteLog(const std::string& level, const std::string& message) = 0;
        virtual void FlushLogs() = 0;
    };
}
```

### 3. Implement Your Subsystem

```cpp
namespace AIEngine {
    
    class LoggingSubsystem : public ILoggingSubsystem {
    public:
        // IEngineSubsystem implementation (delegates to provider)
        bool Initialize() override {
            return GetLoggingProvider() && GetLoggingProvider()->Initialize();
        }
        
        void Start() override { GetLoggingProvider()->Start(); }
        void Stop() override { GetLoggingProvider()->Stop(); }
        void Destroy() override { GetLoggingProvider()->Destroy(); }
        
        bool IsInitialized() const override {
            return GetLoggingProvider() && GetLoggingProvider()->IsInitialized();
        }
        
        bool IsRunning() const override {
            return GetLoggingProvider() && GetLoggingProvider()->IsRunning();
        }
        
        // Provider management
        void SetSubsystemProvider(std::unique_ptr<ISubsystemProvider> provider) override {
            loggingProvider = std::unique_ptr<ILoggingProvider>(
                static_cast<ILoggingProvider*>(provider.release()));
        }
        
        ISubsystemProvider* GetSubsystemProvider() const override {
            return loggingProvider.get();
        }
        
        bool HasProvider() const override {
            return loggingProvider != nullptr;
        }
        
        // Subsystem metadata
        EngineSystemDescriptor GetDescriptor() const override {
            return EngineSystemDescriptor::Create<ILoggingSubsystem, LoggingSubsystem, FileLoggingProvider>(\"Logging\");
        }
        
        const std::string& GetSubsystemName() const override {
            static const std::string name = \"Logging Subsystem\";
            return name;
        }
        
        // Your subsystem's specific methods
        void LogInfo(const std::string& message) override {
            GetLoggingProvider()->WriteLog(\"INFO\", message);
        }
        
        void LogError(const std::string& message) override {
            GetLoggingProvider()->WriteLog(\"ERROR\", message);
        }
        
        void SetLogLevel(int level) override {
            // Implementation that might configure provider
        }
        
    private:
        std::unique_ptr<ILoggingProvider> loggingProvider;
    };
}
```

### 4. Implement Your Provider

```cpp
namespace AIEngine {
    
    class FileLoggingProvider : public ILoggingProvider {
    public:
        FileLoggingProvider() : initialized(false), running(false) {}
        
        // ISubsystemProvider implementation
        bool Initialize() override {
            // Open log file, setup directories, etc.
            logFile = std::make_unique<std::ofstream>(\"engine.log\");
            initialized = logFile->is_open();
            return initialized;
        }
        
        void Start() override {
            running = true;
        }
        
        void Stop() override {
            FlushLogs();
            running = false;
        }
        
        void Destroy() override {
            if (logFile) {
                logFile->close();
                logFile.reset();
            }
            initialized = false;
        }
        
        bool IsInitialized() const override { return initialized; }
        bool IsRunning() const override { return running; }
        
        const std::string& GetProviderName() const override {
            static const std::string name = \"File Logging Provider\";
            return name;
        }
        
        const std::string& GetProviderId() const override {
            static const std::string id = \"file-logging\";
            return id;
        }
        
        // Your provider's specific methods
        void WriteLog(const std::string& level, const std::string& message) override {
            if (logFile && running) {
                *logFile << \"[\" << level << \"] \" << message << std::endl;
            }
        }
        
        void FlushLogs() override {
            if (logFile) {
                logFile->flush();
            }
        }
        
    private:
        bool initialized;
        bool running;
        std::unique_ptr<std::ofstream> logFile;
    };
}
```

### 5. Register Your Subsystem

```cpp
// In your engine initialization code (before Engine::Initialize())
void RegisterEngineSubsystems() {
    auto& manager = EngineSystemManager::GetInstance();
    
    // Register your subsystem
    auto loggingDescriptor = EngineSystemDescriptor::Create<ILoggingSubsystem, LoggingSubsystem, FileLoggingProvider>(\"Logging\");
    manager.RegisterSystemDescriptor(loggingDescriptor);
    
    // Create the subsystem instance
    manager.CreateSystem<ILoggingSubsystem>(loggingDescriptor);
}
```

### 6. Use Your Subsystem

```cpp
// Anywhere in your game code after Engine::Initialize()
void SomeGameFunction() {
    auto* logging = GetEngineSubsystem(ILoggingSubsystem);
    
    if (logging) {
        logging->LogInfo(\"Game started successfully!\");
        logging->LogError(\"Example error message\");
    }
}
```

## Complete Working Example

Here's a minimal but complete example you can copy and modify:

```cpp
// LoggingSubsystem.hpp
#pragma once
#include \"AIEngine/core/IEngineSubsystem.hpp\"
#include \"AIEngine/core/ISubsystemProvider.hpp\"
#include <fstream>
#include <memory>

namespace AIEngine {
    
    class ILoggingProvider : public ISubsystemProvider {
    public:
        virtual void WriteLog(const std::string& level, const std::string& message) = 0;
    };
    
    class ILoggingSubsystem : public IEngineSubsystem {
    public:
        virtual void LogInfo(const std::string& message) = 0;
        virtual void LogError(const std::string& message) = 0;
    };
    
    class FileLoggingProvider : public ILoggingProvider {
        bool initialized = false, running = false;
        std::unique_ptr<std::ofstream> logFile;
        
    public:
        bool Initialize() override {
            logFile = std::make_unique<std::ofstream>(\"engine.log\");
            return initialized = logFile->is_open();
        }
        void Start() override { running = true; }
        void Stop() override { running = false; }
        void Destroy() override { logFile.reset(); initialized = false; }
        bool IsInitialized() const override { return initialized; }
        bool IsRunning() const override { return running; }
        const std::string& GetProviderName() const override {
            static const std::string name = \"File Logging\"; return name;
        }
        const std::string& GetProviderId() const override {
            static const std::string id = \"file-logging\"; return id;
        }
        void WriteLog(const std::string& level, const std::string& message) override {
            if (logFile && running) *logFile << \"[\" << level << \"] \" << message << std::endl;
        }
    };
    
    class LoggingSubsystem : public ILoggingSubsystem {
        std::unique_ptr<ILoggingProvider> provider;
        
    public:
        bool Initialize() override { return provider && provider->Initialize(); }
        void Start() override { provider->Start(); }
        void Stop() override { provider->Stop(); }
        void Destroy() override { provider->Destroy(); }
        bool IsInitialized() const override { return provider && provider->IsInitialized(); }
        bool IsRunning() const override { return provider && provider->IsRunning(); }
        
        void SetSubsystemProvider(std::unique_ptr<ISubsystemProvider> p) override {
            provider = std::unique_ptr<ILoggingProvider>(static_cast<ILoggingProvider*>(p.release()));
        }
        ISubsystemProvider* GetSubsystemProvider() const override { return provider.get(); }
        bool HasProvider() const override { return provider != nullptr; }
        
        EngineSystemDescriptor GetDescriptor() const override {
            return EngineSystemDescriptor::Create<ILoggingSubsystem, LoggingSubsystem, FileLoggingProvider>(\"Logging\");
        }
        const std::string& GetSubsystemName() const override {
            static const std::string name = \"Logging\"; return name;
        }
        
        void LogInfo(const std::string& message) override { provider->WriteLog(\"INFO\", message); }
        void LogError(const std::string& message) override { provider->WriteLog(\"ERROR\", message); }
    };
}

// Usage:
// auto desc = EngineSystemDescriptor::Create<ILoggingSubsystem, LoggingSubsystem, FileLoggingProvider>(\"Logging\");
// EngineSystemManager::GetInstance().RegisterSystemDescriptor(desc);
// EngineSystemManager::GetInstance().CreateSystem<ILoggingSubsystem>(desc);
// GetEngineSubsystem(ILoggingSubsystem)->LogInfo(\"Hello World!\");
```

## Common Patterns

### Cross-Platform Providers

```cpp
// Different providers for different platforms
#ifdef _WIN32
    using PlatformLoggingProvider = WindowsLoggingProvider;
#elif defined(__APPLE__)
    using PlatformLoggingProvider = MacLoggingProvider;
#else
    using PlatformLoggingProvider = LinuxLoggingProvider;
#endif

auto descriptor = EngineSystemDescriptor::Create<ILoggingSubsystem, LoggingSubsystem, PlatformLoggingProvider>(\"Logging\");
```

### Error Handling

```cpp
// Always check for nullptr when accessing subsystems
auto* graphics = GetEngineSubsystem(IGraphicsSubsystem);
if (!graphics) {
    // Subsystem not available - handle gracefully
    return false;
}

// Provider initialization can fail
bool Initialize() override {
    auto* provider = GetMyProvider();
    if (!provider) {
        // Log error: \"MySubsystem: No provider assigned\"
        return false;
    }
    
    if (!provider->Initialize()) {
        // Log error: \"MySubsystem: Provider initialization failed\"  
        return false;
    }
    
    return true;
}
```

### Registration Order for Dependencies

```cpp
// Register in dependency order
void RegisterSubsystems() {
    auto& manager = EngineSystemManager::GetInstance();
    
    // Base systems first
    manager.RegisterSystemDescriptor(windowDescriptor);     // Graphics needs window
    manager.RegisterSystemDescriptor(graphicsDescriptor);   // Audio needs graphics context
    manager.RegisterSystemDescriptor(audioDescriptor);
    manager.RegisterSystemDescriptor(inputDescriptor);      // Input needs window
    
    // Create in same order
    manager.CreateSystem<IWindowSubsystem>(windowDescriptor);
    manager.CreateSystem<IGraphicsSubsystem>(graphicsDescriptor);  
    manager.CreateSystem<IAudioSubsystem>(audioDescriptor);
    manager.CreateSystem<IInputSubsystem>(inputDescriptor);
}
```

## Troubleshooting

### Common Issues

**Q: GetSystem<T>() returns nullptr**  
A: Make sure you called RegisterSystemDescriptor() and CreateSystem() before Engine::Initialize()

**Q: Initialize() fails with \"No provider assigned\"**  
A: Provider creation failed. Check your EngineSystemDescriptor::Create<> template parameters

**Q: Subsystem methods crash**  
A: Always check GetSubsystemProvider() != nullptr before calling provider methods

**Q: Registration order issues**  
A: Register dependent subsystems after their dependencies. Use the same order for creation.

### Debugging Tips

- Use descriptive system names for easier debugging
- Check Engine::Initialize() return value - it will be false if any subsystem fails
- Implement proper error messages in your Initialize() methods

---

## Cross-Platform Provider Pattern Example

This example demonstrates how to create a graphics subsystem that works identically across different platforms by using platform-specific providers.

### 1. Define Cross-Platform Subsystem Interface

```cpp
namespace AIEngine {
    
    // Graphics subsystem interface - same across all platforms
    class IGraphicsSubsystem : public IEngineSubsystem {
    public:
        virtual ~IGraphicsSubsystem() = default;
        
        // Platform-independent graphics methods
        virtual void Render() = 0;
        virtual void SetViewport(int width, int height) = 0;
        virtual bool CreateBuffer(const std::string& name, size_t size) = 0;
        virtual bool LoadShader(const std::string& name, const std::string& vertexCode, const std::string& fragmentCode) = 0;
        
        // Type-safe provider access
        IGraphicsProvider* GetGraphicsProvider() const {
            return static_cast<IGraphicsProvider*>(GetSubsystemProvider());
        }
    };
    
    // Graphics provider interface - implemented differently per platform
    class IGraphicsProvider : public ISubsystemProvider {
    public:
        virtual ~IGraphicsProvider() = default;
        
        // Platform-specific graphics API methods
        virtual void InitializeAPI() = 0;
        virtual void ShutdownAPI() = 0;
        virtual bool CreateContext() = 0;
        virtual void SwapBuffers() = 0;
        virtual std::string GetRendererInfo() const = 0;
        virtual std::string GetAPIVersion() const = 0;
        
        // Platform detection
        virtual std::string GetPlatformName() const = 0;
        virtual bool SupportsFeature(const std::string& feature) const = 0;
    };
}
```

### 2. Implement Platform-Independent Subsystem

```cpp
namespace AIEngine {
    
    class GraphicsSubsystem : public IGraphicsSubsystem {
    public:
        // IEngineSubsystem implementation - delegates everything to provider
        bool Initialize() override {
            if (!GetGraphicsProvider()) {
                lastError = "No graphics provider set";
                return false;
            }
            
            // Initialize provider first
            if (!GetGraphicsProvider()->Initialize()) {
                lastError = "Graphics provider initialization failed: " + GetGraphicsProvider()->GetProviderName();
                return false;
            }
            
            // Platform-specific API initialization
            GetGraphicsProvider()->InitializeAPI();
            
            if (!GetGraphicsProvider()->CreateContext()) {
                lastError = "Failed to create graphics context";
                return false;
            }
            
            std::cout << "Graphics initialized: " << GetGraphicsProvider()->GetRendererInfo() 
                      << " on " << GetGraphicsProvider()->GetPlatformName() << std::endl;
            
            return true;
        }
        
        void Start() override {
            if (GetGraphicsProvider()) {
                GetGraphicsProvider()->Start();
            }
        }
        
        void Stop() override {
            if (GetGraphicsProvider()) {
                GetGraphicsProvider()->Stop();
            }
        }
        
        void Destroy() override {
            if (GetGraphicsProvider()) {
                GetGraphicsProvider()->ShutdownAPI();
                GetGraphicsProvider()->Destroy();
            }
        }
        
        bool IsInitialized() const override { 
            return GetGraphicsProvider() && GetGraphicsProvider()->IsInitialized(); 
        }
        
        bool IsRunning() const override { 
            return GetGraphicsProvider() && GetGraphicsProvider()->IsRunning(); 
        }
        
        // Provider management
        void SetSubsystemProvider(std::unique_ptr<ISubsystemProvider> provider) override {
            graphicsProvider = std::unique_ptr<IGraphicsProvider>(
                static_cast<IGraphicsProvider*>(provider.release()));
        }
        
        ISubsystemProvider* GetSubsystemProvider() const override {
            return graphicsProvider.get();
        }
        
        bool HasProvider() const override {
            return graphicsProvider != nullptr;
        }
        
        EngineSystemDescriptor GetDescriptor() const override {
            // Note: Descriptor will be created with appropriate provider type at registration time
            return EngineSystemDescriptor::Create<IGraphicsSubsystem, GraphicsSubsystem, IGraphicsProvider>("Graphics");
        }
        
        const std::string& GetSubsystemName() const override {
            static const std::string name = "Graphics Subsystem";
            return name;
        }
        
        // Graphics-specific methods - delegate to provider as needed
        void Render() override {
            if (!IsRunning()) {
                throw std::runtime_error("Cannot render: graphics subsystem not running");
            }
            
            // Perform rendering using platform-specific provider
            GetGraphicsProvider()->SwapBuffers();
            renderCallCount++;
        }
        
        void SetViewport(int width, int height) override {
            if (!IsInitialized()) {
                throw std::runtime_error("Cannot set viewport: graphics not initialized");
            }
            
            viewportWidth = width;
            viewportHeight = height;
            
            // Platform-specific viewport setting would be delegated here
            // GetGraphicsProvider()->SetViewportImpl(width, height);
        }
        
        bool CreateBuffer(const std::string& name, size_t size) override {
            if (!IsInitialized()) return false;
            
            // Delegate buffer creation to platform-specific implementation
            // return GetGraphicsProvider()->CreateBufferImpl(name, size);
            buffers[name] = size;  // Simplified for example
            return true;
        }
        
        bool LoadShader(const std::string& name, const std::string& vertexCode, const std::string& fragmentCode) override {
            if (!IsInitialized()) return false;
            
            // Delegate shader compilation to platform-specific implementation  
            // return GetGraphicsProvider()->CompileShaderImpl(name, vertexCode, fragmentCode);
            shaders[name] = {vertexCode, fragmentCode};  // Simplified for example
            return true;
        }
        
        // Diagnostic methods
        const std::string& GetLastError() const { return lastError; }
        int GetRenderCallCount() const { return renderCallCount; }
        std::pair<int, int> GetViewportSize() const { return {viewportWidth, viewportHeight}; }
        
    private:
        std::unique_ptr<IGraphicsProvider> graphicsProvider;
        std::string lastError;
        int renderCallCount = 0;
        int viewportWidth = 0;
        int viewportHeight = 0;
        std::map<std::string, size_t> buffers;
        std::map<std::string, std::pair<std::string, std::string>> shaders;
    };
}
```

### 3. Implement Platform-Specific Providers

```cpp
namespace AIEngine {
    
    // Windows DirectX provider
    class DirectXGraphicsProvider : public IGraphicsProvider {
    public:
        DirectXGraphicsProvider() = default;
        
        bool Initialize() override {
            initialized = true;
            return true;
        }
        
        void Start() override { running = true; }
        void Stop() override { running = false; }
        void Destroy() override { 
            ShutdownAPI();
            initialized = false; 
        }
        
        bool IsInitialized() const override { return initialized; }
        bool IsRunning() const override { return running; }
        
        const std::string& GetProviderName() const override {
            static const std::string name = "DirectX Graphics Provider";
            return name;
        }
        
        const std::string& GetProviderId() const override {
            static const std::string id = "directx_graphics";
            return id;
        }
        
        // Platform-specific implementation
        void InitializeAPI() override {
            // Initialize DirectX API
            std::cout << "Initializing DirectX..." << std::endl;
        }
        
        void ShutdownAPI() override {
            std::cout << "Shutting down DirectX..." << std::endl;
        }
        
        bool CreateContext() override {
            std::cout << "Creating DirectX context..." << std::endl;
            return true;
        }
        
        void SwapBuffers() override {
            // DirectX-specific buffer swap
        }
        
        std::string GetRendererInfo() const override {
            return "DirectX 11 Renderer";
        }
        
        std::string GetAPIVersion() const override {
            return "DirectX 11.0";
        }
        
        std::string GetPlatformName() const override {
            return "Windows";
        }
        
        bool SupportsFeature(const std::string& feature) const override {
            return feature == "DirectX" || feature == "Windows" || feature == "Hardware Accelerated";
        }
        
    private:
        bool initialized = false;
        bool running = false;
    };
    
    // Linux OpenGL provider
    class OpenGLGraphicsProvider : public IGraphicsProvider {
    public:
        OpenGLGraphicsProvider() = default;
        
        bool Initialize() override {
            initialized = true;
            return true;
        }
        
        void Start() override { running = true; }
        void Stop() override { running = false; }
        void Destroy() override { 
            ShutdownAPI();
            initialized = false; 
        }
        
        bool IsInitialized() const override { return initialized; }
        bool IsRunning() const override { return running; }
        
        const std::string& GetProviderName() const override {
            static const std::string name = "OpenGL Graphics Provider";
            return name;
        }
        
        const std::string& GetProviderId() const override {
            static const std::string id = "opengl_graphics";
            return id;
        }
        
        // Platform-specific implementation
        void InitializeAPI() override {
            std::cout << "Initializing OpenGL..." << std::endl;
        }
        
        void ShutdownAPI() override {
            std::cout << "Shutting down OpenGL..." << std::endl;
        }
        
        bool CreateContext() override {
            std::cout << "Creating OpenGL context..." << std::endl;
            return true;
        }
        
        void SwapBuffers() override {
            // OpenGL-specific buffer swap
        }
        
        std::string GetRendererInfo() const override {
            return "OpenGL 3.3 Core Renderer";
        }
        
        std::string GetAPIVersion() const override {
            return "OpenGL 3.3";
        }
        
        std::string GetPlatformName() const override {
            return "Linux";
        }
        
        bool SupportsFeature(const std::string& feature) const override {
            return feature == "OpenGL" || feature == "Linux" || feature == "Cross Platform";
        }
        
    private:
        bool initialized = false;
        bool running = false;
    };
}
```

### 4. Platform-Specific Registration

```cpp
// Platform detection and registration
void RegisterGraphicsSubsystem() {
    auto& manager = EngineSystemManager::GetInstance();
    
    // Detect platform and register appropriate provider
#ifdef _WIN32
    auto graphicsDesc = EngineSystemDescriptor::Create<IGraphicsSubsystem, GraphicsSubsystem, DirectXGraphicsProvider>("Graphics");
    std::cout << "Registering DirectX graphics provider for Windows" << std::endl;
#elif defined(__linux__)  
    auto graphicsDesc = EngineSystemDescriptor::Create<IGraphicsSubsystem, GraphicsSubsystem, OpenGLGraphicsProvider>("Graphics");
    std::cout << "Registering OpenGL graphics provider for Linux" << std::endl;
#else
    #error "Unsupported platform for graphics subsystem"
#endif
    
    manager.RegisterSystemDescriptor(graphicsDesc);
    
    // Create the graphics system
    auto* graphics = manager.CreateSystem<IGraphicsSubsystem>(graphicsDesc);
    
    std::cout << "Graphics subsystem registered with " 
              << graphics->GetGraphicsProvider()->GetProviderName() << std::endl;
}
```

### 5. Cross-Platform Usage

```cpp
void RenderGame() {
    auto& manager = EngineSystemManager::GetInstance();
    auto* graphics = manager.GetSystem<IGraphicsSubsystem>();
    
    if (!graphics || !graphics->IsRunning()) {
        std::cerr << "Graphics subsystem not available" << std::endl;
        return;
    }
    
    // This code works identically on all platforms
    graphics->SetViewport(1920, 1080);
    graphics->CreateBuffer("VertexBuffer", 1024 * 1024);
    graphics->LoadShader("BasicShader", vertexShaderCode, fragmentShaderCode);
    graphics->Render();
    
    // Platform-specific information available if needed
    auto* provider = graphics->GetGraphicsProvider();
    std::cout << "Rendering on " << provider->GetPlatformName() 
              << " using " << provider->GetAPIVersion() << std::endl;
}
```

### Key Benefits

1. **Platform Independence**: Game code only interacts with `IGraphicsSubsystem`, never platform-specific providers
2. **Easy Platform Support**: Add new platforms by implementing new providers, no changes to game code
3. **Runtime Platform Detection**: Providers selected automatically based on platform
4. **Type Safety**: Compile-time validation of subsystem-provider compatibility
5. **Clean Separation**: Platform-specific concerns isolated in provider implementations
6. **Testability**: Easy to create mock providers for unit testing

This pattern enables the same graphics code to work on Windows with DirectX, Linux with OpenGL, or any future platform by simply implementing the appropriate provider.

- Use logging subsystem to trace initialization order and failures
