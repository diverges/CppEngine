#pragma once

#include <memory>
#include <string>

#include "ISubsystemProvider.hpp"

namespace AIEngine {

// Forward declaration
class EngineSystemDescriptor;

/**
 * @brief Base interface for all engine subsystems.
 *
 * Subsystems provide the public interface for engine functionality while
 * delegating the actual implementation work to platform-specific providers.
 * This design enables cross-platform compatibility without requiring subsystem
 * interface changes.
 *
 * Key Design Principles:
 * - Each subsystem has exactly one provider that cannot be changed after
 * assignment
 * - Subsystems are stateless facades - all state is managed by providers
 * - Lifecycle operations must be delegated to the provider
 * - Failed initialization should be handled gracefully
 *
 * Usage Example:
 * @code
 * // Created through EngineSystemManager, not directly
 * auto* subsystem = systemManager.GetSystem<IMySubsystem>();
 *
 * if (subsystem && subsystem->Initialize()) {
 *     subsystem->Start();
 *     // Use subsystem functionality...
 *     subsystem->Stop();
 *     subsystem->Destroy();
 * }
 * @endcode
 *
 * @see EngineSystemManager for subsystem creation and management
 * @see ISubsystemProvider for implementation details
 * @see EngineSystemDescriptor for type information and factory logic
 */
class IEngineSubsystem {
public:
  virtual ~IEngineSubsystem() = default;

  /**
   * @brief Initialize the subsystem.
   *
   * Must delegate to the provider's Initialize() method. Should verify
   * that a provider is assigned before attempting initialization.
   *
   * @return true if initialization succeeded, false otherwise
   * @pre HasProvider() == true
   * @post If successful: IsInitialized() == true
   */
  virtual bool Initialize() = 0;

  /**
   * @brief Start subsystem operations.
   *
   * Should only be called after successful initialization.
   * Must delegate to the provider's Start() method.
   *
   * @pre IsInitialized() == true
   * @post If successful: IsRunning() == true
   */
  virtual void Start() = 0;

  /**
   * @brief Stop subsystem operations.
   *
   * Should stop operations but preserve initialized state for restart.
   * Must delegate to the provider's Stop() method.
   *
   * @pre IsRunning() == true (recommended)
   * @post IsRunning() == false, IsInitialized() may remain true
   */
  virtual void Stop() = 0;

  /**
   * @brief Destroy the subsystem and release all resources.
   *
   * Should stop operations and clean up all resources.
   * Must delegate to the provider's Destroy() method.
   *
   * @post IsRunning() == false, IsInitialized() == false
   */
  virtual void Destroy() = 0;

  /**
   * @brief Check if the subsystem is initialized.
   *
   * @return true if Initialize() completed successfully
   */
  virtual bool IsInitialized() const = 0;

  /**
   * @brief Check if the subsystem is currently running.
   *
   * @return true if Start() was called and subsystem is active
   */
  virtual bool IsRunning() const = 0;

  /**
   * @brief Assign a provider to this subsystem.
   *
   * This should only be called once during subsystem creation.
   * The provider cannot be changed after assignment.
   *
   * @param provider Unique pointer to provider implementation
   * @pre HasProvider() == false
   * @post HasProvider() == true, GetSubsystemProvider() != nullptr
   */
  virtual void
  SetSubsystemProvider(std::unique_ptr<ISubsystemProvider> provider) = 0;

  /**
   * @brief Get the assigned provider.
   *
   * @return Raw pointer to provider, or nullptr if no provider assigned
   */
  virtual ISubsystemProvider *GetSubsystemProvider() const = 0;

  /**
   * @brief Check if a provider is assigned.
   *
   * @return true if SetSubsystemProvider() was called with valid provider
   */
  virtual bool HasProvider() const = 0;

  /**
   * @brief Get the descriptor for this subsystem type.
   *
   * Used for type identification and system management.
   *
   * @return Descriptor containing factory and type information
   */
  virtual EngineSystemDescriptor GetDescriptor() const = 0;

  /**
   * @brief Get human-readable subsystem name.
   *
   * @return Display name for logging and debugging
   */
  virtual const std::string &GetSubsystemName() const = 0;
};

} // namespace AIEngine