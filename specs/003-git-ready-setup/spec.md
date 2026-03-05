# Feature Specification: Git Ready Multi-Developer Setup

**Feature Branch**: `003-git-ready-setup`  
**Created**: March 4, 2026  
**Status**: Draft  
**Input**: User description: "Currently the project has been bootstrapped and is local only. We want to make the project Git Ready. That means multiple people on different window machines can easily checkout and build the project. We need to make sure there's no local machine paths on the project. Dependencies libs are checked into the project instead of using a package manager. And debuging and testing is possible out of the box with minimal configuration."

## Clarifications

### Session 2026-03-04

- Q: Build output organization - where should build artifacts be placed instead of scattered in individual source projects? → A: Build type subdirectories with a flat structure
- Q: Dependency version compatibility - how should version compatibility be maintained across different Windows environments and compiler toolchains? → A: Keep one version and update dependencies and toolchains as needed
- Q: IDE configuration scope - what extent of IDE configuration should be included in the repository? → A: Focus entirely on VS Code
- Q: Standard development hardware definition - what hardware specifications should build time expectations target? → A: At least mid range laptop
- Q: Test execution strategy - how should different test types be organized and executed from root commands? → A: Separate commands for unit tests and integration tests
- Q: Individual component build capability - should the root makefile provide commands for building individual components? → A: Root makefile should contain commands for building only engine or only testgame

## User Scenarios & Testing *(mandatory)*

### User Story 1 - Fresh Developer Onboarding (Priority: P1)

A new developer joins the team and can clone the repository and have a working development environment without manual configuration of paths, dependencies, or external tools.

**Why this priority**: This is the core value proposition - enabling seamless developer onboarding is critical for team productivity and project adoption.

**Independent Test**: Can be fully tested by having someone clone the repo on a fresh Windows machine and successfully build and run the project using only the included documentation and scripts.

**Acceptance Scenarios**:

1. **Given** a fresh Windows machine with Git installed, **When** developer clones the repository, **Then** all project dependencies are available locally without external downloads
2. **Given** the cloned repository, **When** developer runs the build command, **Then** the project builds successfully without any path configuration
3. **Given** a successful build, **When** developer attempts to run tests, **Then** all tests execute and produce results without additional setup

---

### User Story 2 - Cross-Machine Development (Priority: P2)

Developers can work on the project from multiple Windows machines without reconfiguring paths or rebuilding dependencies.

**Why this priority**: Enables flexible development workflows and ensures consistency across different development environments.

**Independent Test**: Can be tested by taking an existing project directory, copying it to a different Windows machine, and verifying it builds and runs without modification.

**Acceptance Scenarios**:

1. **Given** a project directory from Machine A, **When** copied to Machine B with different drive letters or folder structures, **Then** the project builds and runs identically
2. **Given** different Windows user accounts, **When** accessing the same project directory, **Then** all functionality works without permission or path issues

---

### User Story 3 - Unified Build Workflow (Priority: P2)

Developers can build the entire project (engine + testgame) using a single command from the repository root, without navigating between multiple directories or understanding the internal build dependencies.

**Why this priority**: Simplifies the development workflow and reduces cognitive load for new developers who shouldn't need to understand the internal project structure to perform basic build operations.

**Independent Test**: Can be tested by running a single build command from the repository root and verifying that both the engine and testgame are built in the correct dependency order without manual intervention.

**Acceptance Scenarios**:

1. **Given** a fresh repository checkout, **When** developer runs the root build command, **Then** both engine and testgame are built successfully in the correct dependency order
2. **Given** changes to engine code, **When** developer runs the root build command, **Then** the engine is rebuilt and testgame is automatically rebuilt to use the updated engine
3. **Given** a developer wants to clean the project, **When** they run the root clean command, **Then** both engine and testgame build artifacts are removed completely
4. **Given** a developer wants to run tests, **When** they execute the root unit test command, **Then** all engine unit tests are executed and results are reported
5. **Given** a need to build only one component, **When** developer runs individual component build commands from root, **Then** only the specified component (engine or testgame) is built successfully

---

### User Story 4 - IntelliSense and Code Navigation (Priority: P2)

Developers get full IntelliSense support, code completion, error detection, and navigation features in Visual Studio Code without additional configuration.

**Why this priority**: IntelliSense is essential for productive C++ development - without it, developers lose code completion, error detection, and navigation capabilities that significantly impact development speed and code quality.

**Independent Test**: Can be tested by opening the project in VS Code and verifying that code completion, error highlighting, go-to-definition, and symbol search work correctly for both engine and game code.

**Acceptance Scenarios**:

1. **Given** a fresh project checkout in VS Code, **When** developer types code in any C++ file, **Then** IntelliSense provides accurate code completion for project symbols, standard library, and dependencies
2. **Given** VS Code with the project open, **When** developer uses Ctrl+Click or F12 on a symbol, **Then** navigation jumps to the correct definition across engine and test code files
3. **Given** code with syntax or semantic errors, **When** developer views the file in VS Code, **Then** problems are highlighted with accurate error messages and suggestions
4. **Given** the project workspace, **When** developer uses Ctrl+Shift+O or Ctrl+T, **Then** symbol search finds and navigates to classes, functions, and variables across the entire codebase

---

### User Story 5 - Out-of-Box Debugging Experience (Priority: P3)

Developers can immediately debug the project using standard tools without additional configuration.

**Why this priority**: Reduces friction for debugging and development workflow, improving developer experience.

**Independent Test**: Can be tested by setting breakpoints in the code and verifying the debugger attaches and functions correctly with default project configuration.

