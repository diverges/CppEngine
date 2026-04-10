# Implementation Plan: CMake Build System Migration

**Branch**: `007-cmake-migration` | **Date**: 2026-04-09 | **Spec**: [spec.md](spec.md)  
**Input**: Feature specification from `/specs/007-cmake-migration/spec.md`

## Summary

Migrate the AIEngine project from GNU Make to CMake. The existing Makefiles are complex and include targets unneeded for day-to-day development. The replacement must support exactly three operations from the repository root: a **debug build**, a **release build**, and a **run** command that launches the testgame output. CMake 3.20+ with `CMakePresets.json` delivers these as single-command developer workflows. All third-party dependencies are declared PUBLIC on the engine library so testgame needs only a single `AIEngine` link declaration.

## Technical Context

**Language/Version**: C++17 (constitution minimum; C++20 preferred)  
**Primary Dependencies**: CMake 3.20+, Ninja (generator), SDL2 (vendored), GLEW (vendored), GLM (vendored header-only), GLAD (vendored header-only), doctest (vendored header-only)  
**Storage**: N/A  
**Testing**: doctest — unit test migration out of scope for this feature  
**Target Platform**: Windows primary (MinGW-w64), macOS and Linux (future)  
**Project Type**: desktop-app (game engine static library + testgame executable)  
**Performance Goals**: Build time comparable to current Make; 60fps runtime target unchanged  
**Constraints**: All deps vendored under `engine/deps/`; no internet at build time; `bin/debug/` and `bin/release/` output layout preserved; testgame declares only `AIEngine` as a direct link dep  
**Scale/Scope**: 2 sub-projects (engine library, testgame executable); ~15 engine source files; 5 new CMake files total

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

| Principle | Status | Notes |
|-----------|--------|-------|
| I. Modern C++ First | ✅ PASS | CMake enforces `cxx_std_17` via `target_compile_features`; no source changes |
| II. Cross-Platform Compatibility | ✅ PASS | CMake is inherently cross-platform; `if(WIN32)` guards isolate Windows-specific DLL copy and linker flags |
| III. Component-Driven Architecture | ✅ PASS | Build system does not affect source architecture |
| IV. Performance-Conscious Development | ✅ PASS | Release preset uses `-O3 -DNDEBUG`; debug preset uses `-g -O0` |
| V. Test-Driven Learning | ⚠️ NOTED | Test infrastructure migration explicitly out of scope (see spec Assumptions); existing test targets are not touched |
| **Build System Standard** | ⚠️ JUSTIFIED DEVIATION | Constitution mandates GNU Make; this feature replaces it with CMake per explicit user request. CMake better satisfies the cross-platform mandate of Principle II. Constitution Build System Standards section to be updated post-merge. |

**Gate result**: PROCEED. The single deviation is the purpose of this feature and is explicitly justified.

## Project Structure

### Documentation (this feature)

```text
specs/007-cmake-migration/
├── plan.md              # This file
├── research.md          # Phase 0 output ✅
├── data-model.md        # Phase 1 output ✅
├── quickstart.md        # Phase 1 output ✅
├── contracts/           # N/A — internal build tooling
└── tasks.md             # Phase 2 output — run /speckit.tasks
```

### Source Code (repository root)

```text
CMakeLists.txt                       # NEW: Root orchestrator
CMakePresets.json                    # NEW: debug/release presets
engine/
├── CMakeLists.txt                   # NEW: Engine static library
└── cmake/
    └── VendoredDeps.cmake           # NEW: IMPORTED targets for bundled deps
testgame/
└── CMakeLists.txt                   # NEW: Testgame executable (links AIEngine only)
```

**Structure Decision**: Single-project CMake layout with two `add_subdirectory()` calls from the root. Mirrors the existing Make components pattern (root → engine → testgame). Old Makefiles remain until validated and removed as a separate cleanup step.

## Complexity Tracking

> **Fill ONLY if Constitution Check has violations that must be justified**

| Violation | Why Needed | Simpler Alternative Rejected Because |
|-----------|------------|-------------------------------------|
| Build tool switch (GNU Make → CMake) | This IS the feature; CMake provides cross-platform preset support that Make cannot | Keeping Make would not satisfy FR-001 through FR-009 |
| `VendoredDeps.cmake` helper module | SDL2 and GLEW are pre-built vendored binaries; IMPORTED targets are required for generator expressions used in DLL copying and PUBLIC link propagation | Inline absolute paths lose generator expression support and break cross-machine reproducibility |

