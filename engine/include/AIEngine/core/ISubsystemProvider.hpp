#pragma once

#include <string>

namespace AIEngine {

/**
 * @brief Base interface for all engine subsystem providers.
 *
 * Providers implement the actual platform-specific behavior for subsystems.
 * They are the bridge between the engine's abstract interface and the underlying
 * platform APIs (OpenGL/Vulkan, SDL/Win32, etc.).
 *
 * Key Design Principles:
 * - Each provider is owned by exactly one subsystem
 * - Providers cannot be changed after assignment to a subsystem
 * - Providers must manage their own state and resources
 * - All lifecycle operations must be safe to call multiple times
 * - Failed operations should be handled gracefully without crashing
 *
 * Implementation Guidelines:
 * - Initialize() should validate prerequisites and allocate resources
 * - Start() should begin operations (start threads, open connections, etc.)
 * - Stop() should halt operations but preserve resources for restart
 * - Destroy() should release all resources and return to pre-Initialize() state
 * - State queries should return actual provider state, not cached values
 *
 * Usage Example:
 * @code
 * class OpenGLGraphicsProvider : public ISubsystemProvider {
 * public:
 *     bool Initialize() override {
 *         if (!gladLoadGL()) return false;
 *         initialized = true;
 *         return true;
 *     }
 *
 *     void Start() override {
 *         if (initialized) running = true;
 *     }
 *
 *     // ... other methods
 * };
 * @endcode
 *
 * @see IEngineSubsystem for the subsystem interface that uses providers
 * @see EngineSystemDescriptor for provider binding and creation
 */
class ISubsystemProvider {
   public:
    virtual ~ISubsystemProvider() = default;

    /**
     * @brief Initialize the provider and allocate resources.
     *
     * Should validate that all prerequisites are met (platform APIs available,
     * dependencies loaded, etc.) and allocate necessary resources.
     * Must be safe to call multiple times - subsequent calls should be no-ops.
     *
     * @return true if initialization succeeded, false otherwise
     * @post If successful: IsInitialized() == true
     */
    virtual bool Initialize() = 0;

    /**
     * @brief Start provider operations.
     *
     * Should begin active operations like starting threads, opening connections,
     * or activating platform APIs. Must be safe to call when already running.
     *
     * @pre IsInitialized() == true (recommended)
     * @post If initialized: IsRunning() == true
     */
    virtual void Start() = 0;

    /**
     * @brief Stop provider operations.
     *
     * Should halt active operations but preserve resources for potential restart.
     * Must be safe to call when already stopped or not running.
     *
     * @post IsRunning() == false, IsInitialized() may remain true
     */
    virtual void Stop() = 0;

    /**
     * @brief Destroy the provider and release all resources.
     *
     * Should stop operations and release all allocated resources, returning
     * the provider to its pre-Initialize() state. Must be safe to call
     * multiple times.
     *
     * @post IsRunning() == false, IsInitialized() == false
     */
    virtual void Destroy() = 0;

    /**
     * @brief Check if the provider is initialized.
     *
     * Should return the actual initialization state, not a cached value.
     *
     * @return true if Initialize() completed successfully and resources allocated
     */
    virtual bool IsInitialized() const = 0;

    /**
     * @brief Check if the provider is currently running.
     *
     * Should return the actual running state, not a cached value.
     *
     * @return true if Start() was called and provider is actively operating
     */
    virtual bool IsRunning() const = 0;

    /**
     * @brief Get human-readable provider name.
     *
     * Should return a descriptive name suitable for logging and debugging.
     * Examples: "OpenGL Graphics Provider", "SDL2 Platform Provider"
     *
     * @return Display name for this provider type
     */
    virtual const std::string& GetProviderName() const = 0;

    /**
     * @brief Get unique provider identifier.
     *
     * Should return a unique string identifier for this provider type.
     * Used for provider selection and configuration.
     * Examples: "opengl-graphics", "sdl2-platform"
     *
     * @return Unique identifier string (lowercase, hyphen-separated recommended)
     */
    virtual const std::string& GetProviderId() const = 0;
};

}  // namespace AIEngine