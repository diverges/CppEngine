@echo off
REM AIEngine Installation Script for Windows
REM Requires MSYS2/MinGW-w64 development environment

setlocal enabledelayedexpansion

REM Configuration
set "ENGINE_VERSION=1.0.0"
set "DEFAULT_PREFIX=C:\AIEngine"
set "INSTALL_PREFIX=%1"
if "%INSTALL_PREFIX%"=="" set "INSTALL_PREFIX=%DEFAULT_PREFIX%"

REM Get script directory and engine root
set "SCRIPT_DIR=%~dp0"
set "ENGINE_ROOT=%SCRIPT_DIR%\.."

REM Colors (if supported)
set "ESC="

echo.
echo === AIEngine Windows Installation ===
echo Version: %ENGINE_VERSION%
echo Install prefix: %INSTALL_PREFIX%
echo Engine root: %ENGINE_ROOT%
echo.

REM Check if MSYS2 is available
where /q pacman
if %errorlevel% neq 0 (
    echo ERROR: MSYS2/pacman not found in PATH
    echo Please install MSYS2 from https://www.msys2.org/
    goto :error
)
echo [OK] MSYS2 environment detected

REM Check compiler
where /q g++
if %errorlevel% neq 0 (
    echo ERROR: g++ compiler not found
    echo Install via: pacman -S mingw-w64-x86_64-gcc
    goto :error
)
echo [OK] C++ compiler found

REM Check make
where /q make
if %errorlevel% neq 0 (
    echo ERROR: make not found
    echo Install via: pacman -S mingw-w64-x86_64-make
    goto :error
)
echo [OK] Make build system found

REM Check SDL2 headers
if not exist "C:\msys64\mingw64\include\SDL2\SDL.h" (
    echo WARNING: SDL2 development headers not found
    echo Install via: pacman -S mingw-w64-x86_64-sdl2
    echo Continuing anyway...
) else (
    echo [OK] SDL2 development headers found
)

REM Check GLEW headers
if not exist "C:\msys64\mingw64\include\GL\glew.h" (
    echo WARNING: GLEW development headers not found
    echo Install via: pacman -S mingw-w64-x86_64-glew
    echo Continuing anyway...
) else (
    echo [OK] GLEW development headers found
)

echo.
echo Building AIEngine library...
echo.

REM Build the engine
pushd "%ENGINE_ROOT%\engine"
call make clean
if %errorlevel% neq 0 (
    echo ERROR: Failed to clean engine build
    popd
    goto :error
)

call make release
if %errorlevel% neq 0 (
    echo ERROR: Failed to build engine library
    popd
    goto :error
)

if not exist "lib\libAIEngine.a" (
    echo ERROR: Engine library not found after build
    popd
    goto :error
)
popd

echo [OK] Engine library built successfully
echo.

REM Create installation directories
echo Creating installation directories...
if not exist "%INSTALL_PREFIX%" mkdir "%INSTALL_PREFIX%"
if not exist "%INSTALL_PREFIX%\include" mkdir "%INSTALL_PREFIX%\include"
if not exist "%INSTALL_PREFIX%\lib" mkdir "%INSTALL_PREFIX%\lib"
if not exist "%INSTALL_PREFIX%\bin" mkdir "%INSTALL_PREFIX%\bin"
if not exist "%INSTALL_PREFIX%\docs" mkdir "%INSTALL_PREFIX%\docs"

REM Copy headers
echo Installing headers...
xcopy "%ENGINE_ROOT%\engine\include\AIEngine" "%INSTALL_PREFIX%\include\AIEngine\" /E /I /Y /Q >nul
if %errorlevel% neq 0 (
    echo ERROR: Failed to copy headers
    goto :error
)
echo [OK] Headers installed to %INSTALL_PREFIX%\include\AIEngine

REM Copy library
echo Installing library...
copy "%ENGINE_ROOT%\engine\lib\libAIEngine.a" "%INSTALL_PREFIX%\lib\" >nul
if %errorlevel% neq 0 (
    echo ERROR: Failed to copy library
    goto :error
)
echo [OK] Library installed to %INSTALL_PREFIX%\lib\libAIEngine.a

REM Copy shaders
echo Installing shaders...
if exist "%ENGINE_ROOT%\engine\shaders" (
    xcopy "%ENGINE_ROOT%\engine\shaders\*" "%INSTALL_PREFIX%\shaders\" /E /I /Y /Q >nul
    echo [OK] Shaders installed to %INSTALL_PREFIX%\shaders
)

