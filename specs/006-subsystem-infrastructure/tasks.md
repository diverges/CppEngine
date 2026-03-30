# Tasks: Core Engine Subsystem Infrastructure

**Input**: Design documents from `/specs/006-subsystem-infrastructure/`
**Prerequisites**: plan.md (required), spec.md (required for user stories), research.md, data-model.md, contracts/

**Tests**: Unit tests included for all infrastructure components as required by constitution

**Organization**: Tasks are grouped by user story to enable independent implementation and testing of each story.

## Format: `[ID] [P?] [Story] Description`

- **[P]**: Can run in parallel (different files, no dependencies)
- **[Story]**: Which user story this task belongs to (e.g., US1, US2, US3)
- Include exact file paths in descriptions

## Phase 1: Setup (Shared Infrastructure)

**Purpose**: Project initialization and basic structure

- [X] T001 Create header directory structure in engine/include/AIEngine/core/
- [X] T002 Create source directory structure in engine/src/core/
- [X] T003 [P] Create unit test directory structure in engine/tests/unit/

---

## Phase 2: Foundational (Blocking Prerequisites)

**Purpose**: Core infrastructure that MUST be complete before ANY user story can be implemented

**⚠️ CRITICAL**: No user story work can begin until this phase is complete

- [X] T004 [P] Implement ISubsystemProvider base interface in engine/include/AIEngine/core/ISubsystemProvider.hpp
- [X] T005 [P] Implement IEngineSubsystem base interface in engine/include/AIEngine/core/IEngineSubsystem.hpp
- [X] T006 [P] Create base provider implementation skeleton in engine/src/core/ISubsystemProvider.cpp
- [X] T007 [P] Create base subsystem implementation skeleton in engine/src/core/IEngineSubsystem.cpp

**Checkpoint**: Foundation ready - user story implementation can now begin in parallel

---

## Phase 3: User Story 1 - Engine System Registration & Creation (Priority: P1) 🎯 MVP

**Goal**: Enable developers to register new subsystem types and create instances without modifying core engine

**Independent Test**: Can be fully tested by creating a simple test subsystem, registering its descriptor, and verifying creation through EngineSystemManager

### Unit Tests for User Story 1

> **NOTE: Write these tests FIRST, ensure they FAIL before implementation**

- [X] T008 [P] [US1] Create EngineSystemDescriptor unit tests in engine/tests/unit/engine_system_descriptor_test.cpp
- [X] T009 [P] [US1] Create EngineSystemManager unit tests in engine/tests/unit/engine_system_manager_test.cpp

### Implementation for User Story 1

- [X] T010 [P] [US1] Implement EngineSystemDescriptor class in engine/include/AIEngine/core/EngineSystemDescriptor.hpp
- [X] T011 [P] [US1] Implement EngineSystemManager singleton class in engine/include/AIEngine/core/EngineSystemManager.hpp
- [X] T012 [US1] Implement EngineSystemDescriptor creation and metadata methods in engine/src/core/EngineSystemDescriptor.cpp
- [X] T013 [US1] Implement EngineSystemManager registry and creation logic in engine/src/core/EngineSystemManager.cpp
- [X] T014 [US1] Add type hash-based identification and single subsystem per interface validation
- [X] T015 [US1] Implement template-based CreateSystem and GetSystem methods with O(1) performance
- [X] T016 [US1] Add descriptor registration and duplicate rejection logic

**Checkpoint**: At this point, User Story 1 should be fully functional - developers can register and create subsystems

---

## Phase 4: User Story 2 - Reliable Engine Startup and Shutdown (Priority: P2)

**Goal**: Provide automatic lifecycle management for all subsystems with fail-fast error handling

**Independent Test**: Can be tested by repeatedly starting and stopping the engine and verifying no resources are leaked

### Unit Tests for User Story 2

- [X] T017 [P] [US2] Create lifecycle management unit tests in engine/tests/unit/subsystem_lifecycle_test.cpp
- [X] T018 [P] [US2] Create integration tests for complete lifecycle in engine/tests/unit/subsystem_infrastructure_test.cpp

### Implementation for User Story 2

- [X] T019 [US2] Implement InitializeAllSystems method with registration order processing in engine/src/core/EngineSystemManager.cpp
- [X] T020 [US2] Implement StartAllSystems method with running state management in engine/src/core/EngineSystemManager.cpp
- [X] T021 [US2] Implement StopAllSystems method with graceful shutdown in engine/src/core/EngineSystemManager.cpp
- [X] T022 [US2] Implement DestroyAllSystems method with reverse-order cleanup in engine/src/core/EngineSystemManager.cpp
- [X] T023 [US2] Add fail-fast error handling with detailed error reporting for initialization failures
- [X] T024 [US2] Add SystemEntry internal structure for tracking subsystem state (initialized, running)
- [X] T025 [US2] Integrate lifecycle management with existing Engine class Initialize/Shutdown methods

**Checkpoint**: At this point, User Stories 1 AND 2 should both work independently - complete lifecycle management

---

## Phase 5: User Story 3 - Cross-Platform Compatibility (Priority: P3)

