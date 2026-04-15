#include <string>

namespace AIEngine {

/**
 * @brief Engine configuration settings
 *
 * Configuration structure passed to Engine constructor to specify
 * initialization parameters for graphics, audio, and other subsystems.
 */
struct EngineConfig {
  // Window and graphics configuration
  std::string windowTitle = "AIEngine Application";
  int windowWidth = 1280;
  int windowHeight = 720;
  bool fullscreen = false;
  bool vsync = true;
  int msaaSamples = 4; // Antialiasing samples, 0 to disable

  // Graphics API settings
  int openglMajorVersion = 3;
  int openglMinorVersion = 3;
  bool openglCoreProfile = true;

  // Engine behavior
  double targetFrameRate = 60.0;
  bool enableDebugging = true;
  bool logToFile = false;
  std::string logFilePath = "engine.log";

  // Scene and memory limits
  size_t maxSceneNodes = 10000;
  size_t maxComponents = 50000;

  /**
   * @brief Validate configuration settings
   * @return true if all settings are valid
   */
  bool Validate() const;
};

} // namespace AIEngine
