# Engine Library Public API Contract

**Purpose**: Defines the public interface contract for the AIEngine library that external applications must use  
**Version**: 1.0.0  
**Stability**: Initial development  

## Core Engine Interface

### Engine Initialization & Lifecycle

```cpp
namespace AIEngine {
    
    struct EngineConfig {
        uint32_t windowWidth = 800;
        uint32_t windowHeight = 600;
        std::string windowTitle = "AIEngine Application";
        bool enableVSync = true;
    };
    
    class Engine {
    public:
        // Construction/Destruction  
        explicit Engine(const EngineConfig& config);
        ~Engine();
        
        // Non-copyable, movable
        Engine(const Engine&) = delete;
        Engine& operator=(const Engine&) = delete;
        Engine(Engine&&) = default;
        Engine& operator=(Engine&&) = default;
        
        // Lifecycle Management
        bool Initialize();              // Setup graphics context, systems
        void Update(float deltaTime);   // Update all systems and scene graph  
        void Render();                 // Render frame to screen via scene traversal
        void Shutdown();               // Clean shutdown of all systems
        bool ShouldClose() const;      // Check if engine should terminate
        
        // Scene Graph Management
        SceneGraph* GetSceneGraph();
        SceneNode* CreateSceneNode();
        void DestroySceneNode(uint32_t nodeId);
        
        // Time Management  
        float GetDeltaTime() const;
        float GetTotalTime() const;
        
        // Engine System Management
        EngineSystemManager& GetSystemManager();
        
        template<typename SystemType>
        SystemType* GetSystem();
        
        template<typename SystemType>
        const SystemType* GetSystem() const;
    };
}
```

## Engine System Architecture (Facade Pattern)

### System Base Interfaces

```cpp
namespace AIEngine {
    
    // System descriptor for creation and identification
    class EngineSystemDescriptor {
    public:
        // Create descriptor with subsystem and provider types
        template<typename SubsystemInterface, typename SubsystemImpl, typename ProviderType>
        static EngineSystemDescriptor Create(const std::string& name);
        
        // Create and register system directly - returns created system instance
        template<typename SubsystemInterface>
        SubsystemInterface* Create();
        
        // Type identification (interface and provider)
        size_t GetInterfaceHash() const { return interfaceHash; }
        size_t GetProviderTypeHash() const { return providerTypeHash; }
        const std::string& GetSystemName() const { return systemName; }
        const std::string& GetInterfaceTypeName() const { return interfaceTypeName; }
        const std::string& GetProviderTypeName() const { return providerTypeName; }
        const std::string& GetImplementationId() const { return implementationId; }
        uint32_t GetSystemId() const { return systemId; }
        
        // System creation using stored creation function (creates both subsystem and provider)
        template<typename SubsystemInterface>
        std::unique_ptr<SubsystemInterface> CreateSystem() const;
        
        // Provider identification (by type and string)
        template<typename ProviderType>
        bool IsProvider() const;
        bool IsImplementation(const std::string& implId) const { return implementationId == implId; }
        
        bool operator==(const EngineSystemDescriptor& other) const {
            return interfaceHash == other.interfaceHash && 
                   providerTypeHash == other.providerTypeHash;
        }
        
    private:
        EngineSystemDescriptor(size_t iHash, size_t pHash, std::string name, 
                             std::string iTypeName, std::string pTypeName, 
                             std::string implId, uint32_t id,
                             std::function<std::unique_ptr<void>()> createFunc);
        
        size_t interfaceHash;
        size_t providerTypeHash;
        std::string systemName;
        std::string interfaceTypeName;
        std::string providerTypeName;
        std::string implementationId;     // Provider-based implementation identifier
        uint32_t systemId;
        
        // Creation function (type-erased for storage)
        std::function<std::unique_ptr<void>()> creationFunction;
        
        static uint32_t nextSystemId;
        friend class EngineSystemManager;
    };
    
    // Forward declaration for friend access
    class EngineSystemManager;
    
    // Base subsystem provider interface
    class ISubsystemProvider {
    public:
        virtual ~ISubsystemProvider() = default;
        
        // Provider lifecycle
        virtual bool Initialize() = 0;
        virtual void Start() = 0;
        virtual void Stop() = 0;
        virtual void Destroy() = 0;
        
        // Provider state
        virtual bool IsInitialized() const = 0;
        virtual bool IsRunning() const = 0;
        
        // Provider identification
        virtual const std::string& GetProviderName() const = 0;
        virtual const std::string& GetProviderId() const = 0;
    };
    
    // Base interface for all engine subsystems
    class IEngineSubsystem {
    public:
        virtual ~IEngineSubsystem() = default;
        
        // Subsystem lifecycle (delegates to provider)
        virtual bool Initialize() = 0;
        virtual void Start() = 0;
        virtual void Stop() = 0;
        virtual void Destroy() = 0;
        
        // Subsystem state
        virtual bool IsInitialized() const = 0;
        virtual bool IsRunning() const = 0;
        
        // Provider management
        virtual void SetSubsystemProvider(std::unique_ptr<ISubsystemProvider> provider) = 0;
        virtual ISubsystemProvider* GetSubsystemProvider() const = 0;
        virtual bool HasProvider() const = 0;
        
        // Subsystem metadata
        virtual EngineSystemDescriptor GetDescriptor() const = 0;
        virtual const std::string& GetSubsystemName() const = 0;
    };
    

```

