# Input Subsystem Public Interface Contract

**Purpose**: Define the public interface that game code uses to interact with the input subsystem following engine subsystem architecture  
**Audience**: Game developers using AIEngine  
**Stability**: Public API - breaking changes require major version bump

## Engine Subsystem Interface

```cpp
namespace AIEngine {

// Input Provider Interface (follows engine provider pattern)
class IInputProvider : public ISubsystemProvider {
public:
    virtual ~IInputProvider() = default;
    
    // Input-specific provider interface
    virtual void PollInputEvents() = 0;
    virtual bool IsKeyPressed(int keyCode) const = 0;
    virtual bool IsMouseButtonPressed(int button) const = 0;
    virtual glm::vec2 GetMouseDelta() const = 0;
    virtual void SetRelativeMouseMode(bool enabled) = 0;
    virtual std::vector<InputEvent> GetPendingEvents() = 0;
};

// Input Subsystem Interface (follows engine subsystem pattern)  
class IInputSubsystem : public IEngineSubsystem {
public:
    virtual ~IInputSubsystem() = default;
    
    // Component registration for input events
    template<typename EventType>
    virtual void RegisterHandler(uint32_t componentId, 
                                std::function<void(const EventType&)> handler) = 0;
    
    virtual void UnregisterHandler(uint32_t componentId) = 0;
    
    // Global input configuration
    virtual void SetInputEnabled(bool enabled) = 0;
    virtual bool IsInputEnabled() const = 0;
    virtual void SetMouseSensitivity(float sensitivity) = 0;
    virtual void SetMovementSpeed(float speed) = 0;
    virtual float GetMouseSensitivity() const = 0;
    virtual float GetMovementSpeed() const = 0;
    
    // Type-safe provider access (follows graphics subsystem pattern)
    void SetInputProvider(std::unique_ptr<IInputProvider> provider) {
        SetSubsystemProvider(std::move(provider));
    }
    
    IInputProvider* GetInputProvider() const {
        return static_cast<IInputProvider*>(GetSubsystemProvider());
    }
};

// SDL2 Input Provider (concrete implementation)
class SDL2InputProvider : public IInputProvider {
public:
    explicit SDL2InputProvider();
    
    // ISubsystemProvider implementation (engine lifecycle)
    bool Initialize() override;
    void Start() override;
    void Stop() override;
    void Destroy() override;
    bool IsInitialized() const override { return initialized; }
    bool IsRunning() const override { return running; }
    
    const std::string& GetProviderName() const override {
        static const std::string name = "SDL2 Input Provider";
        return name;
    }
    
    const std::string& GetProviderId() const override {
        static const std::string id = "sdl2-input";
        return id;
    }
    
    // IInputProvider implementation
    void PollInputEvents() override;
    bool IsKeyPressed(int keyCode) const override;
    bool IsMouseButtonPressed(int button) const override;
    glm::vec2 GetMouseDelta() const override;
    void SetRelativeMouseMode(bool enabled) override;
    std::vector<InputEvent> GetPendingEvents() override;
    
private:
    bool initialized = false;
    bool running = false;
    // SDL2-specific input state...
};

// Concrete Input Subsystem (uses provider via composition)
class InputSubsystem : public IInputSubsystem {
public:
    InputSubsystem() = default;
    
    // IEngineSubsystem implementation (delegates to provider)
    bool Initialize() override;
    void Start() override;
    void Stop() override;
    void Destroy() override;
    bool IsInitialized() const override;
    bool IsRunning() const override;
    
    // Provider management (engine subsystem pattern)
    void SetSubsystemProvider(std::unique_ptr<ISubsystemProvider> provider) override;
    ISubsystemProvider* GetSubsystemProvider() const override;
    bool HasProvider() const override;
    EngineSystemDescriptor GetDescriptor() const override;
    const std::string& GetSubsystemName() const override;
    
    // IInputSubsystem implementation
    template<typename EventType>
    void RegisterHandler(uint32_t componentId, 
                        std::function<void(const EventType&)> handler) override;
    void UnregisterHandler(uint32_t componentId) override;
    void SetInputEnabled(bool enabled) override;
    bool IsInputEnabled() const override;
    void SetMouseSensitivity(float sensitivity) override;
    void SetMovementSpeed(float speed) override;
    float GetMouseSensitivity() const override;
    float GetMovementSpeed() const override;
    
private:
    std::unique_ptr<IInputProvider> provider;
    // Event handler storage and dispatch logic...
};

} // namespace AIEngine
```

## Event Type Interface

