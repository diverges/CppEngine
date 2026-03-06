#pragma once

#include <memory>
#include <string>

#include "ISubsystemProvider.hpp"

namespace AIEngine {

// Forward declaration
class EngineSystemDescriptor;

/**
 * Base interface for all engine subsystems.
 * Subsystems delegate actual work to platform-specific providers.
 * Each subsystem has exactly one provider that cannot be changed after assignment.
 */
class IEngineSubsystem {
   public:
    virtual ~IEngineSubsystem() = default;

    // Subsystem lifecycle (delegates to provider)
    virtual bool Initialize() = 0;
    virtual void Start() = 0;
    virtual void Stop() = 0;
    virtual void Destroy() = 0;

    // Subsystem state
    virtual bool IsInitialized() const = 0;
    virtual bool IsRunning() const = 0;

    // Provider management
    virtual void SetSubsystemProvider(std::unique_ptr<ISubsystemProvider> provider) = 0;
    virtual ISubsystemProvider* GetSubsystemProvider() const = 0;
    virtual bool HasProvider() const = 0;

    // Subsystem metadata
    virtual EngineSystemDescriptor GetDescriptor() const = 0;
    virtual const std::string& GetSubsystemName() const = 0;
};

}  // namespace AIEngine