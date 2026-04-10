# Tasks: CMake Build System Migration

**Input**: Design documents from `specs/007-cmake-migration/`  
**Branch**: `007-cmake-migration`  
**Date**: 2026-04-09  
**Prerequisites**: plan.md ✅, spec.md ✅, research.md ✅, data-model.md ✅, quickstart.md ✅

---

## Phase 1: Setup

**Purpose**: Create the vendored dependency helper module that all subsequent CMake files depend on.

- [x] T001 Create `engine/cmake/VendoredDeps.cmake` with IMPORTED targets for SDL2::SDL2, SDL2::main, GLEW::GLEW, GLM::GLM (INTERFACE), GLAD::GLAD (INTERFACE), and doctest::doctest (INTERFACE) using paths under `engine/deps/windows-mingw64/`

---

## Phase 2: Foundational (Blocking Prerequisites)

**Purpose**: Root CMakeLists.txt that wires together both sub-projects and sets the shared output directory layout. Must be complete before any user story can be configured.

**⚠️ CRITICAL**: US1, US2, and US3 cannot be configured without this phase.

- [x] T002 Create root `CMakeLists.txt` with `cmake_minimum_required(VERSION 3.20)`, `project(AIEngine)`, `CMAKE_RUNTIME_OUTPUT_DIRECTORY`/`CMAKE_ARCHIVE_OUTPUT_DIRECTORY` set to `${PROJECT_SOURCE_DIR}/bin/$<LOWER_CASE:$<CONFIG>>`, and `add_subdirectory(engine)` / `add_subdirectory(testgame)`

**Checkpoint**: Root CMakeLists.txt exists — user story implementation can now begin

---

## Phase 3: User Story 1 — Debug Build (Priority: P1) 🎯 MVP

**Goal**: `cmake --preset debug && cmake --build --preset debug` produces `bin/debug/libAIEngine.a` and `bin/debug/testgame.exe` with debug symbols and no optimization.

**Independent Test**: Running the debug build command from a clean checkout produces both binaries in `bin/debug/` without errors. Verifying with `objdump -g bin/debug/testgame.exe` confirms debug symbols are present.

- [x] T003 [P] [US1] Create `CMakePresets.json` at repository root with a `debug` configure preset (generator: Ninja, binaryDir: `${sourceDir}/build/debug`, CMAKE_BUILD_TYPE: Debug) and a `debug` build preset referencing it
- [x] T004 [P] [US1] Create `engine/CMakeLists.txt` with `add_library(AIEngine STATIC)` listing all 15 sources under `engine/src/`, `target_include_directories(AIEngine PUBLIC engine/include)`, `target_compile_features(AIEngine PUBLIC cxx_std_17)`, `include(cmake/VendoredDeps.cmake)`, and `target_link_libraries(AIEngine PUBLIC SDL2::SDL2 SDL2::main GLEW::GLEW GLM::GLM GLAD::GLAD)` plus `opengl32 gdi32` on WIN32
- [x] T005 [P] [US1] Create `testgame/CMakeLists.txt` with `add_executable(testgame testgame/src/main.cpp testgame/src/TestGame.cpp)`, `target_link_libraries(testgame PRIVATE AIEngine)`, POST_BUILD `copy_if_different` for `SDL2.dll` and `glew32.dll` to `$<TARGET_FILE_DIR:testgame>` (WIN32 only), and `-static-libgcc -static-libstdc++` linker options (WIN32 + GNU only)
- [x] T006 [US1] Validate debug build: run `cmake --preset debug && cmake --build --preset debug` and confirm `bin/debug/libAIEngine.a` and `bin/debug/testgame.exe` are produced with `SDL2.dll` and `glew32.dll` copied alongside

**Checkpoint**: US1 complete — debug build is fully functional and independently testable

---

## Phase 4: User Story 2 — Release Build (Priority: P2)

**Goal**: `cmake --preset release && cmake --build --preset release` produces `bin/release/libAIEngine.a` and `bin/release/testgame.exe` with full optimization and no debug symbols, isolated from the debug build.

**Independent Test**: Both `bin/debug/` and `bin/release/` exist simultaneously with no overlap. Running the release executable demonstrates visibly faster startup or confirms compiler flags via `objdump`.

- [x] T007 [US2] Add `release` configure preset (generator: Ninja, binaryDir: `${sourceDir}/build/release`, CMAKE_BUILD_TYPE: Release) and `release` build preset to `CMakePresets.json`
- [x] T008 [US2] Validate release build: run `cmake --preset release && cmake --build --preset release` and confirm `bin/release/libAIEngine.a` and `bin/release/testgame.exe` are produced alongside their DLLs, and that `bin/debug/` is unaffected

**Checkpoint**: US2 complete — debug and release builds coexist and are independently testable

