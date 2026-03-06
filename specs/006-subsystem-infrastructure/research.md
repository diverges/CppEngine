# Research: Core Engine Subsystem Infrastructure

**Phase**: 0 - Research & Technical Decisions  
**Date**: March 4, 2026  
**Feature**: [spec.md](spec.md)

## Research Findings

### Subsystem Identification Strategy

**Decision**: TypeID hash-based identification with single subsystem per interface constraint

**Rationale**:

- C++17 provides `std::type_index` and `typeid()` for compile-time type identification
- Hash-based lookups provide O(1) performance as required
- Single subsystem per interface simplifies the initial implementation significantly
- Avoids complexity of multi-implementation selection logic

**Alternatives considered**:

- String-based identification: Too slow, prone to typos
- Enum-based IDs: Not extensible for third-party subsystems
- UUID-based: Overkill for internal engine architecture

**Implementation approach**:

```cpp
template<typename SubsystemInterface>
size_t GetTypeHash() {
    return typeid(SubsystemInterface).hash_code();
}
```

### Lifecycle Management Integration

**Decision**: Engine class directly manages EngineSystemManager lifecycle

**Rationale**:

- Fits naturally with existing Engine::Initialize/Shutdown pattern
- Game code stays focused on game logic, not engine internals
- Central point of control prevents race conditions and ordering issues
- Clear error handling path through Engine initialization

**Implementation approach**:

- Engine::Initialize() calls EngineSystemManager::InitializeAllSystems()
- Engine::Shutdown() calls EngineSystemManager::DestroyAllSystems()
- Registration happens before Engine::Initialize(), lookup after

### Error Handling Strategy

**Decision**: Fail-fast with detailed logging

**Rationale**:

- Easier debugging when problems are caught immediately
- Prevents cascading failures from partially initialized state
- Clear error messages help developers fix configuration issues
- Aligns with development/debugging focus rather than production tolerance

**Implementation approach**:

- Return detailed error information from Initialize() methods
- Engine::Initialize() returns false on any subsystem failure
- Error messages include subsystem name, failure reason, and suggested fixes

### Threading Model

**Decision**: Single main thread with no mutex protection

**Rationale**:

- Matches current engine architecture (single-threaded)
- Eliminates complexity of thread synchronization
- Performance overhead of mutexes not needed for single-thread access
- Can be extended later if multi-threading becomes necessary

**Implementation approach**:

- No mutex or atomic operations needed
- All EngineSystemManager calls happen on main thread
- Documentation clearly states single-threaded requirement

### Dependency Management

**Decision**: Registration order determines initialization order

**Rationale**:

- Simple to implement and understand
- Gives developers explicit control over dependency order
- Avoids complexity of dependency graph resolution
- Can be enhanced later with explicit dependency declaration

**Implementation approach**:

- Store descriptors in registration order (std::vector)
- Initialize subsystems in same order during Engine::Initialize()
- Clear documentation about registration order requirements

## Technical Architecture

### Core Components

1. **IEngineSubsystem**: Pure virtual base class defining lifecycle interface
2. **ISubsystemProvider**: Pure virtual base class for platform implementations  
3. **EngineSystemDescriptor**: Value type containing creation logic and metadata
4. **EngineSystemManager**: Singleton managing registry and active instances

### Memory Management

- Smart pointers throughout (std::unique_ptr for ownership)
- RAII principles for automatic cleanup
- No raw memory allocation in core infrastructure
- Provider instances owned by subsystems, subsystems owned by manager

### Performance Characteristics

- Subsystem lookup: O(1) via unordered_map with type hash keys
- Registration: O(1) insertion
- Initialization: O(n) where n = number of registered subsystems
- Memory overhead: ~40-80 bytes per registered subsystem (descriptor storage)

## Implementation Dependencies

### Required from Previous Features

- Basic Engine class with Initialize/Shutdown lifecycle ✅ (from 001-bootstrap-project)
- Component system architecture ✅ (existing in codebase)
- doctest testing framework ✅ (existing in codebase)

### Standard Library Requirements

- `<unordered_map>` for fast subsystem lookup
- `<memory>` for smart pointer support
- `<typeinfo>` and `<typeindex>` for type identification
- `<vector>` for ordered storage of descriptors
- `<functional>` for creation function storage

### No External Dependencies

This infrastructure uses only standard library components, maintaining the constitutional requirement for minimal external dependencies.

## Risk Mitigation

### Type Safety

- Template-based interfaces prevent type confusion
- Compile-time type checking via C++ type system
- Clear error messages for type mismatches

### Performance

- Hash-based lookup provides required O(1) performance
- Zero runtime allocation for subsystem access
- Minimal memory overhead per subsystem

### Maintainability  

- Clear separation of interface, implementation, and provider
- Self-contained descriptors encapsulate creation logic
- Comprehensive unit test coverage planned
