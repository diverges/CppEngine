# Engine Subsystem Infrastructure API Contract

**Purpose**: Defines the public interface contract for the core engine subsystem infrastructure  
**Version**: 1.0.0  
**Stability**: Foundation - breaking changes require major version bump  

## Core Subsystem Interfaces

### IEngineSubsystem Base Interface

```cpp
namespace AIEngine {
    
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
}
```

### EngineSystemDescriptor Creation and Identification

```cpp
namespace AIEngine {
    
    class EngineSystemDescriptor {
    public:
        // Create descriptor with subsystem and provider types
        template<typename SubsystemInterface, typename SubsystemImpl, typename ProviderType>
        static EngineSystemDescriptor Create(const std::string& systemName);
        
        // System creation using stored creation function
        template<typename SubsystemInterface>
        std::unique_ptr<SubsystemInterface> CreateSystem() const;
        
        // Type identification
        size_t GetInterfaceHash() const;
        size_t GetProviderTypeHash() const;
        uint32_t GetSystemId() const;
        
        // Human-readable identification
        const std::string& GetSystemName() const;
        const std::string& GetInterfaceTypeName() const;
        const std::string& GetProviderTypeName() const;
        const std::string& GetImplementationId() const;
        
        // Comparison
        bool operator==(const EngineSystemDescriptor& other) const;
        
    private:
        EngineSystemDescriptor(/* internal constructor parameters */);
        // Implementation details hidden
    };
}
```

### EngineSystemManager (Singleton Registry)

```cpp
namespace AIEngine {
    
    class EngineSystemManager {
    public:
        // Singleton access
        static EngineSystemManager& GetInstance();
        
        // Descriptor Registration
        void RegisterSystemDescriptor(const EngineSystemDescriptor& descriptor);
        void UnregisterSystemDescriptor(const EngineSystemDescriptor& descriptor);
        
        // System Creation and Access (O(1) performance)
        template<typename SystemInterface>
        SystemInterface* CreateSystem(const EngineSystemDescriptor& descriptor);
        
        template<typename SystemInterface>
        SystemInterface* GetSystem();
        
        template<typename SystemInterface>
        const SystemInterface* GetSystem() const;
        
        template<typename SystemInterface>
        bool HasSystem() const;
        
        template<typename SystemInterface>
        void DestroySystem();
        
        // Lifecycle Management (called by Engine class)
        bool InitializeAllSystems();
        void StartAllSystems();
        void StopAllSystems();
        void DestroyAllSystems();
        
        // System Enumeration
        std::vector<EngineSystemDescriptor> GetRegisteredDescriptors() const;
        std::vector<IEngineSubsystem*> GetActiveSubsystems();
        std::vector<const IEngineSubsystem*> GetActiveSubsystems() const;
        
    private:
        EngineSystemManager() = default;
        ~EngineSystemManager() = default;
        
        // Non-copyable, non-movable singleton
        EngineSystemManager(const EngineSystemManager&) = delete;
        EngineSystemManager& operator=(const EngineSystemManager&) = delete;
    };
    
    // Convenience macro for subsystem access
    #define GetEngineSubsystem(SubsystemType) \
        EngineSystemManager::GetInstance().GetSystem<SubsystemType>()
}
```

## Usage Contract for Engine Developers

### Subsystem Development Pattern

```cpp
namespace AIEngine {
    
    // 1. Define subsystem interface extending IEngineSubsystem
    class IMySubsystem : public IEngineSubsystem {
    public:
        // Subsystem-specific interface methods
        virtual void DoSubsystemOperation() = 0;
        
        // Type-safe provider access
        void SetMyProvider(std::unique_ptr<IMyProvider> provider) {
            SetSubsystemProvider(std::move(provider));
        }
        
        IMyProvider* GetMyProvider() const {
            return static_cast<IMyProvider*>(GetSubsystemProvider());
        }
    };
    
    // 2. Define provider interface extending ISubsystemProvider
    class IMyProvider : public ISubsystemProvider {
    public:
        // Provider-specific implementation methods
        virtual void DoProviderOperation() = 0;
    };
    
    // 3. Implement concrete subsystem (delegates to provider)
    class MySubsystem : public IMySubsystem {
    public:
        bool Initialize() override {
            return provider && provider->Initialize();
        }
        
        void DoSubsystemOperation() override {
            provider->DoProviderOperation();
        }
        
        // ... other IEngineSubsystem methods
    private:
        std::unique_ptr<IMyProvider> provider;
    };
    
    // 4. Implement concrete provider
    class ConcreteMyProvider : public IMyProvider {
    public:
        void DoProviderOperation() override {
            // Actual implementation
        }
        
        // ... other ISubsystemProvider methods
    };
    
    // 5. Registration (usually in engine initialization)
    auto descriptor = EngineSystemDescriptor::Create<IMySubsystem, MySubsystem, ConcreteMyProvider>("My System");
    EngineSystemManager::GetInstance().RegisterSystemDescriptor(descriptor);
    
    // 6. Creation and usage
    auto* subsystem = EngineSystemManager::GetInstance().CreateSystem<IMySubsystem>(descriptor);
    auto* mySystem = GetEngineSubsystem(IMySubsystem);
}
```

## Contract Requirements

### Lifecycle Contract

- **Registration** MUST occur before Engine::Initialize()
- **CreateSystem()** MUST be called before accessing subsystems
- **Initialize()** MUST succeed before calling Start()
- **Stop()** MUST be called before Destroy()
- **Subsystem lookup** is only valid after successful creation

### Error Handling Contract

- **Initialize()** MUST return false on any failure
- **Failed initialization** causes immediate Engine::Initialize() failure
- **Error messages** MUST include subsystem name and failure reason
- **Provider assignment** failure causes subsystem Initialize() to return false

### Performance Contract

- **GetSystem<T>()** MUST execute in O(1) time
- **Registration** MUST complete in O(1) time
- **Lookup operations** MUST not allocate memory
- **Type hash** MUST be consistent across program runs

### Thread Safety Contract

- **Single main thread** - all operations must occur on main thread
- **No concurrent access** - engine is single-threaded by design
- **Registration thread** must be same as access thread

### Memory Management Contract

- **RAII ownership** - smart pointers manage all lifetimes
- **Automatic cleanup** during Engine destruction
- **No memory leaks** - all resources cleaned up deterministically
- **Provider ownership** - subsystem owns its provider exclusively

## Breaking Change Policy

Changes that require interface modifications or behavior changes MUST:

1. Update version number according to semantic versioning
2. Provide migration guide for existing subsystems
3. Maintain backward compatibility for one major version where possible
4. Update all example code and documentation
