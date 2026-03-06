# Quickstart: Input-Controlled Camera Component

**Goal**: Get a simple input-controlled camera working in 5 minutes using engine subsystem architecture  
**Prerequisites**: AIEngine project setup and basic C++ knowledge  
**Result**: WASD + mouse camera navigation in your application

## Step 1: Engine Subsystem Setup (2 minutes)

### Include Headers

```cpp
#include <AIEngine/AIEngine.hpp>
#include <AIEngine/input/InputSubsystem.hpp>
#include <AIEngine/components/InputCameraComponent.hpp>
```

### Initialize Engine + Input Subsystem  

```cpp
int main() {
    // Create engine instance
    AIEngine::Engine engine;
    auto& systemManager = engine.GetSystemManager();
    
    // Create input subsystem descriptor
    auto inputDesc = AIEngine::EngineSystemDescriptor::Create<
        AIEngine::IInputSubsystem,
        AIEngine::InputSubsystem,
        AIEngine::SDL2InputProvider>("SDL2 Input System");
    
    // Register input subsystem with engine
    systemManager.RegisterSystemDescriptor(inputDesc);
    auto* inputSystem = inputDesc.Create<AIEngine::IInputSubsystem>();
    
    // Initialize engine (sets up input subsystem + window)
    if (!engine.Initialize()) {
        return -1; // Input subsystem initialization failed
    }
    
    return 0; // Continue to step 2...
}
```

## Step 2: Add Input Camera (1 minute)

### Create Camera Scene Node

```cpp
// Get scene and create camera node
auto* sceneGraph = engine.GetSceneGraph();
auto* cameraNode = sceneGraph->CreateNode();
sceneGraph->GetRootNode()->AddChild(cameraNode);

// Position camera in 3D space
auto& transform = cameraNode->AddComponent<AIEngine::TransformComponent>();
transform.position = glm::vec3(0, 0, 5);
```

### Attach Input Component  

```cpp
// Add input-controlled camera component
auto& inputCamera = cameraNode->AddComponent<AIEngine::InputCameraComponent>();

// Component automatically accesses input subsystem via GetEngineSubsystem()
// No manual registration required!

// Optional: customize settings
inputCamera.SetMovementSpeed(8.0f);      // Faster movement  
inputCamera.SetMouseSensitivity(0.15f);  // More sensitive mouse
```

## Step 3: Start Engine Systems (30 seconds)

### Engine Lifecycle

```cpp
// Start all subsystems (including input)
systemManager.StartAllSystems();

// Verify input subsystem is running
auto* inputSystem = GetEngineSubsystem(AIEngine::IInputSubsystem);
if (!inputSystem || !inputSystem->IsRunning()) {
    std::cerr << "Input subsystem failed to start!" << std::endl;
    return -1;
}

std::cout << "Input subsystem ready: " 
          << inputSystem->GetInputProvider()->GetProviderName() << std::endl;
```

## Step 4: Create Test Scene (30 seconds)

### Add Visible Objects  

```cpp
// Create some objects to navigate around
auto* cubeNode = sceneGraph->CreateNode();
sceneGraph->GetRootNode()->AddChild(cubeNode);

auto& cubeTransform = cubeNode->AddComponent<AIEngine::TransformComponent>();
cubeTransform.position = glm::vec3(2, 0, 0);
cubeNode->AddComponent<AIEngine::RenderComponent>(); // Make it visible

auto* groundNode = sceneGraph->CreateNode();
sceneGraph->GetRootNode()->AddChild(groundNode);

auto& groundTransform = groundNode->AddComponent<AIEngine::TransformComponent>();
groundTransform.position = glm::vec3(0, -1, 0);
groundNode->AddComponent<AIEngine::RenderComponent>();
```

## Step 5: Run the Application (1 minute)

### Main Loop

```cpp
// Game loop
while (!engine.ShouldClose()) {
    engine.Update();          // Updates input subsystem and camera components
    engine.Render();          // Draws scene from camera view  
}

// Clean shutdown (engine handles subsystem cleanup)
systemManager.StopAllSystems();
engine.Shutdown();
systemManager.DestroyAllSystems();
return 0;
```

### Build and Test

