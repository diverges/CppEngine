# User Story 3 Tasks: Unified Build Workflow (Priority: P2)

**Story Goal**: Developers can build the entire project (engine + testgame) using a single command from the repository root, without navigating between multiple directories or understanding the internal build dependencies.

**Independent Test**: Can be tested by running a single build command from the repository root and verifying that both the engine and testgame are built in the correct dependency order without manual intervention.

**Prerequisites**: User Story 1 (US1) foundational build system must be complete

---

## Implementation Tasks

### Root Build Orchestration (US3)

- [x] T033 [US3] Implement dependency chain in root `Makefile` where `testgame` target depends on `engine` target
- [x] T034 [P] [US3] Add `all` target to root `Makefile` that builds both debug and release variants
- [x] T035 [P] [US3] Implement incremental build detection using component timestamp comparison
- [x] T036 [P] [US3] Add build progress reporting to show engine → testgame build progression

### Individual Component Commands (US3)

- [x] T037 [P] [US3] Implement standalone `engine` target in root `Makefile` for engine-only builds
- [x] T038 [P] [US3] Implement standalone `testgame` target in root `Makefile` for testgame-only builds
- [x] T039 [P] [US3] Ensure individual component targets also respect `$(BIN_OUTPUT)` directory organization

### Test Command Integration (US3)

- [ ] T040 [P] [US3] Implement `test-unit` target in root `Makefile` that executes engine unit tests
- [ ] T041 [P] [US3] Implement `test-integration` target in root `Makefile` that runs testgame with validation
- [ ] T042 [P] [US3] Add `test` target that runs both unit and integration tests in sequence
- [x] T043 [US3] Implement `run` target that ensures debug build exists before launching testgame

### Build System Robustness (US3)

- [x] T044 [P] [US3] Add error handling for failed component builds (halt entire build process)
- [x] T045 [P] [US3] Implement proper cleanup of partial builds on build failure
- [x] T046 [P] [US3] Add build timing information and summary reporting

---

## User Story 3 Completion Criteria

### Independent Test Validation

1. **Unified Build Test**: `make debug` builds both components in correct order
2. **Individual Build Test**: `make engine` and `make testgame` work independently
3. **Incremental Test**: Changes to engine trigger testgame rebuild automatically
4. **Clean Test**: `make clean` removes all artifacts from centralized directories  
5. **Test Execution**: `make test-unit` and `make test-integration` work independently

### Success Metrics (from spec.md)

- **SC-010**: ✅ Single root build command builds entire project in under 5 minutes
- **SC-011**: ✅ Incremental builds complete in under 30 seconds  
- **SC-012**: ✅ Root clean command removes 100% of build artifacts
- **SC-014**: ✅ Unit and integration test commands work independently

### Deliverable

After completing User Story 3 tasks, the project provides:

- ✅ Complete root-level build orchestration with dependency management
- ✅ Individual component build capabilities (engine-only, testgame-only)
- ✅ Separate test execution commands (unit vs integration)
- ✅ Robust error handling and incremental build detection
- ✅ Centralized build artifact organization

**Dependencies**: User Story 1 completion (foundational build system)  
**Enables**: All other user stories can leverage unified build commands  
**Independent Value**: Dramatically simplified developer workflow and cognitive load reduction