### System Manager (Singleton)

```cpp
namespace AIEngine {
    
    class EngineSystemManager {
    public:
        // Singleton access
        static EngineSystemManager& GetInstance();
        
        // Descriptor Registration
        void RegisterSystemDescriptor(const EngineSystemDescriptor& descriptor);
        void UnregisterSystemDescriptor(const EngineSystemDescriptor& descriptor);
        
        // System Creation from Descriptor
        template<typename SystemInterface>
        SystemInterface* CreateSystem(const EngineSystemDescriptor& descriptor);
        
        template<typename SystemInterface>
        void DestroySystem(SystemInterface* system);
        
        // System Access (O(1) performance)
        template<typename SystemInterface>
        SystemInterface* GetSystem();
        
        template<typename SystemInterface>
        const SystemInterface* GetSystem() const;
        
        template<typename SystemInterface>
        bool HasSystem() const;
        
        // Implementation queries (by string identifier)
        bool HasImplementation(const std::string& implementationId) const;
        std::string GetActiveImplementation(size_t interfaceHash) const;
        
        // Lifecycle Management
        bool InitializeAllSystems();
        void StartAllSystems();
        void StopAllSystems();
        void DestroyAllSystems();
        
        // System Enumeration
        std::vector<EngineSystemDescriptor> GetRegisteredDescriptors() const;
        std::vector<EngineSystemDescriptor> GetDescriptorsForInterface(size_t interfaceHash) const;
        std::vector<IEngineSubsystem*> GetActiveSubsystems();
        std::vector<const IEngineSubsystem*> GetActiveSubsystems() const;
        
    private:
        EngineSystemManager() = default;
        ~EngineSystemManager();
        
        // Non-copyable, non-movable singleton
        EngineSystemManager(const EngineSystemManager&) = delete;
        EngineSystemManager& operator=(const EngineSystemManager&) = delete;
        EngineSystemManager(EngineSystemManager&&) = delete;
        EngineSystemManager& operator=(EngineSystemManager&&) = delete;
        
        struct SystemEntry {
            std::unique_ptr<IEngineSubsystem> subsystem;
            EngineSystemDescriptor descriptor;
            bool initialized = false;
            bool running = false;
        };
        
        // Fast lookup by interface hash
        std::unordered_map<size_t, std::unique_ptr<SystemEntry>> activeSubsystemsByInterface;
        std::unordered_map<size_t, std::vector<EngineSystemDescriptor>> registeredDescriptorsByInterface;
        mutable std::mutex systemMutex;           // Thread safety
    };
    
    // Convenience macro for subsystem access
    #define GetEngineSubsystem(SubsystemType) \
        EngineSystemManager::GetInstance().GetSystem<SubsystemType>()
}
```

### Example System Implementations