```bash
# Build your application 
make debug

# Run and test controls
./bin/debug/your-app

# Test controls:
# WASD - Move around
# Right-click + mouse - Look around  
# ESC - Quit
```

## Expected Result

You should now have:

- ✅ **WASD movement**: Forward/back/left/right navigation through scene
- ✅ **Mouse look**: Right-click and drag to rotate camera view  
- ✅ **Smooth control**: Responsive input with consistent movement speed
- ✅ **First-person**: Camera movement relative to current facing direction

## Troubleshooting

### Common Issues

**"Input subsystem failed to start"**

- ✓ Check EngineSystemDescriptor was registered before engine.Initialize()
- ✓ Verify SDL2 libraries are properly linked and available
- ✓ Ensure descriptor.Create<IInputSubsystem>() was called after registration
- ✓ Check engine initialization logs for subsystem-specific errors

**"Camera doesn't move"**

- ✓ Verify input subsystem is running: inputSystem->IsRunning()
- ✓ Check component registered successfully during Initialize()
- ✓ Ensure GetEngineSubsystem<IInputSubsystem>() returns valid pointer
- ✓ Verify engine main loop is calling Update() each frame

**"Mouse rotation not working"**  

- ✓ Try right-click and drag (not just mouse movement)
- ✓ Check SDL2 window has proper input focus
- ✓ Verify mouse sensitivity > 0 via inputSystem->GetMouseSensitivity()
- ✓ Check provider initialization: inputSystem->GetInputProvider()->IsInitialized()

**"Movement too fast/slow"**

- ✓ Adjust SetMovementSpeed() value (try 1.0 to 10.0 range)
- ✓ Check frame rate - movement should be frame-rate independent
- ✓ Verify engine deltaTime is being passed to Update() calls

**"Camera goes upside-down"**  

- ✓ This is expected behavior - pitch limits prevent gimbal lock
- ✓ Vertical look automatically limited to ±89 degrees
- ✓ Use SetPitchLimit() to customize vertical rotation range

## Next Steps

### Multiple Cameras  

```cpp
// Create second camera for splitscreen
auto* camera2Node = sceneGraph->CreateNode();
sceneGraph->GetRootNode()->AddChild(camera2Node);
auto& inputCamera2 = camera2Node->AddComponent<InputCameraComponent>();

// Control which camera gets input
inputCamera2.SetInputEnabled(false);  // Disable player 2 initially
// Switch based on game logic
```

### Global Input Configuration

```cpp
// Access input subsystem for engine-wide settings
auto* inputSystem = GetEngineSubsystem(IInputSubsystem);
if (inputSystem) {
    inputSystem->SetMouseSensitivity(0.2f);     // Global sensitivity
    inputSystem->SetMovementSpeed(15.0f);       // Global speed
    inputSystem->SetInputEnabled(false);        // Disable all input (pause menu)
}
```

### Custom Input Providers

```cpp
// Create custom provider for game controllers
auto gamepadDesc = EngineSystemDescriptor::Create<
    IInputSubsystem,
    InputSubsystem, 
    GamepadInputProvider>("Gamepad Input");
    
// Register alternative input provider
systemManager.RegisterSystemDescriptor(gamepadDesc);
// Switch providers at runtime based on user preference
```

### Integration with Game Logic

```cpp  
// Temporarily disable input for cutscenes
auto* inputSystem = GetEngineSubsystem(IInputSubsystem);
inputSystem->SetInputEnabled(false);  // All cameras stop responding

// Reset camera position programmatically
inputCamera.ResetToInitialTransform();

// Access camera state for gameplay
glm::vec3 playerPosition = inputCamera.GetPosition();
glm::quat cameraRotation = inputCamera.GetRotation();
glm::mat4 viewMatrix = inputCamera.GetViewMatrix();

// Check provider implementation at runtime
if (inputSystem->GetInputProvider()->GetProviderId() == "sdl2-input") {
    std::cout << "Using keyboard/mouse input" << std::endl;
}
```

---

**🎉 Success!** You now have a fully functional input-controlled camera integrated with AIEngine's subsystem architecture. The input subsystem handles all hardware abstraction, event processing, and component integration automatically through the engine's provider pattern.
