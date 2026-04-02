/**
 * TestGame.cpp - Test Game Application Implementation
 *
 * Implementation of test game functionality including engine integration,
 * scene setup, and game loop management.
 *
 * @version 1.0.0
 * @date 2026-03-04
 */

#include "TestGame.hpp"
#include <chrono>
#include <iomanip>
#include <iostream>
#include <thread>

TestGame::TestGame() {
  std::cout << "TestGame: Creating test game instance...\n";
}

TestGame::~TestGame() {
  if (m_initialized) {
    Shutdown();
  }
  std::cout << "TestGame: Destructor completed.\n";
}

TestGame::TestGame(TestGame &&other) noexcept
    : m_engine(std::move(other.m_engine)),
      m_engineConfig(std::move(other.m_engineConfig)),
      m_sceneGraph(other.m_sceneGraph), m_initialized(other.m_initialized),
      m_running(other.m_running), m_exitRequested(other.m_exitRequested),
      m_lastFrameTime(other.m_lastFrameTime), m_deltaTime(other.m_deltaTime),
      m_fpsUpdateTimer(other.m_fpsUpdateTimer),
      m_currentFPS(other.m_currentFPS), m_frameCount(other.m_frameCount),
      m_testObject(other.m_testObject), m_camera(other.m_camera),
      m_rootNode(other.m_rootNode) {

  // Clear moved-from object
  other.m_sceneGraph = nullptr;
  other.m_initialized = false;
  other.m_running = false;
  other.m_testObject = nullptr;
  other.m_camera = nullptr;
  other.m_rootNode = nullptr;
}

TestGame &TestGame::operator=(TestGame &&other) noexcept {
  if (this != &other) {
    // Clean up current resources
    if (m_initialized) {
      Shutdown();
    }

    // Move resources
    m_engine = std::move(other.m_engine);
    m_engineConfig = std::move(other.m_engineConfig);
    m_sceneGraph = other.m_sceneGraph;
    m_initialized = other.m_initialized;
    m_running = other.m_running;
    m_exitRequested = other.m_exitRequested;
    m_lastFrameTime = other.m_lastFrameTime;
    m_deltaTime = other.m_deltaTime;
    m_fpsUpdateTimer = other.m_fpsUpdateTimer;
    m_currentFPS = other.m_currentFPS;
    m_frameCount = other.m_frameCount;
    m_testObject = other.m_testObject;
    m_camera = other.m_camera;
    m_rootNode = other.m_rootNode;

    // Clear moved-from object
    other.m_sceneGraph = nullptr;
    other.m_initialized = false;
    other.m_running = false;
    other.m_testObject = nullptr;
    other.m_camera = nullptr;
    other.m_rootNode = nullptr;
  }
  return *this;
}

bool TestGame::Initialize() {
  std::cout << "TestGame: Initializing...\n";

  if (m_initialized) {
    std::cout << "TestGame: Already initialized, skipping.\n";
    return true;
  }

  try {
    // Create engine configuration
    m_engineConfig = CreateEngineConfig();
    std::cout << "TestGame: Engine configuration created.\n";

    // Create engine instance
    m_engine = std::make_unique<AIEngine::Engine>(m_engineConfig);
    std::cout << "TestGame: Engine instance created.\n";

    // Initialize engine systems
    if (!m_engine->Initialize()) {
      std::cerr << "TestGame: Engine initialization failed!\n";
      return false;
    }
    std::cout << "TestGame: Engine initialized successfully.\n";

    // Get scene graph reference
    m_sceneGraph = m_engine->GetSceneGraph();
    if (!m_sceneGraph) {
      std::cerr << "TestGame: Failed to get scene graph!\n";
      return false;
    }
    std::cout << "TestGame: Scene graph acquired.\n";

    // Setup initial scene
    if (!SetupScene()) {
      std::cerr << "TestGame: Scene setup failed!\n";
      return false;
    }
    std::cout << "TestGame: Scene setup completed.\n";

    // Initialize timing
    auto now = std::chrono::high_resolution_clock::now();
    m_lastFrameTime =
        std::chrono::duration<double>(now.time_since_epoch()).count();

    m_initialized = true;
    std::cout << "TestGame: Initialization completed successfully.\n";
    return true;
  } catch (const std::exception &e) {
    std::cerr << "TestGame: Initialization failed with exception: " << e.what()
              << "\n";
    return false;
  }
}

