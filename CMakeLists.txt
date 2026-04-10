cmake_minimum_required(VERSION 3.20)
project(AIEngine VERSION 1.0.0 LANGUAGES CXX)

# ---------------------------------------------------------------------------
# MinGW/UCRT64 workaround: CMake 4.x applies two problematic patterns on
# Windows-GNU with non-Ninja generators:
#   1. --out-implib flag that causes ld to exit 5 (ERROR_ACCESS_DENIED) when
#      writing the import library for a standalone executable.
#   2. --whole-archive objects.a pattern (needed for old GCC < 4.4) that
#      conflicts with the LTO plugin init in GCC 15.2, also causing ld exit 5.
# Reset the executable link rule to a simple direct form: pass objects
# directly to the compiler frontend which handles response files natively.
# ---------------------------------------------------------------------------
if(WIN32 AND CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(CMAKE_CXX_LINK_EXECUTABLE
        "<CMAKE_CXX_COMPILER> <FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")
endif()

# ---------------------------------------------------------------------------
# Output directories — both debug and release binaries land in bin/<config>/
# Works with single-config (Ninja/Makefiles) and multi-config (VS/Xcode) generators
# ---------------------------------------------------------------------------
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin/$<LOWER_CASE:$<CONFIG>>")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin/$<LOWER_CASE:$<CONFIG>>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin/$<LOWER_CASE:$<CONFIG>>")

# ---------------------------------------------------------------------------
# Sub-projects
# ---------------------------------------------------------------------------
add_subdirectory(engine)
add_subdirectory(testgame)

# ---------------------------------------------------------------------------
# Run target — launches testgame from its output directory so bundled DLLs
# (SDL2.dll, glew32.dll) copied alongside it are found at runtime.
# Usage: cmake --build --preset debug --target run
# ---------------------------------------------------------------------------
add_custom_target(run
    COMMAND "$<TARGET_FILE:testgame>"
    DEPENDS testgame
    WORKING_DIRECTORY "$<TARGET_FILE_DIR:testgame>"
    COMMENT "Launching testgame..."
    USES_TERMINAL
)