```cpp
namespace AIEngine {
    
    // Example: Graphics Subsystem Interface
    class IGraphicsSystemProvider : public ISubsystemProvider {
    public:
        virtual ~IGraphicsSystemProvider() = default;
        
        // Graphics-specific provider interface
        virtual bool CreateRenderContext() = 0;
        virtual void SetViewport(uint32_t width, uint32_t height) = 0;
        virtual void ClearScreen(float r, float g, float b, float a = 1.0f) = 0;
        virtual void SwapBuffers() = 0;
        virtual uint32_t LoadShader(const std::string& vertexPath, const std::string& fragmentPath) = 0;
    };
    
    class IGraphicsSubsystem : public IEngineSubsystem {
    public:
        virtual ~IGraphicsSubsystem() = default;
        
        // Graphics-specific interface (delegates to provider)
        virtual bool CreateRenderContext() = 0;
        virtual void SetViewport(uint32_t width, uint32_t height) = 0;
        virtual void ClearScreen(float r, float g, float b, float a = 1.0f) = 0;
        virtual void SwapBuffers() = 0;
        virtual uint32_t LoadShader(const std::string& vertexPath, const std::string& fragmentPath) = 0;
        
        // Type-safe provider access
        void SetGraphicsProvider(std::unique_ptr<IGraphicsSystemProvider> provider) {
            SetSubsystemProvider(std::move(provider));
        }
        
        IGraphicsSystemProvider* GetGraphicsProvider() const {
            return static_cast<IGraphicsSystemProvider*>(GetSubsystemProvider());
        }
    };
    
    // OpenGL Graphics Provider (actual implementation)
    class OpenGLGraphicsProvider : public IGraphicsSystemProvider {
    public:
        explicit OpenGLGraphicsProvider(bool debugMode);
        
        // ISubsystemProvider implementation
        bool Initialize() override;
        void Start() override;
        void Stop() override;
        void Destroy() override;
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
        
        // IGraphicsSystemProvider implementation
        bool CreateRenderContext() override;
        void SetViewport(uint32_t width, uint32_t height) override;
        void ClearScreen(float r, float g, float b, float a = 1.0f) override;
        void SwapBuffers() override;
        uint32_t LoadShader(const std::string& vertexPath, const std::string& fragmentPath) override;
        
    private:
        bool initialized = false;
        bool running = false;
        bool debugMode;
        // OpenGL-specific members...
    };
    
    // Concrete Graphics Subsystem (uses provider via composition)
    class GraphicsSubsystem : public IGraphicsSubsystem {
    public:
        GraphicsSubsystem() = default;
        
        // IEngineSubsystem implementation (delegates to provider)
        bool Initialize() override {
            if (!provider) {
                return false;  // Cannot initialize without provider
            }
            return provider->Initialize();
        }
        
        void Start() override {
            provider->Start();  // Provider guaranteed to exist after Initialize()
        }
        
        void Stop() override {
            provider->Stop();
        }
        
        void Destroy() override {
            provider->Destroy();
        }
        
        bool IsInitialized() const override {
            return provider && provider->IsInitialized();
        }
        
        bool IsRunning() const override {
            return provider && provider->IsRunning();
        }
        
        // Provider management
        void SetSubsystemProvider(std::unique_ptr<ISubsystemProvider> newProvider) override {
            provider = std::unique_ptr<IGraphicsSystemProvider>(
                static_cast<IGraphicsSystemProvider*>(newProvider.release()));
        }
        
        ISubsystemProvider* GetSubsystemProvider() const override {
            return provider.get();
        }
        
        bool HasProvider() const override {
            return provider != nullptr;
        }
        
        EngineSystemDescriptor GetDescriptor() const override {
            return EngineSystemDescriptor::Create<IGraphicsSubsystem, GraphicsSubsystem, OpenGLGraphicsProvider>(
                "OpenGL Graphics");
        }
        
        const std::string& GetSubsystemName() const override {
            static const std::string name = "Graphics Subsystem";
            return name;
        }
        
        // IGraphicsSubsystem implementation (delegates to provider)
        bool CreateRenderContext() override {
            return provider->CreateRenderContext();
        }
        
        void SetViewport(uint32_t width, uint32_t height) override {
            provider->SetViewport(width, height);
        }
        
        void ClearScreen(float r, float g, float b, float a) override {
            provider->ClearScreen(r, g, b, a);
        }
        
        void SwapBuffers() override {
            provider->SwapBuffers();
        }
        
        uint32_t LoadShader(const std::string& vertexPath, const std::string& fragmentPath) override {
            return provider->LoadShader(vertexPath, fragmentPath);
        }
        
    private:
        std::unique_ptr<IGraphicsSystemProvider> provider;
    };
}
```

**Engine System Architecture Contract Requirements**:

