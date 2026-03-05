# VS Code Integration Interface Contract

**Purpose**: Define VS Code workspace configuration interface for seamless C++ development
**Type**: VS Code workspace files and settings
**Scope**: .vscode/ directory configuration files

## Workspace Configuration Files

### c_cpp_properties.json Contract

**Purpose**: Configure IntelliSense for accurate code analysis and completion

```json
{
    "configurations": [
        {
            "name": "AIEngine-Windows",
            "includePath": [
                "${workspaceFolder}/engine/include",
                "${workspaceFolder}/engine/deps/SDL2/include",
                "${workspaceFolder}/engine/deps/glad/include",
                "${workspaceFolder}/engine/deps/glm",
                "${workspaceFolder}/engine/deps/doctest"
            ],
            "compilerPath": "C:/msys64/ucrt64/bin/g++.exe",
            "cStandard": "c17",
            "cppStandard": "c++17",
            "intelliSenseMode": "gcc-x64",
            "compilerArgs": [
                "-Wall",
                "-Wextra", 
                "-DSDL_MAIN_HANDLED"
            ]
        }
    ],
    "version": 4
}
```

**Contracts**:

- **includePath**: Must contain all directories needed for symbol resolution
- **compilerPath**: Must be auto-detectable or provide fallback configuration
- **cppStandard**: Must match constitution requirement (C++17 minimum)
- **compilerArgs**: Must match actual build flags used in Makefiles

### tasks.json Contract

**Purpose**: Integrate VS Code build tasks with root Makefile system

```json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "Build Debug",
            "type": "shell", 
            "command": "make",
            "args": ["debug"],
            "group": {
                "kind": "build",
                "isDefault": true
            },
            "presentation": {
                "echo": true,
                "reveal": "always",
                "focus": false,
                "panel": "shared"
            },
            "problemMatcher": "$gcc"
        },
        {
            "label": "Build Release", 
            "type": "shell",
            "command": "make", 
            "args": ["release"],
            "group": "build"
        },
        {
            "label": "Clean All",
            "type": "shell",
            "command": "make",
            "args": ["clean"]
        },
        {
            "label": "Run Unit Tests",
            "type": "shell", 
            "command": "make",
            "args": ["test-unit"],
            "group": "test"
        },
        {
            "label": "Run Integration Tests",
            "type": "shell",
            "command": "make", 
            "args": ["test-integration"],
            "group": "test"
        },
        {
            "label": "Run TestGame",
            "type": "shell",
            "command": "make",
            "args": ["run"],
            "group": "test"
        }
    ]
}
```

**Contracts**:

- **Default Build Task**: "Build Debug" is the default (Ctrl+Shift+P → "Run Build Task")
- **Problem Matcher**: "$gcc" enables error navigation from Problems panel
- **Task Labels**: Must be descriptive and match available Makefile targets
- **Working Directory**: All tasks run from workspace root

### launch.json Contract

**Purpose**: Configure debugging for both engine tests and testgame executable

```json  
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Debug TestGame",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/bin/debug/testgame.exe",
            "args": [],
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}",
            "environment": [],
            "externalConsole": false,
            "MIMode": "gdb",
            "miDebuggerPath": "C:/msys64/ucrt64/bin/gdb.exe",
            "preLaunchTask": "Build Debug"
        },
        {
            "name": "Debug Engine Tests",
            "type": "cppdbg", 
            "request": "launch",
            "program": "${workspaceFolder}/bin/debug/engine_tests.exe",
            "args": [],
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}",
            "MIMode": "gdb",
            "miDebuggerPath": "C:/msys64/ucrt64/bin/gdb.exe",
            "preLaunchTask": "Build Debug"
        }
    ]
}
```

**Contracts**:

- **Program Paths**: Must reference actual executable locations in bin/debug/
- **Pre-Launch Tasks**: Must trigger appropriate build before debugging
- **Debugger Path**: Should be configurable for different MSYS2 installations
- **Working Directory**: Consistent workspace root for all configurations

### settings.json Contract  

**Purpose**: Optimize VS Code for C++ development productivity

```json
{
    "files.associations": {
        "*.hpp": "cpp",
        "*.inl": "cpp"  
    },
    "C_Cpp.default.cppStandard": "c++17",
    "C_Cpp.default.compilerPath": "C:/msys64/ucrt64/bin/g++.exe",
    "C_Cpp.errorSquiggles": "enabled",
    "C_Cpp.clang_format_style": "{ BasedOnStyle: Google, IndentWidth: 4, ColumnLimit: 100 }",
    "editor.formatOnSave": true,
    "files.exclude": {
        "**/build/": true,
        "**/*.o": true,
        "**/*.d": true
    }
}
```

**Contracts**:

- **File Associations**: Ensure .hpp and .inl files get C++ syntax highlighting
- **Compiler Path**: Must point to actual compiler in toolchain
- **Format on Save**: Automatic code formatting with consistent style
- **File Exclusions**: Hide build artifacts from VS Code file explorer

## Integration Requirements

### Automatic Configuration Detection

- IntelliSense must resolve all symbols without "red squigglies"
- Code completion must include project classes, functions, and dependency APIs
- Go-to-definition must work across engine, testgame, and dependency code
- Error detection must show problems in real-time as developers type

### Build Integration

- Keyboard shortcuts (Ctrl+Shift+B) trigger default build task
- Build output appears in integrated terminal with error navigation
- Failed builds highlight problems in Problems panel with clickable navigation
- Successful builds show completion status and artifact locations

### Debug Integration  

- F5 launches default debug configuration (TestGame)
- Breakpoints work throughout engine and testgame code
- Variable inspection includes engine objects and game state
- Debug console provides access to C++ expressions and function calls

## Customization Guidelines

### Required Configuration  

Developers **MUST NOT** need to modify any .vscode/ files for basic functionality

### Optional Customizations

Developers **MAY** customize:

- Additional debug configurations for specific scenarios  
- Personal editor preferences (themes, keybindings)
- Extended build tasks for personal workflows
- Compiler-specific optimizations

### Preservation Requirements

Custom developer additions **MUST** be preserved during:

- Configuration updates from feature development
- Repository pulls that update .vscode/ files  
- Migration to new VS Code versions

## Platform Considerations

### Windows-Specific Paths

- Use forward slashes where possible: `engine/include` vs `engine\\include`
- Compiler paths may vary by MSYS2 installation location
- Executable extensions automatic (.exe) for proper platform detection

### Compatibility Requirements

- Configuration must work with VS Code 1.75+
- Compatible with C++ extension pack (ms-vscode.cpptools)
- Does not require additional VS Code extensions beyond C++ support
- Graceful degradation if specific tools (gdb) not found in expected locations
