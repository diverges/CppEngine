# VendoredDeps.cmake
# Defines IMPORTED targets for all bundled dependencies under engine/deps/
# Include this file from engine/CMakeLists.txt via: include(${PROJECT_SOURCE_DIR}/cmake/VendoredDeps.cmake)

set(_DEPS_DIR "${CMAKE_CURRENT_LIST_DIR}/../engine/deps")

# ---------------------------------------------------------------------------
# Detect platform-specific dependency subdirectory
# ---------------------------------------------------------------------------
if(WIN32)
    set(_DEP_ARCH "windows-mingw64")
else()
    # macOS / Linux: extend this block when platform libs are added under deps/
    message(WARNING "VendoredDeps: no prebuilt binaries for this platform under engine/deps/. "
                    "System SDL2/GLEW will be required.")
    set(_DEP_ARCH "")
endif()

# ---------------------------------------------------------------------------
# SDL2 (pre-built static archive + shared DLL)
# Use INTERFACE + link_directories so the linker receives -lSDL2 style flags
# rather than full library paths.  Full absolute paths to .a files trigger an
# ld crash (exit 5) with UCRT64/GCC 15.2 on Windows when the LTO plugin is
# active.  -l flags go through the frontend which handles them correctly.
# ---------------------------------------------------------------------------
add_library(SDL2::SDL2 INTERFACE IMPORTED GLOBAL)
target_include_directories(SDL2::SDL2 INTERFACE "${_DEPS_DIR}/SDL2/include")
target_link_directories(SDL2::SDL2 INTERFACE "${_DEPS_DIR}/SDL2/lib/${_DEP_ARCH}")
target_link_libraries(SDL2::SDL2 INTERFACE -lSDL2)

add_library(SDL2::main INTERFACE IMPORTED GLOBAL)
target_link_directories(SDL2::main INTERFACE "${_DEPS_DIR}/SDL2/lib/${_DEP_ARCH}")
target_link_libraries(SDL2::main INTERFACE -lSDL2main)

# ---------------------------------------------------------------------------
# GLEW (pre-built static archive + shared DLL)
# ---------------------------------------------------------------------------
add_library(GLEW::GLEW INTERFACE IMPORTED GLOBAL)
target_include_directories(GLEW::GLEW INTERFACE "${_DEPS_DIR}/glew/include")
target_link_directories(GLEW::GLEW INTERFACE "${_DEPS_DIR}/glew/lib/${_DEP_ARCH}")
target_link_libraries(GLEW::GLEW INTERFACE -lglew32)

# ---------------------------------------------------------------------------
# GLM (header-only)
# GLM headers live at engine/deps/glm/glm.hpp → include dir is engine/deps/
# so consumers can use: #include <glm/glm.hpp>
# ---------------------------------------------------------------------------
add_library(GLM::GLM INTERFACE IMPORTED GLOBAL)
target_include_directories(GLM::GLM INTERFACE "${_DEPS_DIR}")

# ---------------------------------------------------------------------------
# GLAD (header-only — headers live under deps/glad/include/)
# ---------------------------------------------------------------------------
add_library(GLAD::GLAD INTERFACE IMPORTED GLOBAL)
target_include_directories(GLAD::GLAD INTERFACE "${_DEPS_DIR}/glad/include")

# ---------------------------------------------------------------------------
# doctest (header-only)
# ---------------------------------------------------------------------------
add_library(doctest::doctest INTERFACE IMPORTED GLOBAL)
target_include_directories(doctest::doctest INTERFACE "${_DEPS_DIR}/doctest")