---

## Phase 5: User Story 3 — Run Testgame (Priority: P3)

**Goal**: `cmake --build --preset debug --target run` builds (if needed) and launches the testgame from its output directory without the developer specifying a path.

**Independent Test**: After a debug build, invoking the run target launches the testgame window. SDL2.dll and glew32.dll are found automatically because the working directory is `$<TARGET_FILE_DIR:testgame>`.

- [x] T009 [US3] Add `add_custom_target(run COMMAND "$<TARGET_FILE:testgame>" DEPENDS testgame WORKING_DIRECTORY "$<TARGET_FILE_DIR:testgame>" COMMENT "Launching testgame..." USES_TERMINAL)` to root `CMakeLists.txt` after the `add_subdirectory(testgame)` call
- [x] T010 [US3] Validate run target: run `cmake --build --preset debug --target run` and confirm the testgame window launches correctly from `bin/debug/`

**Checkpoint**: All three user stories complete — full CMake workflow is operational

---

## Phase 6: Polish & Cross-Cutting Concerns

**Purpose**: Integrate the new build system into the developer environment and retire old tooling.

- [x] T011 [P] Update `.vscode/tasks.json` to replace all `make`-based task commands with CMake equivalents: debug build → `cmake --preset debug && cmake --build --preset debug`; release build → `cmake --preset release && cmake --build --preset release`; run → `cmake --build --preset debug --target run`
- [x] T012 [P] Update `.specify/memory/constitution.md` Build System Standards section: replace "GNU Make is the mandated build system" with CMake 3.20+, and document the rationale (cross-platform preset support, simpler developer workflow)
- [x] T013 Archive old Makefiles by removing `Makefile`, `engine/Makefile`, and `testgame/Makefile` from the repository root after confirming the CMake build passes end-to-end validation on the target platform

---

## Dependencies & Execution Order

### Phase Dependencies

- **Setup (Phase 1)**: No dependencies — can start immediately
- **Foundational (Phase 2)**: Depends on Phase 1 (T001) — BLOCKS all user stories
- **US1 (Phase 3)**: Depends on Phase 2 (T002) — T003, T004, T005 can run in parallel; T006 requires all three
- **US2 (Phase 4)**: Depends on Phase 3 completion (T006 validated) — modifies existing files
- **US3 (Phase 5)**: Depends on Phase 3 completion — modifies existing root CMakeLists.txt
- **Polish (Phase 6)**: Depends on all user stories validated; T011 and T012 can run in parallel; T013 requires T011 and T012

### User Story Dependencies

- **US1 (P1)**: Can start after Foundational — no dependency on US2 or US3
- **US2 (P2)**: Can start after US1 is validated — extends CMakePresets.json
- **US3 (P3)**: Can start after US1 is validated — extends root CMakeLists.txt; US2 and US3 can proceed in parallel

---

## Parallel Execution Example: US1

```bash
# After T002 (root CMakeLists.txt) is complete, run in parallel:
# Terminal 1                        Terminal 2                        Terminal 3
# T003: Create CMakePresets.json    T004: Create engine/CMakeLists    T005: Create testgame/CMakeLists

# After all three complete:
# T006: Validate
cmake --preset debug
cmake --build --preset debug
ls bin/debug/   # libAIEngine.a  testgame.exe  SDL2.dll  glew32.dll
```

---

## Implementation Strategy

**MVP scope**: Phase 1 + Phase 2 + Phase 3 (US1) — delivers a working debug build, which is the most critical daily operation. US2 and US3 are fast follow-ons that reuse all existing infrastructure.

**Suggested execution order for a single developer**:

1. T001 → T002 (sequential, each ~5 min)
2. T003, T004, T005 in parallel (or sequential, ~10–15 min total)
3. T006 validate — if it passes, US1 MVP is done
4. T007 + T009 can be done together (both small edits to existing files, ~5 min total)
5. T008 + T010 validate
6. T011, T012 in parallel, then T013

**Total estimated file count**: 5 new files created, 2 existing files modified (CMakePresets.json extended twice, root CMakeLists.txt extended once), 3 files deleted at end.

---

## Format Validation

All tasks follow checklist format: `- [ ] [TaskID] [P?] [Story?] Description with file path`

| Check | Result |
|-------|--------|
| All tasks have checkbox `- [ ]` | ✅ |
| All tasks have sequential Task ID (T001–T013) | ✅ |
| `[P]` present only on parallelizable tasks (different files, no blocking deps) | ✅ |
| `[US1]`/`[US2]`/`[US3]` present on all user story phase tasks | ✅ |
| Setup and Foundational tasks have no story label | ✅ |
| Polish tasks have no story label | ✅ |
| All tasks include explicit file paths | ✅ |
| No test tasks (not requested in spec) | ✅ |
