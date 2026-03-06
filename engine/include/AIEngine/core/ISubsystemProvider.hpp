#pragma once

#include <string>

namespace AIEngine {

/**
 * Base interface for all engine subsystem providers.
 * Providers implement platform-specific behavior for subsystems.
 * Each provider is owned by exactly one subsystem and cannot be changed after assignment.
 */
class ISubsystemProvider {
   public:
    virtual ~ISubsystemProvider() = default;

    // Provider lifecycle
    virtual bool Initialize() = 0;
    virtual void Start() = 0;
    virtual void Stop() = 0;
    virtual void Destroy() = 0;

    // Provider state
    virtual bool IsInitialized() const = 0;
    virtual bool IsRunning() const = 0;

    // Provider identification
    virtual const std::string& GetProviderName() const = 0;
    virtual const std::string& GetProviderId() const = 0;
};

}  // namespace AIEngine