# Game Integration Contract

**Purpose**: Define how game developers integrate input-controlled cameras into their projects using engine subsystem architecture  
**Audience**: Users of AIEngine creating games/applications  
**Stability**: Public interface - maintained across minor versions

## Basic Usage Pattern

### Engine Setup (Required)

```cpp
#include <AIEngine/AIEngine.hpp>
#include <AIEngine/input/InputSubsystem.hpp>
#include <AIEngine/components/InputCameraComponent.hpp>

// In engine initialization (before game logic)
int main() {
    AIEngine::Engine engine;
    auto& systemManager = engine.GetSystemManager();
    
    // Create and register input subsystem descriptor
    auto inputDesc = AIEngine::EngineSystemDescriptor::Create<
        AIEngine::IInputSubsystem,
        AIEngine::InputSubsystem,
        AIEngine::SDL2InputProvider>("SDL2 Input");
    
    systemManager.RegisterSystemDescriptor(inputDesc);
    
    // Create input subsystem (provider auto-assigned)
    auto* inputSystem = inputDesc.Create<AIEngine::IInputSubsystem>();
    
    // Engine initialization handles subsystem lifecycle
    if (!engine.Initialize()) {
        return -1;  // Subsystem initialization failed
    }
    
    // Continue with game setup...
}
```

### Component Usage

```cpp
// In game initialization
auto sceneRoot = engine.GetScene().GetRootNode();
auto cameraNode = sceneRoot->CreateChild("MainCamera");

// Add input-controlled camera component
auto inputCamera = cameraNode->AddComponent<InputCameraComponent>();
inputCamera->SetMovementSpeed(5.0f);       // 5 units per second
inputCamera->SetMouseSensitivity(0.1f);    // 0.1 degrees per pixel

// Component automatically registers with input subsystem during Initialize()
// No manual registration required
```

### Advanced Configuration  

```cpp
// Access input subsystem for global configuration
auto* inputSystem = GetEngineSubsystem(AIEngine::IInputSubsystem);
if (inputSystem && inputSystem->IsRunning()) {
    inputSystem->SetMouseSensitivity(0.2f);    // Global setting
    inputSystem->SetMovementSpeed(10.0f);      // Global default
    inputSystem->SetInputEnabled(true);        // Global enable/disable
}

// Per-component overrides still supported
inputCamera->SetMouseSensitivity(0.05f);  // This camera only
inputCamera->SetInputEnabled(false);      // Disable just this camera
```

## Integration Scenarios

### Scenario 1: First-Person Game

**Use Case**: Single player camera for FPS-style navigation  
**Components Required**: InputCameraComponent only  
**Engine Setup**: Input subsystem registered during engine initialization  
**Game Setup**: Attach component to main camera scene node  
**Expected Behavior**: WASD movement + mouse look, standard FPS controls

### Scenario 2: Splitscreen Multiplayer  

**Use Case**: Multiple players each controlling separate cameras  
**Components Required**: Multiple InputCameraComponent instances  
**Engine Setup**: Single input subsystem handles all cameras  
**Game Setup**: One component per player scene node, focus management via SetInputEnabled()  
**Expected Behavior**: Components individually enabled/disabled, shared input subsystem

### Scenario 3: Debug/Development Camera

**Use Case**: Developer camera for scene inspection  
**Components Required**: InputCameraComponent + debug UI  
**Engine Setup**: Input subsystem provides global input control  
**Game Setup**: Toggle-able component activation alongside main game camera  
**Expected Behavior**: Switch between game camera and free-roaming debug camera via input enable/disable

## Event Flow Contract

### Input Processing Pipeline

```text
User Input → SDL2 → SDL2InputProvider → InputSubsystem → MoveEvent/RotateEvent → InputCameraComponent → Transform Changes
```

### Engine Integration Pipeline

```text
EngineInitialization → SystemDescriptor → InputSubsystem → Provider Assignment → Component Registration → Event Processing
```

### Timing Guarantees

- **Subsystem registration**: Occurs during engine initialization  
- **Provider assignment**: Automatic during EngineSystemDescriptor.Create()
- **Subsystem initialization**: Engine calls Initialize() before Start()
- **Component registration**: Occurs during component Initialize() call
- **Event processing**: Every frame during engine Update() phase  
- **Transform updates**: Applied immediately after event processing
- **Rendering integration**: Transform changes visible next frame

