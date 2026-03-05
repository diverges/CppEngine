# AIEngine Developer Onboarding Checklist

**Purpose**: Step-by-step guide for new developers joining the AIEngine project  
**Time to Complete**: ~15 minutes  
**Prerequisites**: Windows 10/11 with administrator access

## 🎯 Quick Start (5 minutes)

### Step 1: Prerequisites ✅

Verify you have these installed:

- [ ] **Git** - `git --version` should work
- [ ] **MSYS2** with MinGW-w64 - [Download here](https://www.msys2.org/)
- [ ] **VS Code** with C++ extension pack - [Download here](https://code.visualstudio.com/)

### Step 2: MSYS2 Setup ✅

Open MSYS2 terminal and install toolchain:

```bash
# Update package database
pacman -Syu

# Install development tools
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-make

# Install dependencies (already bundled, but useful for future projects)
pacman -S mingw-w64-x86_64-sdl2 mingw-w64-x86_64-glew mingw-w64-x86_64-glm

# Add to Windows PATH: C:\msys64\mingw64\bin
```

**Verification**:

- [ ] `g++ --version` shows MinGW compiler
- [ ] `make --version` shows GNU Make

## 🚀 Project Setup (5 minutes)

### Step 3: Clone and Build ✅

```bash
# Clone repository
git clone <repository-url> AIEngine
cd AIEngine

# Build the complete project
make debug
```

**Expected Output**:

- [ ] "Building engine (debug)..." and success message
- [ ] "Building testgame (debug)..." and success message  
- [ ] Files created: `bin/debug/libAIEngine.a`, `bin/debug/testgame.exe`

### Step 4: Verify Execution ✅

```bash
# Run the demo application
make run
```

**Expected Results**:

- [ ] Window opens (1280x720 resolution)
- [ ] 3D rotating cube is visible
- [ ] FPS counter shows 60+ FPS
- [ ] ESC key closes application cleanly
- [ ] No error messages in terminal

## 🛠️ VS Code Development (3 minutes)

### Step 5: VS Code Integration ✅

```bash
# Open workspace
code .
```

**Verify VS Code Features**:

- [ ] **IntelliSense**: Auto-completion works in C++ files
- [ ] **Build Tasks**: Ctrl+Shift+P → "Tasks: Run Task" shows build options
- [ ] **Debugging**: Press F5 starts debugging testgame.exe
- [ ] **Error Highlighting**: Red squiggles appear for syntax errors
- [ ] **Go to Definition**: F12 works on AIEngine classes

### Step 6: Debug Workflow Test ✅

1. **Set Breakpoint**: Open `testgame/src/main.cpp`, click line number margin
2. **Start Debugging**: Press F5
3. **Verify Functionality**:
   - [ ] Debugger attaches and stops at breakpoint
   - [ ] Variables panel shows local variables
   - [ ] Debug console accepts C++ expressions
   - [ ] Continue (F5) resumes execution properly

## ✅ Development Workflow

### Daily Development Commands ✅

```bash
# Build debug variant (development)
make debug

# Build release variant (testing/distribution)  
make release

# Run the current build
make run

# Clean and rebuild everything
make clean && make debug

# Run tests (when available)
make test
```

### VS Code Shortcuts ✅

- **F5**: Start debugging
- **F12**: Go to definition
- **Shift+F12**: Find all references
- **Alt+O**: Switch between header/source
- **Ctrl+T**: Symbol search
- **Ctrl+Shift+P**: Command palette

## 🧪 Verification Tests

### Test 1: Fresh Build Cycle ✅

```bash
make clean
make debug
make run
```

**Success Criteria**:

- [ ] Clean removes all build artifacts
- [ ] Debug build completes without errors
- [ ] Application runs successfully

### Test 2: Component Development ✅

1. **Modify Engine**: Edit any file in `engine/src/` or `engine/include/`
2. **Build**: `make debug` (should rebuild only changed components)
3. **Test**: Application should reflect changes

**Success Criteria**:

- [ ] Incremental builds work correctly
- [ ] Changes are reflected in running application
- [ ] No link errors or missing dependencies

### Test 3: Cross-Component Development ✅

1. **Edit Game**: Modify `testgame/src/main.cpp`
2. **Edit Engine**: Modify `engine/src/core/Engine.cpp`
3. **Build**: `make debug`
4. **Debug**: Set breakpoints in both files, press F5

**Success Criteria**:

- [ ] Both components rebuild correctly
- [ ] Debugger works across engine and game code
- [ ] Variable inspection works for both components

## 🎯 Next Steps

After completing this checklist, you're ready to:

- [ ] **Read Architecture**: Check `docs/architecture.md` for engine design
- [ ] **Explore API**: Review `include/AIEngine/` headers for public API  
- [ ] **Run Tests**: Execute `make test` when unit tests are available
- [ ] **Create Features**: Follow component-driven development patterns
- [ ] **Submit PRs**: Use standard Git workflow for contributions

## 🆘 Troubleshooting

### Common Issues

**Build fails with "g++ not found"**:

- Solution: Add `C:\msys64\mingw64\bin` to Windows PATH
- Verify: `g++ --version` should work in PowerShell

**VS Code IntelliSense not working**:

- Solution: Install "C++ Extension Pack" from marketplace
- Verify: Green checkmark on C++ files in status bar

**Debugging doesn't start**:

- Solution: Ensure VS Code C++ extension is installed and debug build exists
- Verify: `bin/debug/testgame.exe` file should exist

**Application window doesn't open**:

- Solution: Check graphics drivers and OpenGL support
- Verify: Update graphics drivers for your hardware

### Getting Help

- **Engine Architecture**: See `docs/architecture.md`
- **API Documentation**: Headers in `include/AIEngine/`  
- **Build System**: Root `Makefile` and component-specific Makefiles
- **Project Specs**: Feature specifications in `specs/` directory

**Estimated Total Time**: ~15 minutes for complete onboarding  
**Ready to Develop**: After ✅ verification passes, you're ready for productive development!
