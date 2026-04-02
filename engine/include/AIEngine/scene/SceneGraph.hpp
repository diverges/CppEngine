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
 * player->AddComponent<TransformComponent>();
 * player->AddComponent<RenderComponent>();
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
   * @brief Update entire scene graph
   * @param deltaTime Time elapsed since last frame
   *
   * Recursively updates all nodes and their components starting
   * from the root node.
   */
  void Update(double deltaTime);

  /**
   * @brief Render entire scene graph
   * @param renderer Active renderer for drawing operations
   *
   * Recursively renders all nodes and their components starting
   * from the root node.
   */
  void Render(Renderer *renderer);

  /**
   * @brief Find node by unique ID
   * @param id Node ID to search for
   * @return Pointer to node if found, nullptr otherwise
   */
  SceneNode *FindNodeById(uint32_t id);

  /**
   * @brief Find node by name (first match)
   * @param name Node name to search for
   * @return Pointer to first node with matching name, nullptr if not found
   */
  SceneNode *FindNodeByName(const std::string &name);

  /**
   * @brief Find all nodes with given name
   * @param name Node name to search for
   * @return Vector of pointers to all matching nodes
   */
  std::vector<SceneNode *> FindAllNodesByName(const std::string &name);

  /**
   * @brief Find all nodes with specified component type
   * @tparam T Component type to search for
   * @return Vector of pointers to nodes containing component T
   */
  template <typename T> std::vector<SceneNode *> FindNodesWithComponent();

  /**
   * @brief Apply function to all nodes in the scene
   * @param func Function to apply to each node
   * @param activeOnly If true, only process active nodes
   *
   * Traverses the entire scene graph and applies the given function
   * to each node. Useful for batch operations.
   */
  void ForEachNode(std::function<void(SceneNode *)> func,
                   bool activeOnly = false);

  /**
   * @brief Apply function to all nodes in the scene (const version)
   * @param func Function to apply to each node
   * @param activeOnly If true, only process active nodes
   */
  void ForEachNode(std::function<void(const SceneNode *)> func,
                   bool activeOnly = false) const;

  /**
   * @brief Apply function to all nodes with specific component
   * @tparam T Component type filter
   * @param func Function to apply to each matching node
   * @param activeOnly If true, only process active nodes
   */
  template <typename T>
  void ForEachNodeWithComponent(std::function<void(SceneNode *, T *)> func,
                                bool activeOnly = false);

  /**
   * @brief Get total number of nodes in scene
   * @return Node count including root
   */
  size_t GetNodeCount() const;

  /**
   * @brief Get total number of active nodes
   * @return Active node count
   */
  size_t GetActiveNodeCount() const;

  /**
   * @brief Clear entire scene (except root node)
   *
   * Removes all child nodes from root, effectively clearing the scene.
   * The root node itself remains but becomes empty.
   */
  void Clear();

  /**
   * @brief Validate scene graph integrity
   * @return true if scene graph is valid
   *
   * Performs consistency checks on the scene graph structure
   * to ensure parent-child relationships are correct.
   */
  bool ValidateIntegrity() const;

  /**
   * @brief Get scene graph statistics
   * @return String containing node and component counts
   */
  std::string GetDebugInfo() const;

  /**
   * @brief Optimize scene graph for rendering
   *
   * Performs optimizations like spatial sorting or culling
   * preparation to improve rendering performance.
   */
  void Optimize();

private:
  std::unique_ptr<SceneNode> m_rootNode;

  // Node tracking for efficient lookups
  std::unordered_map<uint32_t, SceneNode *> m_nodeRegistry;
  std::unordered_map<std::string, std::vector<SceneNode *>> m_nameRegistry;

  // Statistics
  mutable size_t m_nodeCount = 0;
  mutable bool m_statisticsDirty = true;

  /**
   * @brief Register node for fast lookup
   * @param node Node to register
   */
  void RegisterNode(SceneNode *node);

  /**
   * @brief Unregister node from lookup tables
   * @param node Node to unregister
   */
  void UnregisterNode(SceneNode *node);

  /**
   * @brief Recursive node traversal helper
   * @param node Current node to process
   * @param func Function to apply
   * @param activeOnly Only process active nodes
   */
  void TraverseNodes(SceneNode *node, std::function<void(SceneNode *)> func,
                     bool activeOnly);

  /**
   * @brief Recursive node traversal helper (const version)
   * @param node Current node to process
   * @param func Function to apply
   * @param activeOnly Only process active nodes
   */
  void TraverseNodes(const SceneNode *node,
                     std::function<void(const SceneNode *)> func,
                     bool activeOnly) const;

  /**
   * @brief Update internal statistics
   */
  void UpdateStatistics() const;
};

} // namespace AIEngine

// Template implementations
#include "SceneGraph.inl"