## Configuration Contract

### Required Engine Setup

```cpp
// Engine initialization with input subsystem
AIEngine::Engine engine;
auto& systemManager = engine.GetSystemManager();

// Input subsystem must be registered before engine.Initialize()
auto inputDesc = AIEngine::EngineSystemDescriptor::Create<
    AIEngine::IInputSubsystem,
    AIEngine::InputSubsystem,
    AIEngine::SDL2InputProvider>("SDL2 Input");
    
systemManager.RegisterSystemDescriptor(inputDesc);
auto* inputSystem = inputDesc.Create<AIEngine::IInputSubsystem>();

// Initialize engine (initializes all registered subsystems)
engine.Initialize();  // Sets up input subsystem and provider

// Window creation occurs during engine initialization
// Input now active and ready for component registration
```

### Component System Integration

```cpp  
// Global input settings (affects all input cameras)
auto* inputSystem = GetEngineSubsystem(AIEngine::IInputSubsystem);
if (inputSystem && inputSystem->IsRunning()) {
    inputSystem->SetMouseSensitivity(0.2f);    // Override default
    inputSystem->SetMovementSpeed(10.0f);      // Override default
}

// Per-component overrides (component-specific)
inputCamera->SetMouseSensitivity(0.05f);  // This camera only
inputCamera->SetPitchLimit(glm::radians(45.0f)); // Component setting
```

## Lifecycle Contract

### Subsystem Creation (Engine-Managed)

1. **Descriptor Creation**: EngineSystemDescriptor created with type information
2. **Registration**: Descriptor registered with EngineSystemManager  
3. **System Creation**: descriptor.Create<IInputSubsystem>() creates subsystem + provider
4. **Initialization**: Engine calls Initialize() on all subsystems
5. **Activation**: Engine calls Start() before main game loop

### Component Creation  

1. **Construction**: InputCameraComponent created via AddComponent<>()
2. **Registration**: Component automatically accesses input subsystem via GetEngineSubsystem()  
3. **Event Subscription**: Component registers event handlers during Initialize()
4. **Activation**: Input processing begins immediately if subsystem is running

### Component Destruction  

1. **Deactivation**: Component stops receiving input events
2. **Unregistration**: Automatic cleanup from InputSubsystem via component ID  
3. **Resource Release**: All event subscriptions cleaned up automatically

### Subsystem Destruction (Engine-Managed)

1. **Deactivation**: Engine calls Stop() on all subsystems
2. **Cleanup**: Engine calls Destroy() on all subsystems  
3. **Resource Release**: Provider destruction handled by subsystem
4. **System Cleanup**: EngineSystemManager handles subsystem destruction

### Scene Integration

- **Transform synchronization**: Changes apply to component's SceneNode transform
- **Camera matrix updates**: View matrix automatically updated for rendering
- **Coordinate space**: All movement relative to camera's current orientation
- **Engine integration**: No additional setup required beyond component creation

## Error Handling Contract

### Engine System Failures

**Input subsystem registration missing** → Engine initialization fails, clear error message  
**Provider creation failure** → Subsystem Initialize() returns false, engine reports error  
**SDL2 initialization failure** → Provider Initialize() returns false, fallback to no-input mode  
**Input device disconnection** → Provider handles gracefully, resumes on reconnection

### Component Failures

**Component initialization without input subsystem** → Component functions but no input response, log warning  
**Invalid component configurations** → Clamp to valid values + log warning  
**Event registration failure** → Component continues running, logs error, no input response  
**Subsystem access failure** → GetEngineSubsystem() returns nullptr, component handles gracefully

### Runtime Failures  

**Input subsystem stops during runtime** → Components continue running, input disabled until restart  
**Event queue overflow** → Oldest events discarded, log performance warning  
**Component update errors** → Skip frame, attempt recovery on next update
**Provider failure during operation** → Subsystem attempts provider restart, logs error

### Recovery Behavior

- All error conditions attempt graceful degradation
- Game continues running even with input subsystem failures
- Components function normally except for input response
- Engine subsystem restart mechanisms available for catastrophic failures
- Debug builds provide detailed subsystem state information