- EngineSystemManager MUST be thread-safe for descriptor registration and system access
- GetSystem<T>() MUST return nullptr if system type T is not created
- EngineSystemDescriptor MUST contain both subsystem and provider type information
- EngineSystemDescriptor.Create<Interface, Impl, Provider>() MUST handle complete creation logic
- RegisterSystemDescriptor() MUST store descriptor for tracking available system types
- EngineSystemDescriptor.Create<Interface>() MUST create system and register active system instance internally
- Descriptor creation MUST automatically instantiate and assign providers to subsystems
- Initialize() MUST return false if internal provider creation fails
- Subsystem lifecycle MUST follow: Create() → Initialize() → Start() → Stop() → Destroy() sequence
- Provider calls MUST NOT include null checks - providers guaranteed from creation
- System lookup MUST execute in O(1) time using interface hash optimization
- EngineSystemDescriptor MUST identify implementations using provider type information
- Provider identification MUST use both type hash and string identifiers
- Descriptor creation functions MUST be fully self-contained
- DestroySystem() MUST properly cleanup subsystem resources and call Destroy()
- InitializeAllSystems() MUST initialize subsystems in registration order
- GetDescriptorsForInterface() MUST return all registered implementations for an interface
- Client code MUST NOT handle subsystem or provider creation manually
- All creation logic MUST be encapsulated within EngineSystemDescriptor

**Contract Requirements**:

- Engine MUST be initialized before calling Update() or Render()
- Initialize() MUST return true for successful startup, false for failures
- Update() and Render() MUST be called in sequence each frame
- ShouldClose() MUST return true when window close is requested
- Shutdown() MUST be called before Engine destruction

## Scene Graph Interface

### Scene Graph Management

```cpp
namespace AIEngine {
    
    class SceneGraph {
    public:
        // Root Node Access
        SceneNode* GetRootNode();
        
        // Traversal and Updates
        void Update(float deltaTime);         // Update entire hierarchy
        void Render(Renderer* renderer);     // Traverse and render visible nodes
        
        // Node Management
        SceneNode* CreateNode(SceneNode* parent = nullptr);
        void RemoveNode(SceneNode* node);
        void Clear();                         // Remove all nodes
    };
    
    class SceneNode {
    public:
        // Hierarchy Management
        SceneNode* GetParent() const;
        void SetParent(SceneNode* parent);
        void AddChild(SceneNode* child);
        void RemoveChild(SceneNode* child);
        const std::vector<SceneNode*>& GetChildren() const;
        
        // Component Management
        template<typename ComponentType, typename... Args>
        ComponentType& AddComponent(Args&&... args);
        
        template<typename ComponentType>  
        ComponentType* GetComponent();
        
        template<typename ComponentType>
        const ComponentType* GetComponent() const;
        
        template<typename ComponentType>
        void RemoveComponent();
        
        template<typename ComponentType>
        bool HasComponent() const;
        
        // Transform Management
        const glm::mat4& GetLocalTransform() const;
        const glm::mat4& GetWorldTransform() const;
        void SetLocalTransform(const glm::mat4& transform);
        void MarkTransformDirty();            // Force recalculation
        
        // Node Properties
        uint32_t GetId() const;
        bool IsVisible() const;
        void SetVisible(bool visible);
    };
    
    // Component Base Interface
    class IComponent {
    public:
        virtual ~IComponent() = default;
        virtual void Update(float deltaTime) {}  // Optional update behavior
    };
}
```

**Contract Requirements**:

- AddComponent() MUST return valid reference to created component
- GetComponent() MUST return nullptr if component doesn't exist
- Component templates MUST inherit from IComponent interface
- Scene Node IDs MUST be unique within scene graph
- SetVisible(false) MUST prevent node rendering but preserve hierarchy
- Transform hierarchy MUST maintain parent-child relationships correctly
- World transform MUST be calculated from parent chain automatically

## Graphics System Interface

### Core Components

```cpp
namespace AIEngine {
    
    // Transform Component - Required for positioned objects
    class TransformComponent : public IComponent {
    public:
        glm::vec3 position{0.0f, 0.0f, 0.0f};
        glm::vec3 rotation{0.0f, 0.0f, 0.0f};  // Euler angles in degrees
        glm::vec3 scale{1.0f, 1.0f, 1.0f};
        
        glm::mat4 GetTransformMatrix() const;   // Combined transformation
        void Update(float deltaTime) override;
    };
    
    // Render Component - Required for visible objects  
    class RenderComponent : public IComponent {
    public:
        uint32_t meshId = 0;        // Reference to loaded mesh
        bool visible = true;         // Visibility toggle
        
        void Update(float deltaTime) override;
    };
    
    // Built-in Geometry Factory
    class GeometryFactory {
    public:
        static uint32_t CreateCube(float size = 1.0f);     // Returns mesh ID
        static uint32_t CreateQuad(float width, float height);
        static uint32_t CreateSphere(float radius, uint32_t subdivisions);
    };
}
```