```cpp
namespace AIEngine {

// Base event interface - all input events derive from this
class InputEvent {
public:
    uint64_t GetTimestamp() const;
    EventType GetType() const;
    
protected:
    InputEvent(EventType type);  // Protected constructor
};

// Movement event contract
class MoveEvent : public InputEvent {
public:
    glm::vec3 GetDirection() const;      // Normalized movement direction
    float GetMagnitude() const;          // Movement strength [0.0, 1.0]
    bool IsActive() const;               // True if movement is ongoing
};

// Rotation event contract  
class RotateEvent : public InputEvent {
public:
    float GetYawDelta() const;           // Horizontal rotation (radians)
    float GetPitchDelta() const;         // Vertical rotation (radians)  
    float GetSensitivity() const;        // Applied sensitivity multiplier
};

} // namespace AIEngine
```

## Component Interface Contract

```cpp
namespace AIEngine {

class InputCameraComponent : public Component {
public:
    // Camera configuration
    void SetMovementSpeed(float unitsPerSecond);
    void SetMouseSensitivity(float degreesPerPixel);
    void SetPitchLimit(float maxPitchRadians);
    
    // Runtime control
    void SetInputEnabled(bool enabled);
    void ResetToInitialTransform();
    
    // Read-only access to camera state
    glm::vec3 GetPosition() const;
    glm::quat GetRotation() const;
    glm::mat4 GetViewMatrix() const;
    
    // Component lifecycle
    void Initialize(SceneNode* parentNode) override;
    void Update(float deltaTime) override;
    void Cleanup() override;
    
private:
    // Access input subsystem via engine system manager
    IInputSubsystem* GetInputSubsystem() {
        return GetEngineSubsystem(IInputSubsystem);
    }
};

} // namespace AIEngine
```

## Contract Guarantees

### Performance Promises

- Input event processing: <1ms latency for event dispatch
- Component updates: O(1) complexity for single camera updates
- Memory allocation: Zero runtime allocation after initialization
- Frame rate impact: <5% overhead for 4 simultaneous input cameras
- System access: O(1) subsystem lookup via GetEngineSubsystem<IInputSubsystem>()

### Behavioral Guarantees  

- **Thread safety**: All public methods are thread-safe via engine system manager
- **Input consistency**: Event ordering preserved across all subscribers
- **Graceful degradation**: System continues functioning if input devices disconnect
- **Clean shutdown**: All resources released properly during subsystem destruction
- **Provider delegation**: All input operations delegate to provider after Initialize() succeeds

### Cross-Platform Promises

- Identical behavior across Windows/macOS/Linux
- Consistent input sensitivity and timing
- Same keyboard/mouse event behavior via SDL2 abstraction

### Engine Integration Guarantees

- **Subsystem lifecycle**: Follows standard Initialize() → Start() → Stop() → Destroy() sequence
- **Provider management**: Provider automatically assigned during EngineSystemDescriptor creation
- **System registration**: Automatic registration in EngineSystemManager during creation
- **Type safety**: Template-based component access with compile-time type checking

## Usage Contract

### Required Engine Setup Sequence

1. Create InputSystemDescriptor during engine initialization
2. Register descriptor with EngineSystemManager
3. Create InputSubsystem via descriptor (auto-assigns provider)
4. Engine calls Initialize() on all subsystems
5. Engine calls Start() before main loop begins
6. InputCameraComponents automatically register during their Initialize()
7. Input events begin processing once both subsystem and components are active

### Engine System Setup

```cpp
// During engine initialization
auto& systemManager = engine.GetSystemManager();

// Create input system descriptor with provider type information
auto inputDesc = EngineSystemDescriptor::Create<
    IInputSubsystem,
    InputSubsystem, 
    SDL2InputProvider>("SDL2 Input System");

// Register descriptor
systemManager.RegisterSystemDescriptor(inputDesc);

// Create input subsystem (provider auto-assigned)
auto* inputSystem = inputDesc.Create<IInputSubsystem>();

// Engine handles Initialize() and Start() automatically
```

### Component Integration

```cpp
// Component accesses input subsystem via engine system manager
void InputCameraComponent::Initialize(SceneNode* parentNode) {
    auto* inputSystem = GetEngineSubsystem(IInputSubsystem);
    if (inputSystem && inputSystem->IsRunning()) {
        // Register event handlers
        inputSystem->RegisterHandler<MoveEvent>(GetId(), moveHandler);
        inputSystem->RegisterHandler<RotateEvent>(GetId(), rotateHandler);
    }
}
```

### Error Handling

- Invalid configurations (negative speed, etc.) → Use default values + log warning
- Event registration failures → Log error, component still functions (no input response)
- Input device disconnection → Continue operation, resume when device reconnects
- Subsystem initialization failure → Engine reports initialization error
- Provider creation failure → Subsystem Initialize() returns false

### Resource Management

- Components automatically unregister from InputSubsystem during cleanup
- InputSubsystem lifetime managed by EngineSystemManager
- Provider lifetime managed by InputSubsystem via provider pattern
- No manual memory management required by user code
- Engine guarantees proper shutdown sequence: Stop() → Destroy() for all subsystems
