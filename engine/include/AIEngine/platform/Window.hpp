/**
 * Window.hpp - SDL2 Window with OpenGL Context
 * 
 * Platform abstraction for window creation, OpenGL context management,
 * and basic input handling using SDL2.
 * 
 * @version 1.0.0
 * @date 2026-03-04
 */

#pragma once

#include <string>
#include <cstdint>

// Forward declare SDL types to avoid including SDL headers
struct SDL_Window;
typedef void* SDL_GLContext;

namespace AIEngine {
    
    /**
     * @brief Window configuration structure
     */
    struct WindowConfig {
        uint32_t width = 800;
        uint32_t height = 600;
        std::string title = "AIEngine Application";
        bool fullscreen = false;
        bool resizable = true;
        bool vsync = true;
        int openglMajor = 3;
        int openglMinor = 3;
        bool openglCore = true;
    };
    
    /**
     * @brief SDL2 window with OpenGL context
     * 
     * Manages window creation, OpenGL context, and basic window operations.
     * Provides platform-independent interface for windowing functionality.
     */
    class Window {
    public:
        /**
         * @brief Create window with configuration
         * @param config Window configuration parameters
         */
        explicit Window(const WindowConfig& config);
        
        /**
         * @brief Destructor - cleanup SDL resources
         */
        ~Window();
        
        // Prevent copying due to SDL resource management
        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;
        Window(Window&&) = delete;
        Window& operator=(Window&&) = delete;
        
        /**
         * @brief Initialize SDL and create window with OpenGL context
         * @return True if initialization successful
         */
        bool Initialize();
        
        /**
         * @brief Check if window should close
         * @return True if close requested
         */
        bool ShouldClose() const;
        
        /**
         * @brief Request window to close
         */
        void RequestClose();
        
        /**
         * @brief Swap OpenGL buffers (present frame)
         */
        void SwapBuffers();
        
        /**
         * @brief Poll window events
         */
        void PollEvents();
        
        /**
         * @brief Get window width
         * @return Window width in pixels
         */
        uint32_t GetWidth() const { return m_config.width; }
        
        /**
         * @brief Get window height
         * @return Window height in pixels
         */
        uint32_t GetHeight() const { return m_config.height; }
        
        /**
         * @brief Get window aspect ratio
         * @return Width/height ratio
         */
        float GetAspectRatio() const { 
            return static_cast<float>(m_config.width) / static_cast<float>(m_config.height); 
        }
        
        /**
         * @brief Check if window is minimized
         * @return True if minimized
         */
        bool IsMinimized() const { return m_minimized; }
        
        /**
         * @brief Get SDL window handle (for advanced usage)
         * @return SDL_Window pointer
         */
        SDL_Window* GetSDLWindow() const { return m_window; }
        
        /**
         * @brief Get OpenGL context handle
         * @return SDL_GLContext handle
         */
        SDL_GLContext GetGLContext() const { return m_glContext; }
        
    private:
        bool CreateSDLWindow();
        bool CreateOpenGLContext();
        bool SetupOpenGLAttributes();
        void CleanupSDL();
        
        WindowConfig m_config;
        SDL_Window* m_window;
        SDL_GLContext m_glContext;
        
        bool m_initialized;
        bool m_shouldClose;
        bool m_minimized;
        
        // SDL initialization tracking
        static bool s_sdlInitialized;
        static uint32_t s_windowCount;
    };
    
}