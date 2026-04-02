/**
 * AIEngine - C++ Game Engine
 * Main convenience header for including all engine functionality
 *
 * This header provides a single include point for all AIEngine features.
 * Include this in your application to access the complete engine API.
 *
 * @version 1.0.0
 * @date 2026-03-04
 */

#pragma once

// Version information
#define AIENGINE_VERSION_MAJOR 1
#define AIENGINE_VERSION_MINOR 0
#define AIENGINE_VERSION_PATCH 0

// Core engine systems
#include "core/Component.hpp"
#include "core/Engine.hpp"

// Scene graph and hierarchy
#include "scene/SceneGraph.hpp"
#include "scene/SceneNode.hpp"

// Component system
#include "components/RenderComponent.hpp"
#include "components/TransformComponent.hpp"

// Graphics and rendering
#include "graphics/GeometryFactory.hpp"
#include "graphics/Mesh.hpp"
#include "graphics/Renderer.hpp"
#include "graphics/Shader.hpp"

// Mathematics utilities
#include "math/Transform.hpp"

// Platform abstraction (when implemented)
// #include "platform/Window.hpp"
// #include "platform/Input.hpp"

/**
 * @namespace AIEngine
 * @brief Main namespace containing all engine functionality
 *
 * The AIEngine namespace encapsulates all game engine components including:
 * - Core engine lifecycle management
 * - Scene graph and hierarchical transforms
 * - Component-driven entity system
 * - Graphics rendering pipeline
 * - Mathematics and utility functions
 *
 * @example Basic Usage
 * @code
 * #include <AIEngine/AIEngine.hpp>
 *
 * int main() {
 *     AIEngine::EngineConfig config;
 *     config.windowTitle = "My Game";
 *
 *     AIEngine::Engine engine(config);
 *     if (!engine.Initialize()) {
 *         return -1;
 *     }
 *
 *     // Create scene with objects
 *     auto* sceneGraph = engine.GetSceneGraph();
 *     auto* node = sceneGraph->CreateNode();
 *
 *     // Add components
 *     auto* transform = node->AddComponent<AIEngine::TransformComponent>();
 *     auto* render = node->AddComponent<AIEngine::RenderComponent>();
 *
 *     // Game loop
 *     while (!engine.ShouldClose()) {
 *         engine.Update(engine.GetDeltaTime());
 *         engine.Render();
 *     }
 *
 *     engine.Shutdown();
 *     return 0;
 * }
 * @endcode
 */
namespace AIEngine {

/**
 * @brief Get engine version information
 * @return Version string in format "Major.Minor.Patch"
 */
inline const char *GetVersion() { return "1.0.0"; }

/**
 * @brief Check if engine version is compatible
 * @param major Required major version
 * @param minor Required minor version
 * @return true if current version is compatible
 */
inline bool IsVersionCompatible(int major, int minor) {
  return (AIENGINE_VERSION_MAJOR == major && AIENGINE_VERSION_MINOR >= minor);
}

/**
 * @brief Initialize engine global systems
 *
 * Called automatically by Engine::Initialize(), but can be called
 * manually if using engine components independently.
 *
 * @return true if initialization successful
 */
bool InitializeGlobalSystems();

/**
 * @brief Shutdown engine global systems
 *
 * Called automatically by Engine::Shutdown(), cleans up global
 * resources and systems.
 */
void ShutdownGlobalSystems();
} // namespace AIEngine

// Educational note: This convenience header allows applications to include
// the entire engine API with a single #include statement, simplifying usage
// while maintaining modular design of individual components.