/**
 * Engine.cpp - Core Engine Implementation
 *
 * Minimal implementation to satisfy build requirements.
 * This will be expanded during Phase 3 User Story development.
 *
 * @version 1.0.0
 * @date 2026-03-04
 */

#include "../../include/AIEngine/core/Engine.hpp"

#include <chrono>
#include <iostream>

#include "../../include/AIEngine/core/EngineSystemManager.hpp"
#include "../../include/AIEngine/graphics/Renderer.hpp"
#include "../../include/AIEngine/platform/Window.hpp"
#include "../../include/AIEngine/scene/SceneGraph.hpp"

namespace AIEngine {

/**
 * EngineConfig implementation
 */
bool EngineConfig::Validate() const {
  // Basic validation checks
  if (windowWidth <= 0 || windowHeight <= 0) {
    return false;
  }

  if (targetFrameRate <= 0.0) {
    return false;
  }

  if (openglMajorVersion < 3 ||
      (openglMajorVersion == 3 && openglMinorVersion < 3)) {
    return false; // Require OpenGL 3.3+
  }

  if (maxSceneNodes == 0 || maxComponents == 0) {
    return false;
  }

  return true;
}

/**
 * Engine implementation using PIMPL idiom
 */
struct Engine::Impl {
  EngineConfig config;
  bool initialized = false;
  bool shouldClose = false;

  // Timing
  std::chrono::high_resolution_clock::time_point startTime;
  std::chrono::high_resolution_clock::time_point lastFrameTime;
  double deltaTime = 0.0;
  double fps = 0.0;

  // Subsystems
  std::unique_ptr<Window> window;
  std::unique_ptr<SceneGraph> sceneGraph;
  std::unique_ptr<Renderer> renderer;

  explicit Impl(const EngineConfig &cfg) : config(cfg) {
    startTime = std::chrono::high_resolution_clock::now();
    lastFrameTime = startTime;
  }
};

Engine::Engine(const EngineConfig &config)
    : m_pImpl(std::make_unique<Impl>(config)) {}

Engine::~Engine() {
  if (m_pImpl && m_pImpl->initialized) {
    Shutdown();
  }
}

Engine::Engine(Engine &&) noexcept = default;
Engine &Engine::operator=(Engine &&) noexcept = default;

bool Engine::Initialize() {
  if (!m_pImpl->config.Validate()) {
    std::cerr << "Engine: Invalid configuration\n";
    return false;
  }

  std::cout << "Engine: Initializing " << m_pImpl->config.windowTitle << "\n";

  // Initialize window first
  WindowConfig windowConfig;
  windowConfig.width = m_pImpl->config.windowWidth;
  windowConfig.height = m_pImpl->config.windowHeight;
  windowConfig.title = m_pImpl->config.windowTitle;
  windowConfig.vsync = m_pImpl->config.vsync;
  windowConfig.openglMajor = m_pImpl->config.openglMajorVersion;
  windowConfig.openglMinor = m_pImpl->config.openglMinorVersion;
  windowConfig.openglCore = m_pImpl->config.openglCoreProfile;
  windowConfig.resizable =
      false; // Prevent window resizing for scripted experience

  m_pImpl->window = std::make_unique<Window>(windowConfig);
  if (!m_pImpl->window->Initialize()) {
    std::cerr << "Engine: Failed to initialize window\n";
    return false;
  }

  // Initialize renderer with window
  m_pImpl->renderer = std::make_unique<Renderer>(m_pImpl->window.get());
  if (!m_pImpl->renderer->Initialize()) {
    std::cerr << "Engine: Failed to initialize renderer\n";
    return false;
  }

  // Initialize scene graph
  m_pImpl->sceneGraph = std::make_unique<SceneGraph>();
  std::cout << "Engine: Scene graph initialized\n";

  // Initialize all registered subsystems
  auto &systemManager = EngineSystemManager::GetInstance();
  if (!systemManager.InitializeAllSystems()) {
    std::cerr << "Engine: Failed to initialize subsystems\n";
    return false;
  }
  std::cout << "Engine: All subsystems initialized\n";

  // Start all subsystems
  systemManager.StartAllSystems();
  std::cout << "Engine: All subsystems started\n";

  m_pImpl->initialized = true;
  return true;
}

void Engine::Update(double deltaTime) {
  if (!m_pImpl->initialized) {
    return;
  }

  // Poll window events
  m_pImpl->window->PollEvents();

  // Check if window wants to close
  if (m_pImpl->window->ShouldClose()) {
    m_pImpl->shouldClose = true;
  }

  m_pImpl->deltaTime = deltaTime;

  // Calculate FPS
  auto currentTime = std::chrono::high_resolution_clock::now();
  auto frameDuration = std::chrono::duration_cast<std::chrono::microseconds>(
                           currentTime - m_pImpl->lastFrameTime)
                           .count();

  if (frameDuration > 0) {
    m_pImpl->fps = 1000000.0 / frameDuration; // Convert microseconds to FPS
  }

  m_pImpl->lastFrameTime = currentTime;

  // TODO: Update scene graph
  // TODO: Process input
  // TODO: Run physics
}

void Engine::Render() {
  if (!m_pImpl->initialized) {
    return;
  }

  // Begin frame rendering
  m_pImpl->renderer->BeginFrame();

  // Render scene graph
  m_pImpl->renderer->RenderScene(m_pImpl->sceneGraph.get());

  // End frame and present
  m_pImpl->renderer->EndFrame();

  // Swap window buffers
  m_pImpl->window->SwapBuffers();
}

void Engine::Shutdown() {
  if (!m_pImpl->initialized) {
    return;
  }

  std::cout << "Engine: Shutting down\n";

  // Stop and destroy all subsystems first
  auto &systemManager = EngineSystemManager::GetInstance();
  systemManager.StopAllSystems();
  std::cout << "Engine: All subsystems stopped\n";

  systemManager.DestroyAllSystems();
  std::cout << "Engine: All subsystems destroyed\n";

  // Cleanup scene graph
  m_pImpl->sceneGraph.reset();

  // Shutdown renderer
  m_pImpl->renderer.reset();

  // Cleanup window and SDL
  m_pImpl->window.reset();

  m_pImpl->initialized = false;
}

bool Engine::ShouldClose() const { return m_pImpl->shouldClose; }

double Engine::GetDeltaTime() const { return m_pImpl->deltaTime; }

double Engine::GetFPS() const { return m_pImpl->fps; }

const EngineConfig &Engine::GetConfig() const { return m_pImpl->config; }

bool Engine::IsInitialized() const { return m_pImpl->initialized; }

SceneGraph *Engine::GetSceneGraph() { return m_pImpl->sceneGraph.get(); }

Renderer *Engine::GetRenderer() { return m_pImpl->renderer.get(); }

void Engine::RequestClose() { m_pImpl->shouldClose = true; }

double Engine::GetUptime() const {
  auto currentTime = std::chrono::high_resolution_clock::now();
  auto uptime = std::chrono::duration_cast<std::chrono::milliseconds>(
                    currentTime - m_pImpl->startTime)
                    .count();

  return uptime / 1000.0; // Convert to seconds
}

/**
 * Global system functions
 */
bool InitializeGlobalSystems() {
  std::cout << "AIEngine: Initializing global systems\n";
  // TODO: Initialize logging, memory pools, etc.
  return true;
}

void ShutdownGlobalSystems() {
  std::cout << "AIEngine: Shutting down global systems\n";
  // TODO: Cleanup global resources
}

} // namespace AIEngine