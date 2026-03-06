#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <typeindex>
#include <typeinfo>

namespace AIEngine {

// Forward declarations
class IEngineSubsystem;

// Forward declaration for friendship
class EngineSystemManager;

/**
 * Factory and metadata container for engine subsystem creation.
 * Encapsulates type information and creation logic for subsystem + provider pairs.
 * Providers are permanently bound to descriptors and cannot be changed.
 */
class EngineSystemDescriptor {
   public:
    // Create descriptor with subsystem and provider types
    template <typename SubsystemInterface, typename SubsystemImpl, typename ProviderType>
    static EngineSystemDescriptor Create(const std::string& systemName);

    // System creation using stored creation function
    template <typename SubsystemInterface>
    std::unique_ptr<SubsystemInterface> CreateSystem() const;

    // Type identification
    size_t GetInterfaceHash() const { return interfaceHash; }
    size_t GetProviderTypeHash() const { return providerTypeHash; }
    uint32_t GetSystemId() const { return systemId; }

    // Human-readable identification
    const std::string& GetSystemName() const { return systemName; }
    const std::string& GetInterfaceTypeName() const { return interfaceTypeName; }
    const std::string& GetProviderTypeName() const { return providerTypeName; }
    const std::string& GetImplementationId() const { return implementationId; }

    // Comparison
    bool operator==(const EngineSystemDescriptor& other) const {
        return interfaceHash == other.interfaceHash && providerTypeHash == other.providerTypeHash;
    }

   private:
    EngineSystemDescriptor(size_t iHash, size_t pHash, std::string name, std::string iTypeName,
                           std::string pTypeName, std::string implId, uint32_t id,
                           std::function<std::unique_ptr<IEngineSubsystem>()> createFunc);

    size_t interfaceHash;
    size_t providerTypeHash;
    std::string systemName;
    std::string interfaceTypeName;
    std::string providerTypeName;
    std::string implementationId;  // Provider-based implementation identifier
    uint32_t systemId;

    // Creation function (type-erased for storage)
    std::function<std::unique_ptr<IEngineSubsystem>()> createFunction;

    static uint32_t nextSystemId;
    friend class EngineSystemManager;
};

// Template implementations
template <typename SubsystemInterface, typename SubsystemImpl, typename ProviderType>
EngineSystemDescriptor EngineSystemDescriptor::Create(const std::string& systemName) {
    size_t interfaceHash = std::type_index(typeid(SubsystemInterface)).hash_code();
    size_t providerHash = std::type_index(typeid(ProviderType)).hash_code();

    // Create type-erased creation function
    auto createFunc = []() -> std::unique_ptr<IEngineSubsystem> {
        auto provider = std::make_unique<ProviderType>();
        auto subsystem = std::make_unique<SubsystemImpl>();
        subsystem->SetSubsystemProvider(std::move(provider));
        return std::move(subsystem);
    };

    return EngineSystemDescriptor(
        interfaceHash, providerHash, systemName, typeid(SubsystemInterface).name(),
        typeid(ProviderType).name(),
        typeid(ProviderType).name(),  // Use provider type as implementation ID
        ++nextSystemId, createFunc);
}

template <typename SubsystemInterface>
std::unique_ptr<SubsystemInterface> EngineSystemDescriptor::CreateSystem() const {
    // Call type-erased creation function and cast back to proper type
    auto subsystem = createFunction();
    SubsystemInterface* rawPtr = static_cast<SubsystemInterface*>(subsystem.release());
    return std::unique_ptr<SubsystemInterface>(rawPtr);
}

}  // namespace AIEngine