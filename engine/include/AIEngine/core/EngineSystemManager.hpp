#pragma once

#include <memory>
#include <stdexcept>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <vector>

#include "EngineSystemDescriptor.hpp"
#include "IEngineSubsystem.hpp"

namespace AIEngine {

/**
 * @brief Singleton registry and lifecycle manager for all engine subsystems.
 *
 * EngineSystemManager serves as the central hub for all subsystem management
 * in the engine. It provides O(1) subsystem access using type-based hash maps
 * and manages the complete lifecycle of registered subsystems.
 *
 * Key Features:
 * - O(1) subsystem access by interface type
 * - Automated lifecycle management (Initialize→Start→Stop→Destroy)
 * - Single subsystem per interface type constraint
 * - Thread-safe singleton access (single-threaded usage only)
 * - Descriptor-based type-safe subsystem creation
 *
 * Design Constraints:
 * - Single-threaded design - all operations must occur on main thread
 * - One active subsystem per interface type maximum
 * - Descriptors must be registered before creating subsystems
 * - Subsystems are owned by the manager (unique_ptr storage)
 *
 * Usage Pattern:
 * @code
 * // 1. Get singleton instance
 * auto& manager = EngineSystemManager::GetInstance();
 *
 * // 2. Register descriptor
 * auto descriptor = EngineSystemDescriptor::Create<...>(...);
 * manager.RegisterSystemDescriptor(descriptor);
 *
 * // 3. Create and use subsystem
 * auto* subsystem = manager.CreateSystem<IMySubsystem>(descriptor);
 *
 * // 4. Access later via convenience macro or direct call
 * auto* sameSubsystem = GetEngineSubsystem(IMySubsystem);
 *
 * // 5. Lifecycle managed automatically by Engine class
 * // manager.InitializeAllSystems();
 * // manager.StartAllSystems();
 * @endcode
 *
 * Performance Guarantees:
 * - GetSystem(): O(1) hash map lookup
 * - CreateSystem(): O(1) registration + construction time
 * - HasSystem(): O(1) hash map lookup
 * - Operations complete within 1ms on typical hardware
 *
 * @see EngineSystemDescriptor for type-safe subsystem creation
 * @see IEngineSubsystem for subsystem interface requirements
 * @see GetEngineSubsystem() macro for convenient access
 */
class EngineSystemManager {
   public:
    // Singleton access
    static EngineSystemManager& GetInstance();

    // Descriptor Registration
    void RegisterSystemDescriptor(const EngineSystemDescriptor& descriptor);
    void UnregisterSystemDescriptor(const EngineSystemDescriptor& descriptor);

    // System Creation and Access (O(1) performance)
    template <typename SystemInterface>
    SystemInterface* CreateSystem(const EngineSystemDescriptor& descriptor);

    template <typename SystemInterface>
    SystemInterface* GetSystem();

    template <typename SystemInterface>
    const SystemInterface* GetSystem() const;

    template <typename SystemInterface>
    bool HasSystem() const;

    template <typename SystemInterface>
    void DestroySystem();

    // Lifecycle Management (called by Engine class)
    bool InitializeAllSystems();
    void StartAllSystems();
    void StopAllSystems();
    void DestroyAllSystems();

    // System Enumeration
    std::vector<EngineSystemDescriptor> GetRegisteredDescriptors() const;
    std::vector<IEngineSubsystem*> GetActiveSubsystems();
    std::vector<const IEngineSubsystem*> GetActiveSubsystems() const;

   private:
    EngineSystemManager() = default;
    ~EngineSystemManager() = default;

    // Non-copyable, non-movable singleton
    EngineSystemManager(const EngineSystemManager&) = delete;
    EngineSystemManager& operator=(const EngineSystemManager&) = delete;
    EngineSystemManager(EngineSystemManager&&) = delete;
    EngineSystemManager& operator=(EngineSystemManager&&) = delete;

    struct SystemEntry {
        std::unique_ptr<IEngineSubsystem> subsystem;
        EngineSystemDescriptor descriptor;
        bool initialized = false;
        bool running = false;
    };

    // Fast lookup by interface hash
    std::unordered_map<size_t, std::unique_ptr<SystemEntry>> activeSubsystemsByInterface;
    std::unordered_map<size_t, std::vector<EngineSystemDescriptor>>
        registeredDescriptorsByInterface;
};

// Template implementations
template <typename SystemInterface>
SystemInterface* EngineSystemManager::CreateSystem(const EngineSystemDescriptor& descriptor) {
    size_t interfaceHash = std::type_index(typeid(SystemInterface)).hash_code();

    // Check if system already exists
    if (activeSubsystemsByInterface.find(interfaceHash) != activeSubsystemsByInterface.end()) {
        throw std::runtime_error("Subsystem of type already exists: " +
                                 descriptor.GetInterfaceTypeName());
    }

    // Create the subsystem using the descriptor
    auto subsystem = descriptor.CreateSystem<SystemInterface>();
    SystemInterface* rawPtr = subsystem.get();

    // Store in active systems
    auto entry = std::make_unique<SystemEntry>();
    entry->subsystem = std::move(subsystem);
    entry->descriptor = descriptor;
    entry->initialized = false;
    entry->running = false;

    activeSubsystemsByInterface[interfaceHash] = std::move(entry);

    return rawPtr;
}

template <typename SystemInterface>
SystemInterface* EngineSystemManager::GetSystem() {
    size_t interfaceHash = std::type_index(typeid(SystemInterface)).hash_code();

    auto it = activeSubsystemsByInterface.find(interfaceHash);
    if (it == activeSubsystemsByInterface.end()) {
        return nullptr;
    }

    return static_cast<SystemInterface*>(it->second->subsystem.get());
}

template <typename SystemInterface>
const SystemInterface* EngineSystemManager::GetSystem() const {
    size_t interfaceHash = std::type_index(typeid(SystemInterface)).hash_code();

    auto it = activeSubsystemsByInterface.find(interfaceHash);
    if (it == activeSubsystemsByInterface.end()) {
        return nullptr;
    }

    return static_cast<const SystemInterface*>(it->second->subsystem.get());
}

template <typename SystemInterface>
bool EngineSystemManager::HasSystem() const {
    size_t interfaceHash = std::type_index(typeid(SystemInterface)).hash_code();
    return activeSubsystemsByInterface.find(interfaceHash) != activeSubsystemsByInterface.end();
}

template <typename SystemInterface>
void EngineSystemManager::DestroySystem() {
    size_t interfaceHash = std::type_index(typeid(SystemInterface)).hash_code();

    auto it = activeSubsystemsByInterface.find(interfaceHash);
    if (it != activeSubsystemsByInterface.end()) {
        if (it->second->running) {
            it->second->subsystem->Stop();
        }
        it->second->subsystem->Destroy();
        activeSubsystemsByInterface.erase(it);
    }
}

// Convenience macro for subsystem access
#define GetEngineSubsystem(SubsystemType) \
    EngineSystemManager::GetInstance().GetSystem<SubsystemType>()

}  // namespace AIEngine