**Goal**: Enable platform-specific implementations through provider pattern without rewriting core systems

**Independent Test**: Can be tested by switching between different platform implementations and verifying identical behavior

### Unit Tests for User Story 3

- [X] T026 [P] [US3] Create provider delegation unit tests in engine/tests/unit/provider_delegation_test.cpp
- [X] T027 [P] [US3] Create mock provider implementation for testing in engine/tests/unit/mock_provider_test.cpp

### Implementation for User Story 3

- [X] T028 [P] [US3] Implement provider assignment and validation in IEngineSubsystem::SetSubsystemProvider method
- [X] T029 [US3] Add provider delegation patterns to base subsystem implementation
- [X] T030 [US3] Implement provider lifecycle management (provider initialized before subsystem start)
- [X] T031 [US3] Add provider failure handling (subsystem initialization fails if provider fails)
- [X] T032 [US3] Implement type-safe provider access methods in base subsystem class
- [X] T033 [US3] Add provider identification and metadata support (provider name and ID)
- [X] T034 [US3] Create example subsystem with provider pattern in quickstart documentation

**Checkpoint**: All user stories should now be independently functional - complete subsystem infrastructure ✅

---

## Phase 6: Polish & Integration

**Purpose**: Cross-cutting concerns and integration with existing engine

- [X] T035 [P] Update existing Engine class to integrate with EngineSystemManager lifecycle
- [X] T036 [P] Add GetEngineSubsystem<T>() convenience macro implementation
- [X] T037 [P] Create comprehensive integration tests for full subsystem workflow
- [X] T038 [P] Update engine build system (Makefile) to include new subsystem infrastructure files
- [X] T039 [P] Add performance validation tests to verify O(1) lookup requirements
- [X] T040 [P] Code cleanup and documentation review across all infrastructure files
- [X] T041 [P] Run quickstart.md validation with example implementations

---

## Dependencies & Execution Order

### Phase Dependencies

- **Setup (Phase 1)**: No dependencies - can start immediately
- **Foundational (Phase 2)**: Depends on Setup completion - BLOCKS all user stories
- **User Stories (Phase 3+)**: All depend on Foundational phase completion
  - User stories can then proceed in parallel (if staffed)
  - Or sequentially in priority order (P1 → P2 → P3)
- **Polish (Final Phase)**: Depends on all user stories being complete

### User Story Dependencies

- **User Story 1 (P1)**: Can start after Foundational (Phase 2) - Core registration and creation functionality
- **User Story 2 (P2)**: Depends on User Story 1 completion - Lifecycle management extends creation functionality
- **User Story 3 (P3)**: Depends on User Story 1 completion - Provider pattern extends base subsystem functionality

### Within Each User Story

- Unit tests MUST be written and FAIL before implementation begins
- Header files before implementation files for same class
- Core implementation before integration with existing systems
- Story validation before moving to next priority

### Parallel Opportunities

- All Setup tasks marked [P] can run in parallel
- All Foundational tasks marked [P] can run in parallel (within Phase 2)
- All unit tests for a user story marked [P] can run in parallel
- Header file implementations marked [P] can run in parallel (different classes)
- Once US1 completes, US2 and US3 can be worked on in parallel by different team members

---

## Parallel Example: User Story 1

```bash
# Team member 1: Create and run failing tests
make test-unit-descriptor  # Should fail
make test-unit-manager     # Should fail

# Team member 2: Implement headers in parallel
# Work on EngineSystemDescriptor.hpp
# Team member 3: Work on EngineSystemManager.hpp

# After headers complete, implement source files
# Team member 1: EngineSystemDescriptor.cpp
# Team member 2: EngineSystemManager.cpp

# Validate tests now pass
make test-unit-descriptor  # Should pass
make test-unit-manager     # Should pass
```

---

## Implementation Strategy

### MVP First Approach

**Minimum Viable Product (MVP)**: User Story 1 only

- Provides core value: developers can add new subsystems without modifying engine code
- Demonstrates the subsystem registration and creation pattern
- Foundation for all other capabilities

**Incremental Delivery**:

1. **MVP Release**: US1 (Registration & Creation)
2. **V1.1 Release**: US1 + US2 (Adds Lifecycle Management)
3. **V1.2 Release**: US1 + US2 + US3 (Adds Cross-Platform Provider Support)

### Task Validation Strategy

Each task completion should be validated by:

1. **Unit tests pass**: All tests for that component/story
2. **Build succeeds**: make command completes without errors
3. **Integration works**: New code integrates with existing Engine class
4. **Performance criteria met**: O(1) lookup, <1ms operations, <10KB overhead

---

## Total Task Count: 41 Tasks

**Task Distribution**:

- Setup: 3 tasks
- Foundational: 4 tasks  
- User Story 1: 9 tasks (including tests)
- User Story 2: 9 tasks (including tests)
- User Story 3: 9 tasks (including tests)
- Polish & Integration: 7 tasks

**Parallel Opportunities Identified**: 23 tasks can run in parallel within their phases
**Independent Test Criteria**: Each user story has clear validation criteria for standalone testing
**Suggested MVP Scope**: User Story 1 (9 tasks) provides core infrastructure value
