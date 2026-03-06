# Data Model: Input-Controlled Camera Component

**Feature**: Input-Controlled Camera Component  
**Date**: March 4, 2026  
**Status**: Phase 1 Design Output

## Core Entities

### InputEvent (Base)

**Purpose**: Base event class for all input abstractions  
**Key Attributes**:

- `timestamp`: Event creation time for temporal ordering
- `source`: Input source identifier (future extensibility)  
- `type`: Event type enumeration

**Relationships**: Base class for MoveEvent, RotateEvent

### MoveEvent

**Purpose**: Semantic movement command derived from WASD input  
**Key Attributes**:

- `direction`: 3D normalized vector (glm::vec3) indicating movement direction  
- `magnitude`: Movement strength/speed multiplier (float, typically 1.0)
- `isActive`: Boolean indicating ongoing movement state

**Validation**:

- Direction vector must be valid (not NaN/infinite)
- Magnitude must be non-negative
- Direction should be normalized for consistent behavior

**State Transitions**: Created when movement keys pressed → Active during hold → Destroyed on release

### RotateEvent

**Purpose**: Camera orientation change derived from mouse movement  
**Key Attributes**:

- `yaw`: Horizontal rotation delta in radians (float)
- `pitch`: Vertical rotation delta in radians (float)
- `sensitivity`: Applied sensitivity multiplier (float)

**Validation**:

- Yaw/pitch values must be finite numbers
- Sensitivity must be positive
- Pitch should respect camera limits (±89°)

**State Transitions**: Created on mouse movement → Applied immediately → Single-use event

### IInputProvider (Engine Subsystem Provider)

**Purpose**: Provider interface for input hardware abstraction (follows engine provider pattern)  
**Key Attributes**:

- `inputStates`: Current input device states (keyboard/mouse)
- `eventQueue`: Pending events awaiting dispatch
- `isEnabled`: Global enable/disable flag (bool)

**Relationships**:

- Inherits from ISubsystemProvider (engine base)
- Implemented by SDL2InputProvider
- Managed by InputSubsystem

**Provider Interface Methods**:

- `PollInputEvents()`: Gather input from hardware
- `TranslateToEvents()`: Convert raw input to semantic events
- `GetCurrentInputState()`: Access current input device states

### IInputSubsystem (Engine Subsystem Interface)

**Purpose**: Public interface for input event broadcasting (follows engine subsystem pattern)
**Key Attributes**:

- `eventSubscribers`: Map of event type to subscriber lists
- `mouseSensitivity`: Global mouse sensitivity setting
- `movementSpeed`: Global movement speed setting

**Relationships**:

- Inherits from IEngineSubsystem (engine base)
- Uses IInputProvider for hardware abstraction
- Provides registration interface for components

**Subsystem Interface Methods**:

- `RegisterHandler<EventType>()`: Component subscription
- `UnregisterHandler()`: Component cleanup
- `SetMouseSensitivity()`: Global input configuration
- `SetInputEnabled()`: Global input toggle

### InputSubsystem (Concrete Implementation)

**Purpose**: Concrete input subsystem implementing event broadcasting  
**Key Attributes**:

- `provider`: Unique pointer to IInputProvider implementation
- `subscribers`: Active event handler registrations
- `configSettings`: Current input configuration

**Relationships**:

- Implements IInputSubsystem interface
- Delegates hardware operations to IInputProvider
- Managed by EngineSystemManager

**Implementation Details**:

- Follows engine subsystem lifecycle (Initialize/Start/Stop/Destroy)
- Provider assignment handled by EngineSystemDescriptor
- Thread-safe event dispatch for component callbacks

### InputCameraComponent  

**Purpose**: Camera component that responds to input events for player control  
**Key Attributes**:

- `position`: Camera world position (glm::vec3)
- `rotation`: Camera orientation quaternion (glm::quat)  
- `movementSpeed`: Units per second for movement (float, default 5.0)
- `mouseSensitivity`: Degrees per pixel (float, default 0.1)
- `pitchLimit`: Maximum pitch angle in radians (float, default ±89°)
- `isInputEnabled`: Toggle for input responsiveness (bool)
- `inputSubsystem`: Reference to engine's input subsystem (IInputSubsystem*)

