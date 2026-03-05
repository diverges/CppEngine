# User Story 2 Tasks: Cross-Machine Development (Priority: P2)

**Story Goal**: Developers can work on the project from multiple Windows machines without reconfiguring paths or rebuilding dependencies.

**Independent Test**: Can be tested by taking an existing project directory, copying it to a different Windows machine, and verifying it builds and runs without modification.

**Prerequisites**: User Story 1 (US1) foundational build system must be complete

---

## Implementation Tasks

### Path Portability (US2)

- [x] T023 [P] [US2] Audit `engine/Makefile` for any hardcoded absolute paths and replace with relative paths
- [x] T024 [P] [US2] Audit `testgame/Makefile` for any hardcoded absolute paths and replace with relative paths  
- [x] T025 [P] [US2] Verify all dependency include paths in `engine/deps/` use relative paths only
- [x] T026 [P] [US2] Test build system with paths containing spaces and special characters

### Cross-Machine Compatibility (US2)

- [x] T027 [US2] Implement path normalization in root `Makefile` using `$(CURDIR)` for absolute path generation
- [x] T028 [P] [US2] Add Windows path compatibility handling (forward slashes) to all Makefiles
- [x] T029 [P] [US2] Validate that build works from any drive letter (C:, D:, etc.)

### Environment Independence (US2)

- [x] T030 [P] [US2] Ensure no dependency on user-specific environment variables in build process
- [x] T031 [P] [US2] Verify build system works without administrative privileges  
- [x] T032 [P] [US2] Test build consistency between different Windows user accounts

---

## User Story 2 Completion Criteria

### Independent Test Validation

1. **Copy Test**: Project directory copied between machines builds identically
2. **Drive Letter Test**: Build works from different drive letters (C:\, D:\, E:\)
3. **User Account Test**: Build works under different Windows user accounts
4. **Path Characters Test**: Build succeeds with repository in path containing spaces/special chars

### Success Metrics (from spec.md)

- **SC-005**: ✅ Project directory movable between different drive letters and folder structures  
- **SC-002**: ✅ Project builds successfully on 100% of tested Windows machines

### Deliverable

After completing User Story 2 tasks, the project provides:

- ✅ Completely portable build system (no machine-specific paths)
- ✅ Robust directory handling (spaces, special characters, different drives)
- ✅ User account independence (no permission issues)
- ✅ Validated cross-machine consistency

**Dependencies**: None after US1 foundation  
**Enables**: Reliable base for US3 (Build Workflow), US4 (IntelliSense), US5 (Debugging)  
**Independent Value**: Teams can share project directories without configuration issues