**Contract Requirements**:

- TransformComponent MUST provide GetTransformMatrix() for rendering
- RenderComponent.meshId MUST reference valid geometry or rendering fails
- GeometryFactory MUST return unique mesh IDs for each created geometry
- Mesh IDs MUST remain valid for entire engine lifetime
- Position/rotation/scale changes MUST affect rendering in next frame

## Usage Pattern Contract

### Required Application Structure

```cpp
#include <AIEngine/AIEngine.hpp>

int main() {
    // 1. Create engine with configuration
    AIEngine::EngineConfig config;
    config.windowTitle = "My Game";
    config.windowWidth = 1024;
    config.windowHeight = 768;
    
    AIEngine::Engine engine(config);
    
    // 2. Create descriptors with type information (engine initialization)
    auto& systemManager = engine.GetSystemManager();
    
    // Create descriptors - they handle subsystem and provider creation internally
    auto openglDesc = AIEngine::EngineSystemDescriptor::Create<
        AIEngine::IGraphicsSubsystem, 
        AIEngine::GraphicsSubsystem, 
        AIEngine::OpenGLGraphicsProvider>("OpenGL Graphics");
    
    auto vulkanDesc = AIEngine::EngineSystemDescriptor::Create<
        AIEngine::IGraphicsSubsystem, 
        AIEngine::GraphicsSubsystem, 
        AIEngine::VulkanGraphicsProvider>("Vulkan Graphics");
    
    // Register descriptors with system manager
    systemManager.RegisterSystemDescriptor(openglDesc);
    systemManager.RegisterSystemDescriptor(vulkanDesc);
    
    // Choose implementation at runtime - descriptor creates and registers system
    auto graphicsDesc = vulkanSupported ? vulkanDesc : openglDesc;
    auto* graphicsSubsystem = graphicsDesc.Create<AIEngine::IGraphicsSubsystem>();
    
    // 4. Initialize engine systems
    if (!engine.Initialize()) {
        return -1;  // Initialization failed
    }
    
    // 3. Create scene graph objects  
    auto sceneGraph = engine.GetSceneGraph();
    auto cubeNode = engine.CreateSceneNode();
    auto& transform = cubeNode->AddComponent<AIEngine::TransformComponent>();
    auto& render = cubeNode->AddComponent<AIEngine::RenderComponent>();

    sceneGraph->GetRootNode()->AddChild(cubeNode);
    transform.position = {0.0f, 0.0f, -5.0f};
    render.meshId = AIEngine::GeometryFactory::CreateCube(2.0f);
    
    // Access specific engine subsystems by interface only
    auto* graphicsSubsystem = GetEngineSubsystem(AIEngine::IGraphicsSubsystem);
    if (graphicsSubsystem && graphicsSubsystem->IsRunning()) {
        // No null checks needed - provider guaranteed to exist after Initialize()
        graphicsSubsystem->SetViewport(config.windowWidth, config.windowHeight);
        
        // Check implementation through provider type (no template types needed in client code)
        auto descriptor = graphicsSubsystem->GetDescriptor();
        if (descriptor.IsProvider<AIEngine::OpenGLGraphicsProvider>()) {
            std::cout << "Using OpenGL graphics implementation" << std::endl;
        } else if (descriptor.IsProvider<AIEngine::VulkanGraphicsProvider>()) {
            std::cout << "Using Vulkan graphics implementation" << std::endl;
        }
        
        // Access provider information - guaranteed to exist
        auto* provider = graphicsSubsystem->GetGraphicsProvider();
        std::cout << "Provider: " << provider->GetProviderName() << std::endl;
    }
    
    // 5. Start systems before main loop
    systemManager.StartAllSystems();
    
    // All scene graph manipulation code here...
    auto sceneGraph = engine.GetSceneGraph();
    auto cubeNode = engine.CreateSceneNode();
    auto& transform = cubeNode->AddComponent<AIEngine::TransformComponent>();
    auto& render = cubeNode->AddComponent<AIEngine::RenderComponent>();

    sceneGraph->GetRootNode()->AddChild(cubeNode);
    transform.position = {0.0f, 0.0f, -5.0f};
    render.meshId = AIEngine::GeometryFactory::CreateCube(2.0f);
    
    // 6. Main game loop
    while (!engine.ShouldClose()) {
        // Game engine systems perform work
        engine.Update();
    }
    
    // 7. Clean shutdown
    systemManager.StopAllSystems();
    engine.Shutdown();
    systemManager.DestroyAllSystems();
    return 0;
}
```