int TestGame::Run() {
  if (!m_initialized) {
    std::cerr << "TestGame: Cannot run - not initialized!\n";
    return -1;
  }

  std::cout << "TestGame: Starting main game loop.\n";
  m_running = true;
  m_exitRequested = false;

  try {
    // Main game loop
    while (m_running && !m_exitRequested && !ShouldCloseWindow()) {

      // Update timing
      UpdateTiming();

      // Process input
      if (!ProcessInput()) {
        std::cout << "TestGame: Input processing requested exit.\n";
        break;
      }

      // Update game logic
      Update(m_deltaTime);

      // Render frame
      Render();

      // Yield CPU briefly for other processes
      // In a real implementation, this might be handled by the engine
      std::this_thread::sleep_for(std::chrono::microseconds(100));
    }

    std::cout << "TestGame: Main loop exited normally.\n";
    return 0;
  } catch (const std::exception &e) {
    return HandleError("Game loop error: " + std::string(e.what()));
  } catch (...) {
    return HandleError("Unknown game loop error occurred");
  }
}

void TestGame::Shutdown() {
  if (!m_initialized) {
    return;
  }

  std::cout << "TestGame: Beginning shutdown...\n";

  m_running = false;

  // Clear scene references (scene graph is owned by engine)
  m_testObject = nullptr;
  m_camera = nullptr;
  m_rootNode = nullptr;
  m_sceneGraph = nullptr;

  // Shutdown engine
  if (m_engine) {
    m_engine->Shutdown();
    std::cout << "TestGame: Engine shutdown completed.\n";
  }

  // Reset state
  m_initialized = false;
  m_frameCount = 0;
  m_currentFPS = 0.0;

  std::cout << "TestGame: Shutdown completed.\n";
}

bool TestGame::IsRunning() const { return m_running && m_initialized; }

void TestGame::RequestExit() {
  std::cout << "TestGame: Exit requested.\n";
  m_exitRequested = true;
}

double TestGame::GetFPS() const { return m_currentFPS; }

double TestGame::GetUptime() const {
  return m_engine ? m_engine->GetUptime() : 0.0;
}

const AIEngine::EngineConfig &TestGame::GetEngineConfig() const {
  return m_engineConfig;
}

// --- Private Methods ---

AIEngine::EngineConfig TestGame::CreateEngineConfig() {
  AIEngine::EngineConfig config;

  // Window settings
  config.windowTitle = "AIEngine Test Game v1.0.0";
  config.windowWidth = 1280;
  config.windowHeight = 720;
  config.fullscreen = false;

  // Graphics settings
  config.vsync = true;
  config.msaaSamples = 4; // 4x antialiasing
  config.openglMajorVersion = 3;
  config.openglMinorVersion = 3;
  config.openglCoreProfile = true;

  // Performance settings
  config.targetFrameRate = 60.0;
  config.maxSceneNodes = 1000;
  config.maxComponents = 5000;

  // Debug settings
  config.enableDebugging = true;
  config.logToFile = false; // Console logging only for this demo

  std::cout << "TestGame: Engine config - " << config.windowWidth << "x"
            << config.windowHeight << " @ " << config.targetFrameRate
            << "fps\n";

  return config;
}

bool TestGame::SetupScene() {
  if (!m_sceneGraph) {
    return false;
  }

  // Get root node
  m_rootNode = m_sceneGraph->GetRootNode();
  if (!m_rootNode) {
    std::cerr << "TestGame: No root node available!\n";
    return false;
  }

  std::cout << "TestGame: Setting up camera...\n";
  SetupCamera();

  std::cout << "TestGame: Creating test objects...\n";
  CreateTestObjects();

  return true;
}

void TestGame::CreateTestObjects() {
  // Create a test object node
  auto testNode = m_sceneGraph->CreateNode("TestObject");
  if (!testNode) {
    std::cerr << "TestGame: Failed to create test node!\n";
    return;
  }

  // Add transform component
  auto *transform = testNode->AddComponent<AIEngine::TransformComponent>();
  if (transform) {
    // Position the cube perfectly centered in front of the camera
    transform->SetPosition(0.0f, 0.0f, -5.0f);
    transform->SetScale(1.0f, 1.0f, 1.0f);
    transform->SetRotationEuler(0.0f, 45.0f, 0.0f); // Rotate 45 degrees
    std::cout << "TestGame: Transform component added to test object.\n";
  } else {
    std::cerr << "TestGame: Failed to add transform component!\n";
  }

  // Add render component
  auto *render = testNode->AddComponent<AIEngine::RenderComponent>();
  if (render) {
    render->SetMeshId("test_cube");
    render->SetVisible(true);
    render->SetColor(0.8f, 0.4f, 0.2f); // Orange color
    std::cout << "TestGame: Render component added to test object.\n";
  } else {
    std::cerr << "TestGame: Failed to add render component!\n";
  }

  // Add to scene
  m_testObject = m_rootNode->AddChild(std::move(testNode));

  std::cout << "TestGame: Test object created and added to scene.\n";
}

