#include "EngineSystemDescriptor.hpp"

#include <cstdint>

namespace AIEngine {

// Static member initialization
uint32_t EngineSystemDescriptor::nextSystemId = 0;

EngineSystemDescriptor::EngineSystemDescriptor(
    size_t iHash, size_t pHash, std::string name, std::string iTypeName,
    std::string pTypeName, std::string implId, uint32_t id,
    std::function<std::unique_ptr<IEngineSubsystem>()> createFunc)
    : interfaceHash(iHash), providerTypeHash(pHash),
      systemName(std::move(name)), interfaceTypeName(std::move(iTypeName)),
      providerTypeName(std::move(pTypeName)),
      implementationId(std::move(implId)), systemId(id),
      createFunction(std::move(createFunc)) {}

} // namespace AIEngine
