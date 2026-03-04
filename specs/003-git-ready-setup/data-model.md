# Data Model: Git Ready Multi-Developer Setup

**Feature**: 003-git-ready-setup  
**Created**: March 4, 2026  
**Purpose**: Define the key entities and relationships for multi-developer build orchestration

## Entities

### Build Configuration

**Purpose**: Represents the build system orchestration settings that ensure cross-machine compatibility

**Attributes**:

- **variant**: Build type (debug | release)
- **outputPath**: Target directory for build artifacts (`bin/debug/` | `bin/release/`)  
- **compilerFlags**: Platform and variant-specific compilation settings
- **linkFlags**: Library linking configuration for component dependencies

**Relationships**:

- Coordinates multiple Component Build instances
- Consumed by VS Code Tasks for IDE integration

**Validation Rules**:

- outputPath must be relative to repository root
- variant must be one of predefined types (debug, release)
- All paths must use forward slashes for cross-platform compatibility

---

### Component Build

**Purpose**: Represents individual buildable components (engine, testgame) with their specific requirements

**Attributes**:

- **componentName**: Component identifier (engine | testgame)
- **sourceDirectory**: Component source location (engine/ | testgame/)
- **buildTargets**: Available make targets (all, clean, test)
- **dependencies**: Other components this component requires
- **artifactType**: Type of output (static library | executable)
- **outputName**: Final artifact name (libAIEngine.a | testgame.exe)

**Relationships**:

- Engine component has zero dependencies
- TestGame component depends on Engine component
- Both produce artifacts in shared Build Output Location

**State Transitions**:

- Not Built → Building (when make target invoked)
- Building → Built Successfully | Build Failed
- Built → Not Built (when source files change)

---

### Dependency Bundle  

**Purpose**: Represents external libraries checked into the repository for cross-machine consistency

**Attributes**:

- **libraryName**: External dependency name (SDL2, OpenGL, doctest)
- **version**: Specific version checked into repository  
- **includePath**: Header file location within `engine/deps/`
- **libraryPath**: Compiled library location within `engine/deps/`
- **platformSupport**: Supported compilation targets

**Relationships**:

- Used by Component Build for compilation and linking
- Managed by Environment Validation for compatibility checking

**Validation Rules**:

- All include paths must be relative to `engine/deps/`
- Version must be compatible with current compiler toolchain  
- Platform support must include Windows (minimum requirement)

---

### VS Code Configuration

**Purpose**: IDE workspace settings that provide immediate productivity without manual setup

**Attributes**:

- **includeDirectories**: List of include paths for IntelliSense
- **buildTasks**: VS Code tasks that integrate with root Makefile
- **debugConfigurations**: Launch configurations for engine tests and testgame
- **workspaceSettings**: C++ language server and formatting options

**Relationships**:

- References Component Build source directories for include paths
- Integrates with Build Configuration for task definitions
- Uses Build Output Location for debug target specification

**Validation Rules**:

- All paths must be workspace-relative
- Build tasks must correspond to available Makefile targets
- Debug configurations must reference actual executable locations

---

### Build Output Location

**Purpose**: Centralized artifact storage that eliminates scattered build outputs

**Attributes**:

- **baseDirectory**: Root output directory (`bin/`)
- **variantSubdirectories**: Build type organization (debug/, release/)
- **artifactLayout**: Flat structure within variant directories
- **cleanupScope**: Directories managed by clean operations

**Relationships**:

- Contains artifacts from all Component Builds
- Referenced by VS Code Configuration for debug paths  
- Managed by Build Configuration for output coordination

**Validation Rules**:

- Base directory must be at repository root level
- Variant subdirectories must not contain conflicting artifact names
- All artifacts must be removable via single clean command

---

### Environment Validation

**Purpose**: Verification system that ensures local environment meets project requirements

**Attributes**:

- **compilerVersion**: Required compiler toolchain details
- **makeVersion**: GNU Make version compatibility requirements
- **platformRequirements**: Windows version and architecture constraints  
- **hardwareBaseline**: Minimum specifications for build performance

**Relationships**:  

- Validates Dependency Bundle compatibility
- Ensures Build Configuration can execute successfully
- Provides feedback for developer onboarding process

**Validation Rules**:

- Must complete successfully before first build attempt
- Should provide actionable error messages for missing requirements
- Must be executable without external dependencies

## Entity Relationships

```
Environment Validation
    ↓ validates
Build Configuration
    ↓ coordinates  
Component Build (engine) → Build Output Location
    ↓ depends on
Component Build (testgame) → Build Output Location
    ↑ 
VS Code Configuration
    ↓ references
Dependency Bundle
```

## State Flow

1. **Environment Validation** verifies toolchain compatibility
2. **Build Configuration** establishes output directories and build variant
3. **Component Build (engine)** produces engine library in centralized location
4. **Component Build (testgame)** uses engine library and produces executable  
5. **VS Code Configuration** provides IDE integration for all components
6. **Build Output Location** contains all final artifacts for easy distribution

## Cross-Component Dependencies

- **TestGame → Engine**: Requires engine static library for linking
- **VS Code Tasks → Build Configuration**: Uses same make targets and output locations  
- **Debug Configuration → Build Outputs**: References executable locations for debugging
- **All Components → Dependency Bundle**: Share common external libraries
- **Clean Operations → All Outputs**: Single command removes all build artifacts