REM Copy documentation
echo Installing documentation...
if exist "%ENGINE_ROOT%\docs" (
    xcopy "%ENGINE_ROOT%\docs\*" "%INSTALL_PREFIX%\docs\" /E /I /Y /Q >nul
    echo [OK] Documentation installed to %INSTALL_PREFIX%\docs
)

copy "%ENGINE_ROOT%\README.md" "%INSTALL_PREFIX%\" >nul 2>&1

REM Copy required DLLs from MSYS2
echo Installing runtime DLLs...
set "MSYS2_BIN=C:\msys64\mingw64\bin"
if exist "%MSYS2_BIN%" (
    for %%f in (SDL2.dll glew32.dll) do (
        if exist "%MSYS2_BIN%\%%f" (
            copy "%MSYS2_BIN%\%%f" "%INSTALL_PREFIX%\bin\" >nul
            echo [OK] Copied %%f
        )
    )
)

REM Create pkg-config file
echo Creating pkg-config file...
set "PKGCONFIG_DIR=%INSTALL_PREFIX%\lib\pkgconfig"
if not exist "%PKGCONFIG_DIR%" mkdir "%PKGCONFIG_DIR%"

(
echo prefix=%INSTALL_PREFIX%
echo exec_prefix=${prefix}
echo libdir=${exec_prefix}/lib  
echo includedir=${prefix}/include
echo.
echo Name: AIEngine
echo Description: Modern C++ Game Engine
echo Version: %ENGINE_VERSION%
echo Libs: -L${libdir} -lAIEngine -lSDL2main -lSDL2 -lglew32 -lopengl32
echo Cflags: -I${includedir} -std=c++17
) > "%PKGCONFIG_DIR%\aiengine.pc"
echo [OK] pkg-config file created

REM Create usage example
echo Creating usage example...
(
echo @echo off
echo REM Example compile command for AIEngine
echo REM Make sure %INSTALL_PREFIX%\bin is in your PATH for DLLs
echo.
echo g++ -std=c++17 ^
echo     -I"%INSTALL_PREFIX%\include" ^
echo     your_game.cpp ^
echo     -L"%INSTALL_PREFIX%\lib" -lAIEngine ^
echo     -lSDL2main -lSDL2 -lglew32 -lopengl32 ^
echo     -o your_game.exe
) > "%INSTALL_PREFIX%\compile_example.bat"
echo [OK] Example compile script created

REM Test installation
echo.
echo Testing installation...
set "TEST_FILE=%TEMP%\test_aiengine.cpp"
(
echo #include ^<AIEngine/AIEngine.hpp^>
echo #include ^<iostream^>
echo.
echo int main^(^) {
echo     std::cout ^<^< "AIEngine headers accessible!" ^<^< std::endl;
echo     return 0;
echo }
) > "%TEST_FILE%"

g++ -std=c++17 -I"%INSTALL_PREFIX%\include" "%TEST_FILE%" -o "%TEMP%\test_aiengine.exe" 2>nul
if %errorlevel% equ 0 (
    echo [OK] Test compilation successful!
    del "%TEMP%\test_aiengine.exe" 2>nul
) else (
    echo [WARNING] Test compilation failed - check installation
)
del "%TEST_FILE%" 2>nul

echo.
echo === Installation Complete! ===
echo.
echo Installation directory: %INSTALL_PREFIX%
echo Headers: %INSTALL_PREFIX%\include\AIEngine
echo Library: %INSTALL_PREFIX%\lib\libAIEngine.a 
echo Runtime DLLs: %INSTALL_PREFIX%\bin
echo Documentation: %INSTALL_PREFIX%\docs
echo.
echo Usage in your projects:
echo   1. Add %INSTALL_PREFIX%\bin to your PATH
echo   2. Use compile example: %INSTALL_PREFIX%\compile_example.bat
echo   3. Include headers: #include ^<AIEngine/AIEngine.hpp^>
echo.
echo Example project structure:
echo   my_game.cpp
echo   compile.bat  (copy from %INSTALL_PREFIX%\compile_example.bat)
echo.
pause
goto :end

:error
echo.
echo Installation failed!
echo Check the error messages above and try again.
pause
exit /b 1

:end
endlocal