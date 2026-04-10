cmake_minimum_required(VERSION 3.20)

# ---------------------------------------------------------------------------
# Load vendored dependency IMPORTED targets
# Must be included before any target_link_libraries calls
# ---------------------------------------------------------------------------
include("${CMAKE_CURRENT_SOURCE_DIR}/VendoredDeps.cmake")

# ---------------------------------------------------------------------------
# AIEngine static library
# Sources are auto-discovered from src/ — re-run cmake configure after
# adding or removing files (cmake --preset <name> is sufficient).
# ---------------------------------------------------------------------------
file(GLOB_RECURSE AIENGINE_SOURCES CONFIGURE_DEPENDS
    "src/*.cpp"
)

add_library(AIEngine STATIC ${AIENGINE_SOURCES})

# C++17 declared PUBLIC so consumers (testgame) inherit it automatically
target_compile_features(AIEngine PUBLIC cxx_std_17)

# Public include path — consumers use <AIEngine/...> includes
target_include_directories(AIEngine PUBLIC include)

# Internal include path for src/ files that include deps directly
target_include_directories(AIEngine PRIVATE src)

# ---------------------------------------------------------------------------
# Link all vendored deps as PUBLIC so they propagate transitively to consumers.
# testgame only needs to link AIEngine — no direct dep declarations required there.
# ---------------------------------------------------------------------------
target_link_libraries(AIEngine
    PUBLIC
        SDL2::SDL2
        SDL2::main
        GLEW::GLEW
        GLM::GLM
        GLAD::GLAD
)

# Windows system libraries required by SDL2 / OpenGL
if(WIN32)
    target_link_libraries(AIEngine PUBLIC opengl32 gdi32)
endif()
