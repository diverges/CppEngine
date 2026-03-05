# User Story 5 Tasks: Out-of-Box Debugging Experience (Priority: P3)

**Story Goal**: Developers can immediately debug the project using standard tools without additional configuration.

**Independent Test**: Can be tested by setting breakpoints in the code and verifying the debugger attaches and functions correctly with default project configuration.

**Prerequisites**: User Story 1 (US1) foundational build system and User Story 4 (US4) IntelliSense configuration must be complete

---

## Implementation Tasks

### VS Code Debug Configuration (US5)

- [x] T079 [P] [US5] Create `.vscode/launch.json` with debug configurations for testgame and engine tests
- [x] T080 [P] [US5] Configure "Debug TestGame" launch configuration targeting `bin/debug/testgame.exe`
- [x] T081 [P] [US5] Configure "Debug Engine Tests" launch configuration targeting `bin/debug/engine_tests.exe`  
- [x] T082 [P] [US5] Set up GDB debugger path configuration for MSYS2/MinGW installations
- [x] T083 [P] [US5] Configure pre-launch tasks to ensure debug builds are current

### Debug Symbol Configuration (US5)

- [x] T084 [P] [US5] Add debug symbol generation flags to engine and testgame Makefiles for debug builds
- [x] T085 [P] [US5] Ensure debug information is preserved during linking process  
- [x] T086 [P] [US5] Configure source-level debugging with proper source file path mapping

### Debug Environment Setup (US5)

- [x] T087 [P] [US5] Configure working directory for debug sessions to repository root
- [x] T088 [P] [US5] Set up environment variables for debug sessions if needed (SDL, OpenGL)
- [x] T089 [P] [US5] Configure console output for debug sessions (external vs integrated)

### Debug Workflow Integration (US5)

- [x] T090 [P] [US5] Test F5 debugging functionality from VS Code without manual configuration
- [x] T091 [P] [US5] Verify breakpoints work across engine source code, headers, and testgame code
- [x] T092 [P] [US5] Test variable inspection for engine objects and game state during debugging
- [x] T093 [P] [US5] Configure debug console for C++ expression evaluation

---

## User Story 5 Completion Criteria

### Independent Test Validation

1. **Immediate Debug Test**: ✅ F5 debugging starts without manual configuration
2. **Breakpoint Test**: ✅ Breakpoints trigger correctly in both engine and testgame code
3. **Variable Inspection Test**: ✅ Engine objects and game state visible in debug variables panel
4. **Debug Console Test**: ✅ C++ expressions can be evaluated in debug console
5. **Integration Test**: ✅ Debugging works seamlessly with build tasks from User Story 4

### Success Metrics (from spec.md)

- **SC-004**: ✅ Debugging session starts within 2 minutes of opening project in VS Code

### Deliverable

After completing User Story 5 tasks, the project provides:

- ✅ Complete VS Code debug configuration for multi-component project
- ✅ Immediate debugging capability with F5 keyboard shortcut
- ✅ Cross-component debugging (engine and testgame code)
- ✅ Integrated debug workflow with build tasks
- ✅ Variable inspection and debug console functionality

**Dependencies**: User Story 1 (build foundation) + User Story 4 (IntelliSense configuration)  
**Independent Value**: Eliminates debugging setup friction and provides professional development debugging experience  
**Development Impact**: Reduces debugging workflow complexity and improves troubleshooting efficiency
