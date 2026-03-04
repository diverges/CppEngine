/**
 * TestGame.hpp - Test Game Application Class
 * 
 * Main game class that demonstrates engine functionality including
 * initialization, scene setup, component management, and game loop.
 * 
 * @version 1.0.0
 * @date 2026-03-04
 */

#pragma once

#include <AIEngine/AIEngine.hpp>
#include <memory>
#include <string>

/**
 * @brief Test Game Application Class
 * 
 * Demonstrates engine capabilities through a simple test application
 * that creates a scene with objects and manages the game loop.
 * 
 * This class serves as both an example of engine usage and a test
 * for engine functionality, ensuring the library works correctly.
 * 
 * Features demonstrated:
 * - Engine initialization and configuration
 * - Scene setup with nodes and components  
 * - Component system usage (Transform, Render)
 * - Game loop with update and render phases
 * - Input handling and graceful shutdown
 * 
 * @example Basic Usage
 * @code
 * TestGame game;
 * if (game.Initialize()) {
 *     int exitCode = game.Run();
 *     // Game automatically cleans up on destruction
 * }
 * @endcode
 */
class TestGame {
public:
    /**
     * @brief Construct test game with default settings
     */
    TestGame();

    /**
     * @brief Destructor - ensures proper cleanup
     */
    ~TestGame();

    // Prevent copying to avoid resource issues
    TestGame(const TestGame&) = delete;
    TestGame& operator=(const TestGame&) = delete;

    // Allow moving for flexibility
    TestGame(TestGame&&) noexcept;
    TestGame& operator=(TestGame&&) noexcept;

    /**
     * @brief Initialize engine and game systems
     * @return true if initialization successful, false on failure
     * 
     * Sets up engine configuration, initializes graphics, creates
     * window, and prepares the scene for rendering.
     */
    bool Initialize();

    /**
     * @brief Run main game loop
     * @return Exit code (0 = success, non-zero = error)
     * 
     * Executes the main game loop with update and render phases
     * until the user requests exit or an error occurs.
     */
    int Run();

    /**
     * @brief Clean shutdown of game and engine
     * 
     * Gracefully shuts down all systems and releases resources.
     * Called automatically by destructor if not called explicitly.
     */
    void Shutdown();

    /**
     * @brief Check if game is running
     * @return true if game loop should continue
     */
    bool IsRunning() const;

    /**
     * @brief Request game exit
     * 
     * Signals the game loop to exit on the next iteration.
     */
    void RequestExit();

    /**
     * @brief Get current frames per second
     * @return Current FPS measurement
     */
    double GetFPS() const;

    /**
     * @brief Get total game uptime
     * @return Time since Initialize() was called (seconds)
     */
    double GetUptime() const;

    /**
     * @brief Get engine configuration
     * @return Reference to current engine config
     */
    const AIEngine::EngineConfig& GetEngineConfig() const;

private:
    // Engine and core systems
    std::unique_ptr<AIEngine::Engine> m_engine;        ///< Main engine instance
    AIEngine::EngineConfig m_engineConfig;           ///< Engine configuration
    AIEngine::SceneGraph* m_sceneGraph = nullptr;    ///< Scene graph reference
    
    // Game state
    bool m_initialized = false;                       ///< Initialization status
    bool m_running = false;                          ///< Game loop status
    bool m_exitRequested = false;                   ///< Exit request flag
    
    // Frame timing
    double m_lastFrameTime = 0.0;                   ///< Previous frame timestamp
    double m_deltaTime = 0.0;                       ///< Frame delta time
    double m_fpsUpdateTimer = 0.0;                  ///< FPS calculation timer
    double m_currentFPS = 0.0;                      ///< Current FPS measurement
    int m_frameCount = 0;                           ///< Frame counter for FPS
    
    // Scene objects (for demonstration)
    AIEngine::SceneNode* m_testObject = nullptr;     ///< Test scene object
    AIEngine::SceneNode* m_camera = nullptr;         ///< Camera object
    AIEngine::SceneNode* m_rootNode = nullptr;       ///< Scene root reference

    /**
     * @brief Configure engine settings
     * @return Configured engine settings
     */
    AIEngine::EngineConfig CreateEngineConfig();

    /**
     * @brief Setup initial scene content
     * @return true if scene setup successful
     */
    bool SetupScene();

    /**
     * @brief Create test objects in the scene
     */
    void CreateTestObjects();

    /**
     * @brief Setup camera for viewing the scene
     */
    void SetupCamera();

    /**
     * @brief Update game logic (called each frame)
     * @param deltaTime Time elapsed since last frame
     */
    void Update(double deltaTime);

    /**
     * @brief Render current frame
     */
    void Render();

    /**
     * @brief Handle input events
     * @return true if input processed successfully
     */
    bool ProcessInput();

    /**
     * @brief Update frame timing and FPS calculations
     */
    void UpdateTiming();

    /**
     * @brief Handle window close events
     * @return true if window should close
     */
    bool ShouldCloseWindow();

    /**
     * @brief Print debug information periodically
     */
    void PrintDebugInfo();

    /**
     * @brief Handle game loop error conditions
     * @param error Error message
     * @return Exit code for error
     */
    int HandleError(const std::string& error);
};

// Educational note: This design demonstrates:
// - RAII resource management with proper constructor/destructor
// - Clear separation between initialization, game loop, and shutdown
// - Frame timing and performance monitoring
// - Error handling throughout the application lifecycle