**Acceptance Scenarios**:

1. **Given** a fresh project checkout, **When** developer opens the project in VS Code, **Then** debugging configurations work without manual setup
2. **Given** a running debug session, **When** developer sets breakpoints in engine and test code, **Then** breakpoints trigger and provide full debugging information

---

### Edge Cases

- What happens when the project is cloned to a path with spaces or special characters?
- How does the system handle different Windows versions (10, 11) with varying compiler versions?
- What occurs if a developer tries to build on a machine without administrative privileges?
- How does the build process handle network restrictions that might block external tool downloads?
- What happens when only the engine or only the testgame has changes - does the build system detect this correctly?
- How does the unified build system handle failed builds in one component - does it continue or halt?
- What occurs if someone manually runs make in a subdirectory while a root build is in progress?

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: System MUST build successfully on any Windows machine without requiring path modifications
- **FR-002**: System MUST include all required dependencies within the repository structure
- **FR-003**: Build scripts MUST use relative paths exclusively, with no hardcoded absolute paths
- **FR-004**: System MUST provide working VS Code debugger configurations without manual setup
- **FR-005**: Test suite MUST execute successfully without external dependency installation
- **FR-006**: Documentation MUST provide clear, step-by-step build instructions for Windows
- **FR-007**: Project MUST validate build environment compatibility during setup
- **FR-008**: System MUST support building from any directory location on Windows file systems
- **FR-009**: VS Code workspace MUST provide complete IntelliSense configuration for C++ without manual setup
- **FR-010**: IntelliSense MUST recognize all project headers, dependencies, and standard library includes
- **FR-011**: Code navigation MUST work across all project modules (engine, tests, game code)
- **FR-012**: Root-level build system MUST coordinate engine and testgame builds in correct dependency order
- **FR-013**: Single build command MUST build both engine and testgame components from repository root
- **FR-014**: Build system MUST detect changes and rebuild only necessary components (incremental builds)
- **FR-015**: Root-level commands MUST be available for clean, test-unit, test-integration, and run operations across all components
- **FR-016**: Build artifacts MUST be organized in root `bin/` directory with build type subdirectories (e.g., `bin/debug/`, `bin/release/`) using flat file structure
- **FR-017**: Dependencies MUST maintain single stable versions that are updated together with toolchain requirements
- **FR-018**: VS Code workspace MUST include complete configuration for C++ development (tasks, debugging, IntelliSense) without requiring additional setup
- **FR-019**: Test execution MUST provide separate commands for unit tests and integration tests that can be run independently  
- **FR-020**: Root-level build system MUST provide individual component build commands to build only engine or only testgame when needed

### Key Entities

- **Build Configuration**: Makefile settings, compiler flags, and path resolution mechanisms that ensure portability
- **Dependency Bundle**: Collection of third-party libraries (SDL2, OpenGL, testing frameworks) stored locally in the repository
- **Debug Configuration**: VS Code-specific settings files (.vscode/launch.json, .vscode/tasks.json) that enable immediate debugging capability
- **Environment Validation**: Scripts or tools that verify the local environment meets project requirements
- **Unified Build System**: Root-level build orchestration that coordinates engine and testgame builds with proper dependency management

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: New developers can complete their first successful build within 10 minutes of cloning the repository
- **SC-002**: Project builds successfully on 100% of tested Windows machines (Windows 10 and 11) without configuration changes
- **SC-003**: All tests pass on freshly cloned instances without requiring additional downloads or installations
- **SC-004**: Debugging session can be started within 2 minutes of opening the project in VS Code
- **SC-005**: Project directory can be moved between different drive letters and folder structures without breaking functionality
- **SC-006**: Build process completes in under 5 minutes on mid-range development hardware (8GB RAM, 4-core CPU)
- **SC-007**: IntelliSense provides code completion suggestions within 2 seconds of opening any C++ file in VS Code
- **SC-008**: Go-to-definition navigation works for 100% of project symbols and dependency functions
- **SC-009**: Syntax and semantic errors are highlighted within 5 seconds of typing in VS Code
- **SC-010**: Single root build command successfully builds entire project (engine + testgame) in under 5 minutes
- **SC-011**: Incremental builds after code changes complete in under 30 seconds for typical modifications
- **SC-012**: Root-level clean command removes 100% of build artifacts from all project components
- **SC-013**: All build artifacts are organized in root `bin/debug/` and `bin/release/` directories with flat structure (no scattered outputs)
- **SC-014**: Unit test and integration test commands can be executed independently with clear result reporting

## Assumptions

- Developers have Git installed on their Windows machines
- Developers have a compatible C++ compiler toolchain (MSYS2/MinGW or Visual Studio)
- VS Code is the primary development environment
- Network access is available for initial git clone but not required for building
- Standard Windows development tools (make, debuggers) are available through MSYS2 or Visual Studio installation
- Developers use mid-range or better development hardware (minimum 8GB RAM, 4-core CPU)

## Dependencies

- MSYS2/MinGW toolchain must remain compatible with checked-in dependencies
- SDL2, OpenGL libraries, and testing frameworks use stable versions that are updated as a unit when toolchain requirements change
- Git repository structure supports large binary dependencies without performance issues
- Dependency updates must maintain backward compatibility within major versions

## Exclusions

- Support for non-Windows operating systems (Linux, macOS) is not part of this feature
- Integration with package managers (vcpkg, Conan) is explicitly excluded - all dependencies are checked in
- Automated CI/CD pipeline setup is not included in this scope
- Docker-based development environments are not part of this feature
