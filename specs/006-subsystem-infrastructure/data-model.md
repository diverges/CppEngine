# Data Model: Core Engine Subsystem Infrastructure

**Phase**: 1 - Design & Contracts  
**Date**: March 4, 2026  
**Feature**: [spec.md](spec.md)

## Core Entities

### IEngineSubsystem (Base Interface)

**Purpose**: Abstract base class defining the contract for all engine subsystems

**Core Attributes**:

- Lifecycle state: `{Created, Initialized, Running, Stopped, Destroyed}`
- Associated provider: `std::unique_ptr<ISubsystemProvider>`
- System descriptor: `EngineSystemDescriptor`
- Subsystem name: `std::string`

**Key Relationships**:

- **Owns** one ISubsystemProvider instance (composition)
- **Managed by** EngineSystemManager (aggregation)
- **Created from** EngineSystemDescriptor (factory pattern)

**State Transitions**:

```text
Created → Initialize() → Initialized → Start() → Running
Running → Stop() →  Destroy() → Destroyed
```

**Validation Rules**:

- Cannot call Start() without successful Initialize()
- Cannot call operations without valid provider
- Must be in Running state to process engine operations

### ISubsystemProvider (Base Interface)

**Purpose**: Abstract base class for platform-specific subsystem implementations

**Core Attributes**:

- Provider state: `{Created, Initialized, Running, Stopped, Destroyed}`
- Provider name: `std::string` (human-readable identifier)
- Provider ID: `std::string` (unique technical identifier)

**Key Relationships**:

- **Owned by** single IEngineSubsystem instance
- **Implements** platform-specific behavior for subsystem

**State Management**:

- Lifecycle mirrors owning subsystem
- Provider failure causes subsystem failure
- Provider initialization must succeed before subsystem can start

### EngineSystemDescriptor (Value Type)

**Purpose**: Factory and metadata container for subsystem creation

**Core Attributes**:

- Interface hash: `size_t` (typeid-based unique identifier)
- Provider type hash: `size_t` (provider class identification)
- System name: `std::string` (human-readable name)
- Interface type name: `std::string` (for debugging)
- Provider type name: `std::string` (for debugging)
- Implementation ID: `std::string` (provider-specific identifier)
- System ID: `uint32_t` (sequentially assigned unique ID)
- Creation function: `std::function<std::unique_ptr<void>()>` (type-erased factory)

**Key Relationships**:

- **Creates** IEngineSubsystem instances through factory pattern
- **Registered with** EngineSystemManager
- **Identifies** subsystem types through hash values

**Creation Process**:

```cpp
// Template-based creation
auto descriptor = EngineSystemDescriptor::Create<IGraphicsSubsystem, GraphicsSubsystem, OpenGLProvider>("Graphics");
auto subsystem = descriptor.CreateSystem<IGraphicsSubsystem>();
```

**Validation Rules**:

- Interface hash must be unique per registration
- Provider type hash must be consistent with template parameters
- Creation function must successfully create valid subsystem + provider pair

### EngineSystemManager (Singleton)

**Purpose**: Central registry and lifecycle manager for all engine subsystems

**Core State**:

- Active subsystems: `std::unordered_map<size_t, std::unique_ptr<SystemEntry>>`
- Registered descriptors: `std::unordered_map<size_t, std::vector<EngineSystemDescriptor>>`
- System mutex: `std::mutex` (not used in single-threaded version)

**SystemEntry Structure**:

- Subsystem instance: `std::unique_ptr<IEngineSubsystem>`
- Descriptor: `EngineSystemDescriptor`
- Initialization state: `bool initialized`
- Running state: `bool running`

**Key Relationships**:

- **Manages lifecycle** of all active subsystems
- **Stores registry** of available descriptors
- **Provides O(1) access** to subsystems by type hash
- **Owned by** Engine class instance

**Operations**:

- Registration: Add descriptors to registry
- Creation: Instantiate subsystems from descriptors
- Lifecycle: Initialize/Start/Stop/Destroy all systems
- Access: Type-safe retrieval of active subsystems

## Data Flow Patterns

### Registration Flow

```text
1. Game/Engine code calls RegisterSystemDescriptor()
2. Descriptor stored in registry by interface hash
3. Single descriptor per interface enforced
4. Registration complete - subsystem available for creation
```

### Creation Flow

```text
1. Engine calls CreateSystem<InterfaceType>(descriptor)
2. Descriptor creates subsystem + provider instances
3. Subsystem linked to provider via SetSubsystemProvider()
4. SystemEntry created and stored in active systems map
5. Template-based access enabled via GetSystem<T>()
```

### Lifecycle Flow

```text
Engine::Initialize()
├─→ EngineSystemManager::InitializeAllSystems()
│   ├─→ For each active subsystem (in registration order):
│   │   ├─→ subsystem.Initialize()
│   │   ├─→ provider.Initialize() (delegated)
│   │   └─→ Mark as initialized or fail-fast
│   └─→ All subsystems initialized or engine init fails

Engine::Shutdown()
├─→ EngineSystemManager::DestroyAllSystems()
│   ├─→ For each active subsystem (reverse order):
│   │   ├─→ subsystem.Stop()
│   │   ├─→ subsystem.Destroy() 
│   │   └─→ Release memory via unique_ptr
│   └─→ Clear all active systems
```

### Access Flow

```text
Game code: auto* graphics = GetEngineSubsystem(IGraphicsSubsystem);
├─→ Template expands to: EngineSystemManager::GetInstance().GetSystem<IGraphicsSubsystem>()
├─→ Hash lookup: typeid(IGraphicsSubsystem).hash_code()
├─→ O(1) map access: activeSubsystemsByInterface[hash]
└─→ Return typed pointer or nullptr
```

## Memory Management

### Ownership Model

- **Engine** owns **EngineSystemManager** (unique_ptr)
- **EngineSystemManager** owns **SystemEntry** instances (unique_ptr)
- **SystemEntry** owns **IEngineSubsystem** instances (unique_ptr)
- **IEngineSubsystem** owns **ISubsystemProvider** instances (unique_ptr)

### Lifetime Guarantees

- Subsystems live for entire Engine lifetime after creation
- Providers live for entire Subsystem lifetime after assignment
- Descriptors are value types, copied during registration
- No shared ownership - clear ownership hierarchy

### Resource Cleanup

- Automatic via RAII and smart pointers
- Deterministic destruction order (reverse initialization order)
- Provider cleanup happens automatically during subsystem destruction
- No manual memory management required
