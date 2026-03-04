/**
 * @file basic_usage.cpp
 * @brief Comprehensive example showing AIEngine API usage patterns
 * @version 1.0.0
 * @date 2026-03-04
 * 
 * This file demonstrates common game engine usage patterns including:
 * - Engine initialization and configuration
 * - Scene graph management and hierarchy
 * - Component system usage (Transform, Render, Custom)
 * - Game loop implementation
 * - Resource management and cleanup
 * 
 * Compile with: g++ -std=c++17 -I../engine/include basic_usage.cpp -L../engine/lib -lAIEngine -lSDL2 -lGLEW -lGL
 */

#include <AIEngine/AIEngine.hpp>
#include <iostream>
#include <memory>

/**
 * Custom component example - demonstrates extending the component system
 */
class RotationComponent : public AIEngine::IComponent {
private:
    glm::vec3 rotationAxis{0.0f, 1.0f, 0.0f};  // Default: Y-axis rotation
    float rotationSpeed = 45.0f;                // Degrees per second
    bool isPaused = false;
    
public:
    RotationComponent() = default;
    explicit RotationComponent(const glm::vec3& axis, float speed) 
        : rotationAxis(axis), rotationSpeed(speed) {}
    
    void Update(float deltaTime) override {
        if (isPaused) return;
        
        auto* transform = GetNode()->GetComponent<AIEngine::TransformComponent>();
        if (transform) {
            transform->RotateAround(rotationAxis, rotationSpeed * deltaTime);
        }
    }
    
    // Component interface
    void SetRotationAxis(const glm::vec3& axis) { rotationAxis = glm::normalize(axis); }
    void SetRotationSpeed(float speed) { rotationSpeed = speed; }
    void SetPaused(bool paused) { isPaused = paused; }
    
    // Getters
    glm::vec3 GetRotationAxis() const { return rotationAxis; }
    float GetRotationSpeed() const { return rotationSpeed; }
    bool IsPaused() const { return isPaused; }
};

/**
 * Game class - demonstrates engine integration patterns
 */
class ExampleGame {
private:
    std::unique_ptr<AIEngine::Engine> m_engine;
    AIEngine::SceneGraph* m_sceneGraph;
    
    // Scene objects
    AIEngine::SceneNode* m_rootObject;
    AIEngine::SceneNode* m_childObject1;
    AIEngine::SceneNode* m_childObject2;
    AIEngine::SceneNode* m_camera;
    
    // Game state
    float m_totalTime = 0.0f;
    bool m_isRunning = true;
    
public:
    ExampleGame() = default;
    ~ExampleGame() = default;
    
    /**
     * Initialize the game engine and create scene
     */
    bool Initialize() {
        std::cout << "=== AIEngine Basic Usage Example ===" << std::endl;
        std::cout << "Initializing game engine..." << std::endl;
        
        // 1. Configure engine
        AIEngine::EngineConfig config;
        config.windowWidth = 1280;
        config.windowHeight = 720;
        config.windowTitle = "AIEngine - Basic Usage Example";
        config.enableVSync = true;
        
        // 2. Create engine instance
        m_engine = std::make_unique<AIEngine::Engine>(config);
        if (!m_engine->Initialize()) {
            std::cerr << "Failed to initialize engine!" << std::endl;
            return false;
        }
        
        // 3. Get scene graph reference
        m_sceneGraph = m_engine->GetSceneGraph();
        if (!m_sceneGraph) {
            std::cerr << "Failed to get scene graph!" << std::endl;
            return false;
        }
        
        // 4. Create scene content
        if (!CreateScene()) {
            std::cerr << "Failed to create scene!" << std::endl;
            return false;
        }
        
        std::cout << "Game initialized successfully!" << std::endl;
        std::cout << "Press ESC to exit, Space to pause rotation" << std::endl;
        return true;
    }
    
    /**
     * Main game loop
     */
    void Run() {
        std::cout << "Starting game loop..." << std::endl;
        
        while (m_isRunning && !m_engine->ShouldClose()) {
            float deltaTime = m_engine->GetDeltaTime();
            m_totalTime += deltaTime;
            
            // Update game logic
            Update(deltaTime);
            
            // Process input
            ProcessInput();
            
            // Render frame
            m_engine->Update(deltaTime);
            m_engine->Render();
            
            // Debug output every 2 seconds
            if (static_cast<int>(m_totalTime) % 2 == 0 && 
                static_cast<int>(m_totalTime * 10) % 20 == 0) {
                PrintDebugInfo();
            }
        }
        
        std::cout << "Game loop finished." << std::endl;
    }
    
