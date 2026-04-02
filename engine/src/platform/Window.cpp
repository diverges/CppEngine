/**
 * Window.cpp - SDL2 Window with OpenGL Context Implementation
 *
 * Platform abstraction implementation for window creation, OpenGL context
 * management, and basic input handling using SDL2.
 *
 * @version 1.0.0
 * @date 2026-03-04
 */

#include "../../include/AIEngine/platform/Window.hpp"
#include <iostream>

// Include real SDL2 headers
#include <SDL2/SDL.h>

namespace AIEngine {

// Static members
bool Window::s_sdlInitialized = false;
uint32_t Window::s_windowCount = 0;

Window::Window(const WindowConfig &config)
    : m_config(config), m_window(nullptr), m_glContext(nullptr),
      m_initialized(false), m_shouldClose(false), m_minimized(false) {
  std::cout << "Window: Creating window '" << config.title << "' ("
            << config.width << "x" << config.height << ")\n";
}

Window::~Window() {
  CleanupSDL();
  std::cout << "Window: Destructor completed\n";
}

bool Window::Initialize() {
  std::cout << "Window: Initializing SDL2 and OpenGL context...\n";

  // Initialize SDL if not already done
  if (!s_sdlInitialized) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
      std::cerr << "Window: Failed to initialize SDL video subsystem\n";
      return false;
    }
    s_sdlInitialized = true;
    std::cout << "Window: SDL initialized successfully\n";
  }

  // Set OpenGL attributes before window creation
  if (!SetupOpenGLAttributes()) {
    std::cerr << "Window: Failed to setup OpenGL attributes\n";
    return false;
  }

  // Create SDL window
  if (!CreateSDLWindow()) {
    std::cerr << "Window: Failed to create SDL window\n";
    return false;
  }

  // Create OpenGL context
  if (!CreateOpenGLContext()) {
    std::cerr << "Window: Failed to create OpenGL context\n";
    return false;
  }

  // Setup VSync
  if (m_config.vsync) {
    SDL_GL_SetSwapInterval(1);
    std::cout << "Window: VSync enabled\n";
  } else {
    SDL_GL_SetSwapInterval(0);
  }

  s_windowCount++;
  m_initialized = true;

  std::cout << "Window: Initialized successfully - " << m_config.width << "x"
            << m_config.height << " OpenGL " << m_config.openglMajor << "."
            << m_config.openglMinor << "\n";
  return true;
}

bool Window::ShouldClose() const { return m_shouldClose; }

void Window::RequestClose() {
  m_shouldClose = true;
  std::cout << "Window: Close requested\n";
}

void Window::SwapBuffers() {
  if (m_window && !m_minimized) {
    SDL_GL_SwapWindow(m_window);
  }
}

void Window::PollEvents() {
  if (!m_initialized) {
    return;
  }

  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_QUIT:
      RequestClose();
      break;
    // TODO: Handle other events (resize, input, etc.)
    default:
      break;
    }
  }
}

bool Window::CreateSDLWindow() {
  uint32_t flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

  if (m_config.resizable) {
    flags |= SDL_WINDOW_RESIZABLE;
  }

  // TODO: Handle fullscreen mode

  m_window =
      SDL_CreateWindow(m_config.title.c_str(), SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, static_cast<int>(m_config.width),
                       static_cast<int>(m_config.height), flags);

  if (!m_window) {
    std::cerr << "Window: Failed to create SDL window\n";
    return false;
  }

  std::cout << "Window: SDL window created successfully\n";
  return true;
}

bool Window::CreateOpenGLContext() {
  m_glContext = SDL_GL_CreateContext(m_window);
  if (!m_glContext) {
    std::cerr << "Window: Failed to create OpenGL context\n";
    return false;
  }

  std::cout << "Window: OpenGL context created successfully\n";
  return true;
}

bool Window::SetupOpenGLAttributes() {
  // Set OpenGL version
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, m_config.openglMajor);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, m_config.openglMinor);

  // Set core profile if requested
  if (m_config.openglCore) {
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);
  }

  // Double buffering and depth buffer
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

  std::cout << "Window: OpenGL attributes configured (OpenGL "
            << m_config.openglMajor << "." << m_config.openglMinor
            << (m_config.openglCore ? " Core" : " Compatibility") << ")\n";
  return true;
}

void Window::CleanupSDL() {
  if (m_glContext) {
    SDL_GL_DeleteContext(m_glContext);
    m_glContext = nullptr;
    std::cout << "Window: OpenGL context destroyed\n";
  }

  if (m_window) {
    SDL_DestroyWindow(m_window);
    m_window = nullptr;
    std::cout << "Window: SDL window destroyed\n";
  }

  if (m_initialized) {
    s_windowCount--;

    // Clean up SDL if this is the last window
    if (s_windowCount == 0 && s_sdlInitialized) {
      SDL_Quit();
      s_sdlInitialized = false;
      std::cout << "Window: SDL quit - last window closed\n";
    }
  }

  m_initialized = false;
}

} // namespace AIEngine