void TestGame::SetupCamera() {
  // Create camera node
  auto cameraNode = m_sceneGraph->CreateNode("Camera");
  if (!cameraNode) {
    std::cerr << "TestGame: Failed to create camera node!\n";
    return;
  }

  // Add transform for camera positioning
  auto *cameraTransform =
      cameraNode->AddComponent<AIEngine::TransformComponent>();
  if (cameraTransform) {
    // Position camera to look at scene
    cameraTransform->SetPosition(0.0f, 2.0f, 5.0f);
    cameraTransform->LookAt(glm::vec3(0.0f, 0.0f, 0.0f)); // Look at origin
    std::cout << "TestGame: Camera positioned and oriented.\n";
  }

  // Add to scene
  m_camera = m_rootNode->AddChild(std::move(cameraNode));

  std::cout << "TestGame: Camera created and added to scene.\n";
}

void TestGame::Update(double deltaTime) {
  // Update engine systems
  if (m_engine) {
    m_engine->Update(deltaTime);
  }

  // Example: Rotate the test object
  if (m_testObject) {
    auto *transform =
        m_testObject->GetComponent<AIEngine::TransformComponent>();
    if (transform) {
      // Rotate around Y axis at 30 degrees per second
      float rotationSpeed = 30.0f; // degrees per second
      transform->RotateAround(glm::vec3(0, 1, 0), rotationSpeed * deltaTime);
    }
  }

  // Print debug info occasionally
  static double debugTimer = 0.0;
  debugTimer += deltaTime;
  if (debugTimer >= 2.0) { // Every 2 seconds
    PrintDebugInfo();
    debugTimer = 0.0;
  }
}

void TestGame::Render() {
  // Render through engine
  if (m_engine) {
    m_engine->Render();
  }
}

bool TestGame::ProcessInput() {
  // In a full implementation, this would check for:
  // - ESC key press (request exit)
  // - Window close button
  // - Other input events

  // For minimal implementation, we just check engine state
  if (m_engine && m_engine->ShouldClose()) {
    return false;
  }

  return true;
}

void TestGame::UpdateTiming() {
  auto now = std::chrono::high_resolution_clock::now();
  double currentTime =
      std::chrono::duration<double>(now.time_since_epoch()).count();

  m_deltaTime = currentTime - m_lastFrameTime;
  m_lastFrameTime = currentTime;

  // Update FPS calculation
  m_frameCount++;
  m_fpsUpdateTimer += m_deltaTime;

  if (m_fpsUpdateTimer >= 1.0) { // Update FPS every second
    m_currentFPS = m_frameCount / m_fpsUpdateTimer;
    m_frameCount = 0;
    m_fpsUpdateTimer = 0.0;
  }
}

bool TestGame::ShouldCloseWindow() {
  // Check with engine if window should close
  return m_engine ? m_engine->ShouldClose() : false;
}

void TestGame::PrintDebugInfo() {
  std::cout << "=== TestGame Debug Info ===\n";
  std::cout << "FPS: " << std::fixed << std::setprecision(1) << GetFPS()
            << "\n";
  std::cout << "Uptime: " << std::setprecision(2) << GetUptime() << "s\n";
  std::cout << "Delta Time: " << std::setprecision(4) << m_deltaTime << "s\n";

  if (m_sceneGraph) {
    std::cout << "Scene Nodes: " << m_sceneGraph->GetNodeCount() << "\n";
    std::cout << "Active Nodes: " << m_sceneGraph->GetActiveNodeCount() << "\n";
  }

  if (m_testObject) {
    auto *transform =
        m_testObject->GetComponent<AIEngine::TransformComponent>();
    if (transform) {
      glm::vec3 pos = transform->GetPosition();
      glm::vec3 euler = transform->GetEulerAngles();
      std::cout << "Test Object - Pos: (" << pos.x << ", " << pos.y << ", "
                << pos.z << ")\n";
      std::cout << "            - Rot: (" << euler.x << ", " << euler.y << ", "
                << euler.z << ")\n";
    }
  }

  std::cout << "========================\n\n";
}

int TestGame::HandleError(const std::string &error) {
  std::cerr << "TestGame ERROR: " << error << "\n";
  m_running = false;
  return -1;
}