    /**
     * Cleanup
     */
    void Shutdown() {
        std::cout << "Shutting down game..." << std::endl;
        
        if (m_engine) {
            m_engine->Shutdown();
        }
        
        std::cout << "Game shutdown complete." << std::endl;
    }
    
private:
    /**
     * Create the demo scene with hierarchy and components
     */
    bool CreateScene() {
        std::cout << "Creating scene objects..." << std::endl;
        
        // Create camera
        m_camera = m_engine->CreateSceneNode();
        m_camera->SetName("MainCamera");
        auto* cameraTransform = m_camera->AddComponent<AIEngine::TransformComponent>();
        cameraTransform->SetPosition(glm::vec3(0.0f, 0.0f, 10.0f));
        cameraTransform->LookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        m_sceneGraph->AddNode(m_camera);
        
        // Create root object (parent)
        m_rootObject = m_engine->CreateSceneNode();
        m_rootObject->SetName("RootCube");
        
        auto* rootTransform = m_rootObject->AddComponent<AIEngine::TransformComponent>();
        rootTransform->SetPosition(glm::vec3(0.0f, 0.0f, -5.0f));
        rootTransform->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));
        
        auto* rootRender = m_rootObject->AddComponent<AIEngine::RenderComponent>();
        rootRender->SetMeshId("test_cube");
        rootRender->SetVisible(true);
        
        // Add custom rotation component to root
        auto* rootRotation = m_rootObject->AddComponent<RotationComponent>();
        rootRotation->SetRotationAxis(glm::vec3(0.0f, 1.0f, 0.0f));  // Y-axis
        rootRotation->SetRotationSpeed(30.0f);  // 30 degrees/second
        
        m_sceneGraph->AddNode(m_rootObject);
        
        // Create child object 1 (orbiting)
        m_childObject1 = m_engine->CreateSceneNode();
        m_childObject1->SetName("OrbitingCube1");
        
        auto* child1Transform = m_childObject1->AddComponent<AIEngine::TransformComponent>();
        child1Transform->SetPosition(glm::vec3(3.0f, 0.0f, 0.0f));  // Offset from parent
        child1Transform->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));     // Smaller
        
        auto* child1Render = m_childObject1->AddComponent<AIEngine::RenderComponent>();
        child1Render->SetMeshId("test_cube");
        child1Render->SetVisible(true);
        
        // Add different rotation to child
        auto* child1Rotation = m_childObject1->AddComponent<RotationComponent>();
        child1Rotation->SetRotationAxis(glm::vec3(1.0f, 0.0f, 0.0f));  // X-axis
        child1Rotation->SetRotationSpeed(90.0f);  // 90 degrees/second
        
        // Make child1 a child of root (demonstrates hierarchy)
        m_rootObject->AddChild(m_childObject1);
        
        // Create child object 2 (different orbit)
        m_childObject2 = m_engine->CreateSceneNode();
        m_childObject2->SetName("OrbitingCube2");
        
        auto* child2Transform = m_childObject2->AddComponent<AIEngine::TransformComponent>();
        child2Transform->SetPosition(glm::vec3(-2.0f, 2.0f, 0.0f));  // Different offset
        child2Transform->SetScale(glm::vec3(0.3f, 0.3f, 0.3f));     // Even smaller
        
        auto* child2Render = m_childObject2->AddComponent<AIEngine::RenderComponent>();
        child2Render->SetMeshId("test_cube");
        child2Render->SetVisible(true);
        
        // Add Z-axis rotation to child 2
        auto* child2Rotation = m_childObject2->AddComponent<RotationComponent>();
        child2Rotation->SetRotationAxis(glm::vec3(0.0f, 0.0f, 1.0f));  // Z-axis
        child2Rotation->SetRotationSpeed(120.0f);  // 120 degrees/second
        
        // Make child2 also a child of root
        m_rootObject->AddChild(m_childObject2);
        
        std::cout << "Scene created successfully!" << std::endl;
        std::cout << "  - Root object with Y-axis rotation (30°/s)" << std::endl;
        std::cout << "  - Child 1 with X-axis rotation (90°/s)" << std::endl;
        std::cout << "  - Child 2 with Z-axis rotation (120°/s)" << std::endl;
        std::cout << "  - Hierarchical transforms (children inherit parent rotation)" << std::endl;
        
        return true;
    }
    
    /**
     * Update game logic
     */
    void Update(float deltaTime) {
        // Game-specific update logic could go here
        // For this example, the rotation components handle the animation
        
        // Example: Scale objects based on time (breathing effect)
        if (m_rootObject) {
            auto* transform = m_rootObject->GetComponent<AIEngine::TransformComponent>();
            if (transform) {
                float breathScale = 1.0f + 0.1f * sin(m_totalTime * 2.0f);  // 0.9 - 1.1 scale
                transform->SetScale(glm::vec3(breathScale));
            }
        }
    }
    
    /**
     * Handle input
     */
    void ProcessInput() {
        // Note: In a real implementation, you'd integrate with the engine's input system
        // For this example, we rely on the engine's built-in ESC handling
        
        // Example: Toggle pause on spacebar (would need input system integration)
        // if (Input::IsKeyPressed(Key::Space)) {
        //     ToggleRotationPause();
        // }
    }
    
    /**
     * Print debug information
     */
    void PrintDebugInfo() const {
        std::cout << "\n=== Game Debug Info ===" << std::endl;
        std::cout << "Total Time: " << m_totalTime << "s" << std::endl;
        std::cout << "FPS: " << (1.0f / m_engine->GetDeltaTime()) << std::endl;
        
        if (m_sceneGraph) {
            std::cout << "Scene Nodes: " << m_sceneGraph->GetNodeCount() << std::endl;
        }
        
        // Print object positions
        if (m_rootObject) {
            auto* transform = m_rootObject->GetComponent<AIEngine::TransformComponent>();
            if (transform) {
                auto pos = transform->GetPosition();
                auto rot = transform->GetRotation();
                std::cout << "Root Pos: (" << pos.x << ", " << pos.y << ", " << pos.z << ")" << std::endl; 
                std::cout << "Root Rot: (" << rot.x << ", " << rot.y << ", " << rot.z << ")" << std::endl;
            }
        }
        std::cout << "======================\n" << std::endl;
    }
    
    /**
     * Toggle rotation pause for all objects
     */
    void ToggleRotationPause() {
        auto nodes = {m_rootObject, m_childObject1, m_childObject2};
        
        for (auto* node : nodes) {
            if (node) {
                auto* rotation = node->GetComponent<RotationComponent>();
                if (rotation) {
                    rotation->SetPaused(!rotation->IsPaused());
                }
            }
        }
        
        std::cout << "Rotation " << (m_rootObject->GetComponent<RotationComponent>()->IsPaused() ? "PAUSED" : "RESUMED") << std::endl;
    }
};

