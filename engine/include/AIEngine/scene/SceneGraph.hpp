/**
 * SceneGraph.hpp - Scene Graph Manager Declaration
 *
 * The SceneGraph class manages the hierarchy of SceneNodes and provides
 * efficient traversal, querying, and rendering of the scene.
 *
 * @version 1.0.0
 * @date 2026-03-04
 */

#pragma once

#include "SceneNode.hpp"
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace AIEngine {

// Forward declarations
class Renderer;

/**
 * @brief Scene Graph Manager
 *
 * Manages the hierarchy of scene nodes and provides efficient operations
 * for scene traversal, updating, rendering, and node queries.
 *
 * Features:
 * - Root node management
 * - Efficient scene traversal and updates
 * - Node registration and lookup by ID/name
 * - Batch operations on scene objects
 * - Memory-efficient node pooling
 *
 * @example Basic Scene Graph Usage
 * @code
 * // Create scene graph
 * auto sceneGraph = std::make_unique<SceneGraph>();
 *
 * // Create scene hierarchy
 * auto* root = sceneGraph->GetRootNode();
 * auto* player = sceneGraph->CreateNode("Player");
 * root->AddChild(std::unique_ptr<SceneNode>(player));
 *
 * // Add components to player
 * player->AddNode<TransformNode>();
 * player->AddNode<RenderNode>();
 *
 * // Create weapon as child
 * auto* weapon = sceneGraph->CreateNode("Weapon");
 * player->AddChild(std::unique_ptr<SceneNode>(weapon));
 *
 * // Update scene each frame
 * sceneGraph->Update(deltaTime);
 * sceneGraph->Render(renderer);
 * @endcode
 */
class SceneGraph {
public:
  /**
   * @brief Construct scene graph with root node
   */
  SceneGraph();

  /**
   * @brief Destructor - cleans up all nodes
   */
  ~SceneGraph();

  // Prevent copying due to complex ownership
  SceneGraph(const SceneGraph &) = delete;
  SceneGraph &operator=(const SceneGraph &) = delete;

  // Allow moving for container storage
  SceneGraph(SceneGraph &&) noexcept;
  SceneGraph &operator=(SceneGraph &&) noexcept;

  /**
   * @brief Create a new scene node
   * @param name Optional node name for identification
   * @return Raw pointer to created node (ownership transferred to caller)
   *
   * Creates a new SceneNode that can be added to the scene hierarchy.
   * The caller is responsible for adding the node to the scene.
   */
  std::unique_ptr<SceneNode> CreateNode(const std::string &name = "Node");

  /**
   * @brief Get root node of the scene
   * @return Pointer to root node (always valid)
   */
  SceneNode *GetRootNode() { return m_rootNode.get(); }

  /**
   * @brief Get root node (const version)
   * @return Const pointer to root node
   */
  const SceneNode *GetRootNode() const { return m_rootNode.get(); }

  /**
   * @brief Get scene graph statistics
   * @return String containing node and component counts
   */
  std::string GetDebugInfo() const;

private:
  std::unique_ptr<SceneNode> m_rootNode;
};

} // namespace AIEngine