---

## Phase 0: Research Findings

All NEEDS CLARIFICATION resolved. See [research.md](research.md) for full details.

| Question | Resolution |
|----------|-----------|
| Vendored dep handling | IMPORTED targets in `engine/cmake/VendoredDeps.cmake`; declared PUBLIC on AIEngine for transitive propagation (R-001) |
| Testgame link scope | testgame declares only `AIEngine`; all SDL2/GLEW/system deps propagate via AIEngine's PUBLIC link interface (R-001) |
| Output directory structure | `$<LOWER_CASE:$<CONFIG>>` generator expression → `bin/debug/` / `bin/release/` (R-002) |
| Simple debug/release commands | `CMakePresets.json` with Ninja generator; `cmake --preset debug` pattern (R-003) |
| Run target | `add_custom_target(run ...)` with `$<TARGET_FILE:testgame>` (R-004) |
| DLL copying on Windows | `POST_BUILD copy_if_different` with `$<TARGET_FILE_DIR:testgame>` (R-005) |
| MinGW-w64 specifics | Auto-detect; add `-static-libgcc/-static-libstdc++` on Windows/GCC (R-006) |

---

## Phase 1: Design Decisions

### CMake File Roles

| File | Role | Key Contents |
|------|------|-------------|
| `CMakeLists.txt` (root) | Orchestrator | `cmake_minimum_required`, `project`, output dirs, `add_subdirectory`, `run` target |
| `CMakePresets.json` | Developer interface | `debug` and `release` configure + build presets using Ninja |
| `engine/CMakeLists.txt` | Library definition | `add_library(AIEngine STATIC)`, sources, include paths, all deps declared PUBLIC |
| `engine/cmake/VendoredDeps.cmake` | Dep registry | IMPORTED targets for SDL2, GLEW, GLM, GLAD, doctest |
| `testgame/CMakeLists.txt` | Executable definition | `add_executable(testgame)`, `target_link_libraries(testgame PRIVATE AIEngine)`, DLL copy, MinGW linker flags |

### Key Design Decisions

**D-001: testgame links only `AIEngine`** — All third-party deps (SDL2, GLEW, GLM, GLAD, opengl32, gdi32) are declared PUBLIC on the AIEngine target and propagate transitively. This was explicitly confirmed in the spec clarification session (2026-04-09).

**D-002: No contracts directory** — Internal build tooling only; no public API, CLI schema, or network interface exposed.

**D-003: Ninja as default generator** — Faster than Unix Makefiles, cross-platform, auto-discovered by VS Code CMake Tools. Avoids the naming collision of also having a `Makefile` in the tree during the transition period.

**D-004: Dep arch path scoped to `windows-mingw64`** — The current deps layout only has `windows-mingw64/` archives. The `VendoredDeps.cmake` will include conditional blocks for future macOS/Linux expansion.

**D-005: Old Makefiles not deleted** — They remain until the CMake build is validated. Deletion is a follow-up cleanup task after QA.

**D-006: Constitution update deferred** — The "Build Tool: GNU Make" line in the constitution should be updated to CMake after this feature merges.

### Post-Design Constitution Re-Check

All Phase 1 decisions are consistent with the Phase 0 check. The justified deviation remains the only violation and is the purpose of this feature.

---

## Artifacts Produced

| Artifact | Path | Status |
|----------|------|--------|
| Feature spec | `specs/007-cmake-migration/spec.md` | ✅ Complete (clarification applied) |
| Research | `specs/007-cmake-migration/research.md` | ✅ Complete (updated for PUBLIC deps) |
| Data model | `specs/007-cmake-migration/data-model.md` | ✅ Complete (updated for transitive linking) |
| Quickstart | `specs/007-cmake-migration/quickstart.md` | ✅ Complete |
| Contracts | N/A — internal tooling | ✅ Skipped (justified) |
| Tasks | `specs/007-cmake-migration/tasks.md` | ⏳ Next — run `/speckit.tasks` |
