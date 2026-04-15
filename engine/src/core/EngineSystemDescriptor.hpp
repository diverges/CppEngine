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
 * @brief Factory and metadata container for engine subsystem creation.
 *
 * EngineSystemDescriptor encapsulates all the type information and creation
 * logic needed to instantiate a subsystem with its bound provider. It serves as
 * a type-safe factory that maintains the relationship between subsystem
 * interfaces, implementations, and their platform-specific providers.
 *
 * Key Design Principles:
 * - Providers are permanently bound to descriptors at creation time
 * - Type information is captured using template parameters for safety
 * - Each descriptor represents a unique subsystem+provider combination
 * - Descriptors are value types and can be copied/stored
 * - Thread-safe for read operations, not thread-safe for creation
 *
 * Usage Example:
 * @code
 * // Create descriptor binding graphics subsystem to OpenGL provider
 * auto descriptor = EngineSystemDescriptor::Create<
 *     IGraphicsSubsystem,      // Interface type
 *     GraphicsSubsystem,       // Implementation type
 *     OpenGLGraphicsProvider   // Provider type
 * >("OpenGL Graphics System");
 *
 * // Register with manager
 * systemManager.RegisterSystemDescriptor(descriptor);
 *
 * // Create subsystem instance
 * auto* graphics = systemManager.CreateSystem<IGraphicsSubsystem>(descriptor);
 * @endcode
 *
 * Type Safety:
 * - SubsystemInterface must inherit from IEngineSubsystem
 * - SubsystemImpl must implement SubsystemInterface
 * - ProviderType must inherit from ISubsystemProvider
 * - All type relationships are validated at compile time
 *
 * @see EngineSystemManager for descriptor registration and usage
 * @see IEngineSubsystem for subsystem interface requirements
 * @see ISubsystemProvider for provider interface requirements
 */
class EngineSystemDescriptor {
public:
  /**
   * @brief Create a descriptor for a subsystem+provider combination.
   *
   * This is the primary factory method for creating descriptors. It captures
   * all type information at compile time and creates the binding between
   * subsystem implementation and provider.
   *
   * @tparam SubsystemInterface The interface type (must inherit from
   * IEngineSubsystem)
   * @tparam SubsystemImpl The implementation type (must implement
   * SubsystemInterface)
   * @tparam ProviderType The provider type (must inherit from
   * ISubsystemProvider)
   *
   * @param systemName Human-readable name for the subsystem
   * @return Configured descriptor ready for registration
   *
   * @pre SubsystemImpl must have a default constructor
   * @pre ProviderType must have a default constructor
   * @post Descriptor contains all type information for system creation
   */
  template <typename SubsystemInterface, typename SubsystemImpl,
            typename ProviderType>
  static EngineSystemDescriptor Create(const std::string &systemName);

  /**
   * @brief Create a subsystem instance using this descriptor.
   *
   * Creates and configures a subsystem instance with its bound provider.
   * The provider is automatically created and assigned to the subsystem.
   *
   * @tparam SubsystemInterface The interface type to create
   * @return Unique pointer to created subsystem, or nullptr on failure
   *
   * @pre SubsystemInterface must match the interface used in Create()
   * @post If successful: returned subsystem has provider assigned
   * @post If successful: subsystem->HasProvider() == true
   */
  template <typename SubsystemInterface>
  std::unique_ptr<SubsystemInterface> CreateSystem() const;

  /**
   * @brief Get hash code for the subsystem interface type.
   *
   * Used by EngineSystemManager for O(1) subsystem lookup.
   *
   * @return Type hash for interface type
   */
  size_t GetInterfaceHash() const { return interfaceHash; }

  /**
   * @brief Get hash code for the provider type.
   *
   * Used for provider type identification and validation.
   *
   * @return Type hash for provider type
   */
  size_t GetProviderTypeHash() const { return providerTypeHash; }

  /**
   * @brief Get unique system identifier.
   *
   * Each descriptor gets a unique ID for tracking purposes.
   *
   * @return Unique 32-bit system identifier
   */
  uint32_t GetSystemId() const { return systemId; }

  /**
   * @brief Get human-readable system name.
   *
   * Returns the name provided during Create() call.
   *
   * @return Display name for this system
   */
  const std::string &GetSystemName() const { return systemName; }

  /**
   * @brief Get interface type name for debugging.
   *
   * @return C++ type name for the subsystem interface
   */
  const std::string &GetInterfaceTypeName() const { return interfaceTypeName; }

  /**
   * @brief Get provider type name for debugging.
   *
   * @return C++ type name for the provider type
   */
  const std::string &GetProviderTypeName() const { return providerTypeName; }

  /**
   * @brief Get implementation identifier string.
   *
   * @return Provider-based implementation identifier
   */
  const std::string &GetImplementationId() const { return implementationId; }

  /**
   * @brief Compare descriptors for equality.
   *
   * Two descriptors are equal if they have the same interface and provider
   * types. System name and other metadata do not affect equality.
   *
   * @param other Descriptor to compare against
   * @return true if descriptors represent the same subsystem+provider
   * combination
   */
  bool operator==(const EngineSystemDescriptor &other) const {
    return interfaceHash == other.interfaceHash &&
           providerTypeHash == other.providerTypeHash;
  }

private:
  EngineSystemDescriptor(
      size_t iHash, size_t pHash, std::string name, std::string iTypeName,
      std::string pTypeName, std::string implId, uint32_t id,
      std::function<std::unique_ptr<IEngineSubsystem>()> createFunc);

  size_t interfaceHash;
  size_t providerTypeHash;
  std::string systemName;
  std::string interfaceTypeName;
  std::string providerTypeName;
  std::string implementationId; // Provider-based implementation identifier
  uint32_t systemId;

  // Creation function (type-erased for storage)
  std::function<std::unique_ptr<IEngineSubsystem>()> createFunction;

  static uint32_t nextSystemId;
  friend class EngineSystemManager;
};

// Template implementations
template <typename SubsystemInterface, typename SubsystemImpl,
          typename ProviderType>
EngineSystemDescriptor
EngineSystemDescriptor::Create(const std::string &systemName) {
  size_t interfaceHash =
      std::type_index(typeid(SubsystemInterface)).hash_code();
  size_t providerHash = std::type_index(typeid(ProviderType)).hash_code();

  // Create type-erased creation function
  auto createFunc = []() -> std::unique_ptr<IEngineSubsystem> {
    auto provider = std::make_unique<ProviderType>();
    auto subsystem = std::make_unique<SubsystemImpl>();
    subsystem->SetSubsystemProvider(std::move(provider));
    return std::move(subsystem);
  };

  return EngineSystemDescriptor(
      interfaceHash, providerHash, systemName,
      typeid(SubsystemInterface).name(), typeid(ProviderType).name(),
      typeid(ProviderType).name(), // Use provider type as implementation ID
      ++nextSystemId, createFunc);
}

template <typename SubsystemInterface>
std::unique_ptr<SubsystemInterface>
EngineSystemDescriptor::CreateSystem() const {
  // Call type-erased creation function and cast back to proper type
  auto subsystem = createFunction();
  SubsystemInterface *rawPtr =
      static_cast<SubsystemInterface *>(subsystem.release());
  return std::unique_ptr<SubsystemInterface>(rawPtr);
}

} // namespace AIEngine