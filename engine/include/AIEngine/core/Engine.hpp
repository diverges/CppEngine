/**
 * Engine.hpp - Core Engine Class Declaration
 *
 * The main Engine class coordinates all subsystems and manages the
 * application lifecycle, providing initialization, update loop,
 * and resource management for the game engine.
 *
 * @version 1.0.0
 * @date 2026-03-04
 */

#pragma once

#include <memory>
#include <string>

// Forward declarations to minimize header dependencies
namespace AIEngine {
class SceneGraph;
class Renderer;
struct EngineConfig;
} // namespace AIEngine

namespace AIEngine {

/**
 * @brief Main Engine Class
 *
 * Central coordinator for all engine subsystems including graphics,
 * scene management, input handling, and resource management.
 * Manages the complete application lifecycle from initialization
 * through update loops to shutdown.
 *
 * @example Basic Usage
 * @code
 * AIEngine::EngineConfig config;
 * config.windowTitle = "My Game";
 *
 * AIEngine::Engine engine(config);
 * if (!engine.Initialize()) {
 *     // Handle initialization failure
 *     return -1;
 * }
 *
 * while (!engine.ShouldClose()) {
 *     double deltaTime = engine.GetDeltaTime();
 *     engine.Update(deltaTime);
 *     engine.Render();
 * }
 *
 * engine.Shutdown();
 * @endcode
 */
class Engine {
public:
  /**
   * @brief Construct Engine with configuration
   * @param config Engine configuration settings
   */
  explicit Engine(const EngineConfig &config);

  /**
   * @brief Destructor - ensures proper cleanup
   */
  ~Engine();

  // Prevent copying to avoid resource conflicts
  Engine(const Engine &) = delete;
  Engine &operator=(const Engine &) = delete;

  // Allow moving for transfer semantics
  Engine(Engine &&) noexcept;
  Engine &operator=(Engine &&) noexcept;

  /**
   * @brief Initialize engine and all subsystems
   * @return true if initialization successful, false on failure
   *
   * Initializes graphics context, creates window, initializes
   * OpenGL, sets up scene graph, and prepares all subsystems.
   */
  bool Initialize();

  /**
   * @brief Update engine state and all subsystems
   * @param deltaTime Time elapsed since last frame (in seconds)
   *
   * Updates scene graph, processes input, runs physics,
   * and prepares frame for rendering.
   */
  void Update(double deltaTime);

  /**
   * @brief Render current frame
   *
   * Executes render pipeline: clears buffers, renders scene,
   * swaps buffers, and presents frame to display.
   */
  void Render();

  /**
   * @brief Shutdown engine and cleanup resources
   *
   * Safely shuts down all subsystems, releases resources,
   * and closes graphics context. Called automatically
   * by destructor if not called explicitly.
   */
  void Shutdown();

  /**
   * @brief Check if application should close
   * @return true if close requested (e.g., window close button)
   */
  bool ShouldClose() const;

  /**
   * @brief Get time elapsed since last frame
   * @return Delta time in seconds
   */
  double GetDeltaTime() const;

  /**
   * @brief Get current frames per second
   * @return Current FPS measurement
   */
  double GetFPS() const;

  /**
   * @brief Get engine configuration
   * @return Reference to current configuration
   */
  const EngineConfig &GetConfig() const;

  /**
   * @brief Check if engine is successfully initialized
   * @return true if Initialize() completed successfully
   */
  bool IsInitialized() const;

  /**
   * @brief Get scene graph for managing objects and hierarchy
   * @return Pointer to scene graph, nullptr if not initialized
   */
  SceneGraph *GetSceneGraph();

  /**
   * @brief Get renderer for direct graphics operations
   * @return Pointer to renderer, nullptr if not initialized
   */
  Renderer *GetRenderer();

  /**
   * @brief Request application closure
   *
   * Sets ShouldClose() to return true, allowing graceful
   * exit from main loop.
   */
  void RequestClose();

  /**
   * @brief Get engine uptime in seconds
   * @return Total time since Initialize() was called
   */
  double GetUptime() const;

private:
  // Private implementation to hide internal details
  struct Impl;
  std::unique_ptr<Impl> m_pImpl;
};

} // namespace AIEngine

// Educational notes:
// - Uses PIMPL idiom to hide implementation details and reduce header
// dependencies
// - RAII resource management ensures proper cleanup
// - Move-only semantics prevent accidental resource duplication
// - Clear separation between interface (header) and implementation (.cpp)