**Validation**:

- Position coordinates must be finite
- Movement speed must be positive  
- Mouse sensitivity must be positive
- Pitch limit must be between 0 and π/2

**Relationships**:

- Inherits from base Component interface
- Registers with IInputSubsystem during component initialization
- Uses TransformComponent for scene graph integration
- Accesses input subsystem via GetEngineSubsystem<IInputSubsystem>()

### SDL2InputProvider (Concrete Provider)

**Purpose**: SDL2-based input hardware provider implementation
**Key Attributes**:

- `sdlWindow`: Reference to SDL window for input capture
- `keyboardState`: Current SDL keyboard state
- `mouseState`: Current SDL mouse state
- `eventBuffer`: Pre-allocated event storage for performance

**Relationships**:

- Implements IInputProvider interface
- Integrates with existing SDL2 window system
- Managed by InputSubsystem via provider pattern

## Entity Relationships

```text
EngineSystemManager (1) -----> (*) EngineSystemDescriptor
    |                               |
    |                               +-- InputSystemDescriptor
    |                               
    +-- manages --> IInputSubsystem (interface)
                         |
                         +-- implements --> InputSubsystem (concrete)
                         |                       |
                         |                       +-- uses --> IInputProvider (interface)
                         |                                       |
                         |                                       +-- implements --> SDL2InputProvider
                         |
                         +-- creates/dispatches --> (*) InputEvent
                                                        |
                                                        +-- MoveEvent  
                                                        +-- RotateEvent
                         |
                         +-- notifies --> (*) InputCameraComponent
                                             |
                                             +-- extends --> Component (base)
                                             |
                                             +-- uses --> TransformComponent  
```

## Data Flow

1. **Engine Integration**: EngineSystemManager → InputSystemDescriptor → InputSubsystem (engine lifecycle)
2. **Provider Integration**: InputSubsystem → SDL2InputProvider → SDL2 hardware (provider pattern)
3. **Input capture**: SDL2 hardware → SDL2InputProvider → InputSubsystem (raw device input)
4. **Event creation**: InputSubsystem → InputEvent subclasses (semantic translation)
5. **Component registration**: InputCameraComponent → InputSubsystem (subscribe to events)
6. **Event dispatch**: InputSubsystem → InputCameraComponent (observer notification)
7. **Transform update**: InputCameraComponent → TransformComponent (position/rotation changes)
8. **Render integration**: TransformComponent → Scene graph → Rendering pipeline

## System Integration Points

**Engine System Registration**:

```cpp
// Engine initialization - create input system descriptor
auto inputDesc = EngineSystemDescriptor::Create<
    IInputSubsystem, 
    InputSubsystem,
    SDL2InputProvider>("SDL2 Input");
    
// Register with engine system manager
engine.GetSystemManager().RegisterSystemDescriptor(inputDesc);

// Create and initialize input subsystem
auto* inputSystem = inputDesc.Create<IInputSubsystem>();
```

**Component Integration**:

```cpp
// Component uses engine subsystem access
IInputSubsystem* inputSystem = GetEngineSubsystem(IInputSubsystem);
inputSystem->RegisterHandler<MoveEvent>(componentId, moveHandler);
```

## Memory Layout Considerations

**Cache Efficiency**:

- Event types designed as small POD structures for fast copying
- Component data arranged for sequential access during updates
- Subsystem follows engine's system manager memory layout patterns

**Allocation Strategy**:

- Pre-allocated event pools to avoid runtime allocation
- Fixed-size subscription lists for predictable memory usage
- Component arrays for cache-friendly iteration
- Provider memory managed by engine subsystem lifecycle

**Lifetime Management**:

- Events: Short-lived, immediate processing and destruction  
- Components: Scene-lifetime, managed by component system
- InputSubsystem: Engine-lifetime, managed by EngineSystemManager
- SDL2InputProvider: Provider-lifetime, managed by subsystem via provider pattern

**Engine Integration**:

- Subsystem registration occurs during engine initialization
- Provider instantiation handled automatically by EngineSystemDescriptor
- Component access via GetEngineSubsystem<IInputSubsystem>() for O(1) lookup
- Thread-safe access guaranteed by EngineSystemManager
