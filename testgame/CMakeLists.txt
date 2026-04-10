cmake_minimum_required(VERSION 3.20)

# ---------------------------------------------------------------------------
# Testgame executable
# Links only AIEngine — all third-party deps (SDL2, GLEW, GLM, GLAD, opengl32,
# gdi32) propagate transitively via AIEngine's PUBLIC link interface.
# Sources are auto-discovered from src/ — re-run cmake configure after
# adding or removing files.
# ---------------------------------------------------------------------------
file(GLOB_RECURSE TESTGAME_SOURCES CONFIGURE_DEPENDS
    "src/*.cpp"
)

add_executable(testgame ${TESTGAME_SOURCES})

# Standalone executable — does not export symbols, no import lib needed.
# Disabling ENABLE_EXPORTS removes the --out-implib linker flag which can
# cause write failures on Windows with some MinGW toolchain versions.
set_property(TARGET testgame PROPERTY ENABLE_EXPORTS FALSE)

target_link_libraries(testgame PRIVATE AIEngine)

# ---------------------------------------------------------------------------
# Windows: copy runtime DLLs next to executable after each build so the
# testgame can find them at launch without PATH manipulation.
# ---------------------------------------------------------------------------
if(WIN32)
    set(_DEP_ARCH "windows-mingw64")
    set(_SDL2_DLL  "${CMAKE_SOURCE_DIR}/engine/deps/SDL2/lib/${_DEP_ARCH}/SDL2.dll")
    set(_GLEW_DLL  "${CMAKE_SOURCE_DIR}/engine/deps/glew/lib/${_DEP_ARCH}/glew32.dll")

    foreach(_dll IN ITEMS "${_SDL2_DLL}" "${_GLEW_DLL}")
        add_custom_command(TARGET testgame POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                "${_dll}"
                "$<TARGET_FILE_DIR:testgame>"
            COMMENT "Copying ${_dll} → $<TARGET_FILE_DIR:testgame>"
        )
    endforeach()
endif()

# ---------------------------------------------------------------------------
# Windows + MinGW: note on static runtime linking
# -static-libgcc/-static-libstdc++ are NOT applied here as the UCRT64
# toolchain (msys2 ucrt64) handles CRT linking via ucrt.dll automatically.
# Runtime DLLs (SDL2.dll, glew32.dll) are handled by the POST_BUILD copy above.
# ---------------------------------------------------------------------------