**Contract Requirements**:

- RegisterSystemDescriptor() MUST be called before creating systems
- CreateSystem() MUST be called after descriptor registration
- StartAllSystems() MUST be called before main game loop
- StopAllSystems() MUST be called before shutdown
- System lifecycle MUST follow Initialize() → Start() → Stop() → Destroy() sequence
- Game loop MUST call Update() then Render() each frame
- Applications MUST handle Initialize() returning false
- Applications SHOULD call Shutdown() before exit
- Frame timing MUST use engine-provided delta time for consistency
- Scene nodes MUST be added to scene graph hierarchy before rendering
- Transform hierarchy MUST be respected during world matrix calculation

### Basic Engine System Usage

```cpp
#include <AIEngine/AIEngine.hpp>

void BasicSystemUsage() {
    auto& systemManager = AIEngine::EngineSystemManager::GetInstance();
    
    // 1. Create descriptors with type information - no creation logic needed
    auto openglDesc = AIEngine::EngineSystemDescriptor::Create<
        AIEngine::IGraphicsSubsystem,
        AIEngine::GraphicsSubsystem,
        AIEngine::OpenGLGraphicsProvider>("OpenGL Graphics");
    
    auto vulkanDesc = AIEngine::EngineSystemDescriptor::Create<
        AIEngine::IGraphicsSubsystem,
        AIEngine::GraphicsSubsystem,
        AIEngine::VulkanGraphicsProvider>("Vulkan Graphics");
    
    auto audioDesc = AIEngine::EngineSystemDescriptor::Create<
        AIEngine::IAudioSubsystem,
        AIEngine::AudioSubsystem,
        AIEngine::OpenALAudioProvider>("OpenAL Audio");
    
    // 2. Register descriptors with system manager
    systemManager.RegisterSystemDescriptor(openglDesc);
    systemManager.RegisterSystemDescriptor(vulkanDesc);
    systemManager.RegisterSystemDescriptor(audioDesc);
    
    // 3. Create subsystems directly - descriptors handle system registration automatically
    auto* graphics = openglDesc.Create<AIEngine::IGraphicsSubsystem>();
    auto* audio = audioDesc.Create<AIEngine::IAudioSubsystem>();
    
        // 4. Verify provider type using type information
        if (openglDesc.IsProvider<AIEngine::OpenGLGraphicsProvider>()) {
            std::cout << "Created graphics subsystem with OpenGL provider" << std::endl;
        }
    
    // 5. Initialize and start subsystems (delegates to providers)
    if (graphics->Initialize() && audio->Initialize()) {
        graphics->Start();
        audio->Start();
        
        // Use subsystems through interface only - no null checks needed
        if (graphics->IsRunning()) {
            graphics->ClearScreen(0.0f, 0.0f, 0.0f);  // Provider guaranteed to exist
        }
        
        // Check active provider at runtime - guaranteed to exist after Initialize()
        auto* graphicsProvider = graphics->GetGraphicsProvider();
        std::cout << "Active graphics provider: " << graphicsProvider->GetProviderName() << std::endl;
        
        // Shutdown properly (delegates to providers)
        graphics->Stop();
        audio->Stop();
        graphics->Destroy();
        audio->Destroy();
    }
    
    // 6. Cleanup
    auto& systemManager = AIEngine::EngineSystemManager::GetInstance();
    systemManager.DestroySystem(graphics);
    systemManager.DestroySystem(audio);
}

// Custom system interface example
class IPhysicsSystem : public AIEngine::IEngineSystem {
public:
    virtual ~IPhysicsSystem() = default;
    virtual void AddRigidBody(uint32_t entityId, float mass) = 0;
    virtual void SetGravity(const glm::vec3& gravity) = 0;
    virtual void SimulateStep(float deltaTime) = 0;
};

// Usage with custom systems
void RegisterPhysicsSystem() {
    auto& systemManager = AIEngine::EngineSystemManager::GetInstance();
    
    // Create descriptors with type information - no manual creation
    auto bulletDesc = AIEngine::EngineSystemDescriptor::Create<
        IPhysicsSubsystem,
        PhysicsSubsystem,
        BulletPhysicsProvider>("Bullet Physics");
    
    auto physxDesc = AIEngine::EngineSystemDescriptor::Create<
        IPhysicsSubsystem,
        PhysicsSubsystem, 
        PhysXPhysicsProvider>("PhysX Physics");
    
    // Register descriptors to track available systems
    systemManager.RegisterSystemDescriptor(bulletDesc);
    systemManager.RegisterSystemDescriptor(physxDesc);
    
    // Create specific subsystem - descriptor handles system instance registration
    auto* physics = bulletDesc.Create<IPhysicsSubsystem>();
    if (physics && physics->Initialize()) {
        physics->Start();
        physics->SetGravity({0.0f, -9.81f, 0.0f});
        
        // Verify provider using type information
        if (bulletDesc.IsProvider<BulletPhysicsProvider>()) {
            std::cout << "Using Bullet physics provider" << std::endl;
        }
        
        // Access provider directly - guaranteed to exist after Initialize()
        auto* provider = physics->GetPhysicsProvider();
        std::cout << "Provider: " << provider->GetProviderName() << std::endl;
    }
    
    // Query available provider types for an interface
    auto physicsDescriptors = systemManager.GetDescriptorsForInterface(
        typeid(IPhysicsSubsystem).hash_code());
    
    for (const auto& desc : physicsDescriptors) {
        std::cout << "Available physics: " << desc.GetProviderTypeName() << std::endl;
    }
}
```

