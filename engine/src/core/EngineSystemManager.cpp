#include "AIEngine/core/EngineSystemManager.hpp"

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace AIEngine {

EngineSystemManager& EngineSystemManager::GetInstance() {
    static EngineSystemManager instance;
    return instance;
}

void EngineSystemManager::RegisterSystemDescriptor(const EngineSystemDescriptor& descriptor) {
    size_t interfaceHash = descriptor.GetInterfaceHash();

    // Check for duplicate interface registration (single subsystem per interface constraint)
    auto it = registeredDescriptorsByInterface.find(interfaceHash);
    if (it != registeredDescriptorsByInterface.end() && !it->second.empty()) {
        throw std::runtime_error("Cannot register multiple subsystems for interface: " +
                                 descriptor.GetInterfaceTypeName());
    }

    // Register the descriptor
    registeredDescriptorsByInterface[interfaceHash] = {descriptor};
}

void EngineSystemManager::UnregisterSystemDescriptor(const EngineSystemDescriptor& descriptor) {
    size_t interfaceHash = descriptor.GetInterfaceHash();

    auto it = registeredDescriptorsByInterface.find(interfaceHash);
    if (it != registeredDescriptorsByInterface.end()) {
        auto& descriptors = it->second;
        descriptors.erase(std::remove_if(descriptors.begin(), descriptors.end(),
                                         [&descriptor](const EngineSystemDescriptor& d) {
                                             return d == descriptor;
                                         }),
                          descriptors.end());

        if (descriptors.empty()) {
            registeredDescriptorsByInterface.erase(it);
        }
    }
}

bool EngineSystemManager::InitializeAllSystems() {
    // Initialize in registration order
    for (auto& [interfaceHash, entry] : activeSubsystemsByInterface) {
        if (!entry->initialized) {
            if (!entry->subsystem->Initialize()) {
                std::cerr << "Failed to initialize subsystem: " << entry->descriptor.GetSystemName()
                          << std::endl;
                return false;  // Fail-fast error handling
            }
            entry->initialized = true;
        }
    }
    return true;
}

void EngineSystemManager::StartAllSystems() {
    for (auto& [interfaceHash, entry] : activeSubsystemsByInterface) {
        if (entry->initialized && !entry->running) {
            entry->subsystem->Start();
            entry->running = true;
        }
    }
}

void EngineSystemManager::StopAllSystems() {
    for (auto& [interfaceHash, entry] : activeSubsystemsByInterface) {
        if (entry->running) {
            entry->subsystem->Stop();
            entry->running = false;
        }
    }
}

void EngineSystemManager::DestroyAllSystems() {
    // Since unordered_map doesn't support reverse iteration,
    // collect entries and destroy them (order not guaranteed for destruction)
    std::vector<std::unique_ptr<SystemEntry>> entries;
    entries.reserve(activeSubsystemsByInterface.size());

    // Move entries out of map
    for (auto& pair : activeSubsystemsByInterface) {
        entries.push_back(std::move(pair.second));
    }
    activeSubsystemsByInterface.clear();

    // Destroy systems (order not guaranteed, but all are properly cleaned up)
    for (auto& entry : entries) {
        if (entry->running) {
            entry->subsystem->Stop();
            entry->running = false;
        }
        entry->subsystem->Destroy();
        entry->initialized = false;
    }
}

std::vector<EngineSystemDescriptor> EngineSystemManager::GetRegisteredDescriptors() const {
    std::vector<EngineSystemDescriptor> result;

    for (const auto& [interfaceHash, descriptors] : registeredDescriptorsByInterface) {
        for (const auto& descriptor : descriptors) {
            result.push_back(descriptor);
        }
    }

    return result;
}

std::vector<IEngineSubsystem*> EngineSystemManager::GetActiveSubsystems() {
    std::vector<IEngineSubsystem*> result;

    for (const auto& [interfaceHash, entry] : activeSubsystemsByInterface) {
        result.push_back(entry->subsystem.get());
    }

    return result;
}

std::vector<const IEngineSubsystem*> EngineSystemManager::GetActiveSubsystems() const {
    std::vector<const IEngineSubsystem*> result;

    for (const auto& [interfaceHash, entry] : activeSubsystemsByInterface) {
        result.push_back(entry->subsystem.get());
    }

    return result;
}

}  // namespace AIEngine