/**
 * Application entry point
 */
int main() {
    try {
        // Create game instance
        ExampleGame game;
        
        // Initialize
        if (!game.Initialize()) {
            std::cerr << "Failed to initialize game!" << std::endl;
            return -1;
        }
        
        // Run main loop
        game.Run();
        
        // Cleanup
        game.Shutdown();
        
        std::cout << "Application exited successfully." << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return -1;
    } catch (...) {
        std::cerr << "Unknown exception occurred!" << std::endl;
        return -1;
    }
}

/**
 * Example compile command:
 * g++ -std=c++17 -Wall -Wextra -O2 \
 *     -I../engine/include \
 *     -I/mingw64/include \
 *     basic_usage.cpp \
 *     -L../engine/lib -lAIEngine \
 *     -L/mingw64/lib -lSDL2 -lGLEW -lopengl32 \
 *     -o basic_usage
 *
 * Run with:
 * ./basic_usage
 *
 * Expected output:
 * - Window opens with three rotating cubes
 * - Parent cube rotates around Y-axis
 * - Child cubes orbit parent while rotating on their own axes
 * - Child cubes inherit parent rotation (hierarchical transforms)
 * - Parent cube "breathes" (scales up and down slightly)
 * - Debug info printed every 2 seconds
 * - Press ESC to exit
 */