### Performance Optimized Example

```cpp
void PerformantGraphicsOperations() {
    auto& systemManager = AIEngine::EngineSystemManager::GetInstance();
    auto* graphics = systemManager.GetSystem<AIEngine::IGraphicsSubsystem>();
    
    // After Initialize() succeeds, provider is guaranteed to exist
    // No null checks needed for maximum performance
    if (graphics && graphics->IsInitialized()) {
        // Direct delegation - no null checks
        graphics->ClearScreen(0.2f, 0.3f, 0.3f);
        graphics->SetViewport(1920, 1080);
        
        uint32_t shader = graphics->LoadShader(
            "assets/shaders/vertex.glsl",
            "assets/shaders/fragment.glsl"
        );
        
        // Provider access also guaranteed
        auto* provider = graphics->GetGraphicsProvider();
        std::cout << "Using: " << provider->GetProviderName() << std::endl;
        
        graphics->SwapBuffers();
    }
}
```

### Implementation Identification Example

```cpp
#include <AIEngine/AIEngine.hpp>

void CheckGraphicsImplementation() {
    auto& systemManager = AIEngine::EngineSystemManager::GetInstance();
    
    // Check what graphics provider is active
    auto* graphics = systemManager.GetSystem<AIEngine::IGraphicsSubsystem>();
    if (graphics) {
        // Get the descriptor to check provider (no template types)
        auto descriptor = graphics->GetDescriptor();
        
        // Check specific provider using string identifiers
        if (descriptor.IsImplementation("opengl-graphics")) {
            std::cout << "OpenGL Graphics provider is active" << std::endl;
        }
        else if (descriptor.IsImplementation("vulkan-graphics")) {
            std::cout << "Vulkan Graphics provider is active" << std::endl;
        }
        else if (descriptor.IsImplementation("directx-graphics")) {
            std::cout << "DirectX Graphics provider is active" << std::endl;
        }
        else {
            std::cout << "Unknown graphics provider: " 
                      << descriptor.GetImplementationId() << std::endl;
        }
        
        // Access provider information directly - guaranteed to exist after Initialize()
        auto* provider = graphics->GetGraphicsProvider();
        std::cout << "Provider ID: " << provider->GetProviderId() << std::endl;
        std::cout << "Provider name: " << provider->GetProviderName() << std::endl;
        std::cout << "Provider running: " << (provider->IsRunning() ? "Yes" : "No") << std::endl;
        
        // Display subsystem information
        std::cout << "Implementation ID: " << descriptor.GetImplementationId() << std::endl;
        std::cout << "Interface type: " << descriptor.GetInterfaceTypeName() << std::endl;
        std::cout << "Subsystem name: " << descriptor.GetSystemName() << std::endl;
    }
    
    // List all available graphics provider implementations
    auto graphicsDescriptors = systemManager.GetDescriptorsForInterface(
        typeid(AIEngine::IGraphicsSubsystem).hash_code());
    
    std::cout << "Available graphics providers:" << std::endl;
    for (const auto& desc : graphicsDescriptors) {
        std::cout << "  - " << desc.GetImplementationId() 
                  << " (" << desc.GetSystemName() << ")" << std::endl;
    }
    
    // Query active provider implementation directly
    std::string activeGraphicsImpl = systemManager.GetActiveImplementation(
        typeid(AIEngine::IGraphicsSubsystem).hash_code());
    
    if (!activeGraphicsImpl.empty()) {
        std::cout << "Currently active: " << activeGraphicsImpl << std::endl;
    } else {
        std::cout << "No graphics provider currently active" << std::endl;
    }
}
```

**Engine System Contract Requirements**:

- Subsystem interfaces MUST inherit from IEngineSubsystem
- Provider classes MUST inherit from ISubsystemProvider or specific provider interface
- Descriptors MUST use Create<Interface, Impl, Provider>() template for type information
- Creation functions MUST be automatically generated and handle complete subsystem + provider setup
- Descriptors MUST store both interface and provider type hashes for identification
- RegisterSystemDescriptor() MUST store descriptors with automated creation capability
- CreateSystem() MUST use descriptor's internal creation to instantiate fully configured subsystems
- GetSystem<T>() MUST return nullptr for non-existent subsystem instances
- IsProvider<ProviderType>() MUST accurately identify provider using type information
- Subsystem Initialize() MUST return false if provider creation fails during descriptor creation
- Subsystem lifecycle MUST delegate to provider: Initialize() → Start() → Stop() → Destroy() sequence
- Provider assignment MUST happen automatically during descriptor creation, never manually
- Provider method calls MUST NOT include null checks after successful Initialize()
- Multiple descriptors MAY be registered for same interface with different provider types
- Client code MUST register descriptors first: RegisterSystemDescriptor(), then create systems: descriptor.Create<Interface>()
- Provider identification MUST use both type hashes and type names for verification
- Descriptor.Create<Interface>() MUST completely encapsulate subsystem creation, provider assignment, and active system registration
- All method delegation MUST assume provider validity after Initialize() succeeds
- EngineSystemManager MUST be friend class to EngineSystemDescriptor for internal system instance registration access

## Build System Contract

### Library Linking

```makefile
# Required link flags for applications using AIEngine
AIENGINE_LIBS = -lAIEngine -lSDL2 -lSDL2main -lopengl32 -lgdi32
AIENGINE_INCLUDE = -I$(AIENGINE_ROOT)/include

# Example application build
my_game: my_game.cpp libAIEngine.a
 $(CXX) $(CXXFLAGS) $(AIENGINE_INCLUDE) my_game.cpp $(AIENGINE_LIBS) -o my_game
```

**Contract Requirements**:

- Applications MUST link against AIEngine library  
- Applications MUST include OpenGL and windowing library dependencies
- Header include path MUST point to AIEngine/include directory
- Static library linking order MUST place AIEngine before system libraries

## Error Handling Contract

### Exception Safety

```cpp
namespace AIEngine {
    
    // Engine exceptions for recoverable errors
    class EngineException : public std::runtime_error {
    public:
        explicit EngineException(const std::string& message);
    };
    
    class GraphicsException : public EngineException {
    public:
        explicit GraphicsException(const std::string& message);
    };
}
```

**Contract Requirements**:

- Engine MUST NOT throw exceptions during normal operation
- Initialize() MUST return false instead of throwing for recoverable failures
- Exceptions MAY be thrown for programming errors (invalid parameters)  
- Applications SHOULD check return values rather than rely on exception handling
- Resource creation failures MUST be handled gracefully without crashes

## Performance Contract

### Guaranteed Performance Characteristics

- **Frame Rate**: Engine MUST maintain 60fps for scenes with < 100 entities on standard gaming hardware
- **Memory**: Engine MUST NOT allocate heap memory during Update/Render calls after initialization
- **Startup Time**: Initialize() MUST complete within 2 seconds on standard hardware
- **Entity Operations**: AddComponent/GetComponent MUST execute in O(1) time
- **Transform Updates**: Matrix calculations MUST complete within 1ms for 100 entities
- **System Lookup**: GetSystem<T>() MUST execute in O(1) time using optimized type hash lookup
- **System Creation**: CreateSystem() MUST complete within 100ms regardless of system complexity
- **Descriptor Registration**: RegisterSystemDescriptor() MUST complete within 10ms
- **Lifecycle Operations**: Initialize/Start/Stop/Destroy MUST complete within system-specific timeouts
- **System Manager**: Thread-safe operations MUST NOT introduce frame drops under concurrent access

This contract provides a stable interface for learning C++ game development while maintaining professional API design standards with performant, swappable system architecture.
