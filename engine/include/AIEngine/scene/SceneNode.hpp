/**
 * SceneNode.hpp - Scene Graph Node Declaration
 *
 * SceneNode is a node in the scene graph tree. Each node holds one optional
 * INode payload (its behaviour/data), one parent, and any number of children.
 *
 * @version 1.0.0
 * @date 2026-03-04
 */

#pragma once

#include <AIEngine/core/Node.hpp>
#include <algorithm>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace AIEngine {

/**
 * @brief Scene Graph Node
 *
 * A node in the scene hierarchy. Each SceneNode carries a single INode
 * payload that provides its behaviour (transform, render, physics, etc.).
 * Nodes form a tree via parent-child relationships.
 *
 * @example Basic Usage
 * @code
 * // A node carrying a transform
 * auto transformNode = std::make_unique<SceneNode>("Player");
 * auto* transform = transformNode->SetNode<TransformNode>();
 * transform->SetPosition(10.0f, 5.0f, 2.0f);
 *
 * // A child node carrying a render payload
 * auto renderNode = std::make_unique<SceneNode>("Player/Render");
 * auto* render = renderNode->SetNode<RenderNode>();
 * render->SetMeshId(playerMeshId);
 *
 * transformNode->AddChild(std::move(renderNode));
 * @endcode
 */
class SceneNode {
public:
  /**
   * @brief Construct node with optional name
   * @param name Human-readable node identifier
   */
  explicit SceneNode(const std::string &name = "Node");

  /**
   * @brief Destructor - ensures proper cleanup
   */
  virtual ~SceneNode();

  // Prevent copying to avoid ownership issues
  SceneNode(const SceneNode &) = delete;
  SceneNode &operator=(const SceneNode &) = delete;

  // Allow moving for container storage
  SceneNode(SceneNode &&) noexcept;
  SceneNode &operator=(SceneNode &&) noexcept;

  // -------------------------------------------------------------------------
  // Payload (INode) management
  // -------------------------------------------------------------------------

  /**
   * @brief Create and attach a node payload of type T
   * @tparam T   INode-derived type to create
   * @tparam Args Constructor argument types
   * @return Pointer to the attached node
   *
   * If a node is already attached it is detached first.
   */
  template <typename T, typename... Args> T *SetNode(Args &&...args);

  /**
   * @brief Attach a pre-built node payload
   * @param node Ownership of the node to attach
   *
   * Any previously attached node is detached first.
   */
  void AttachNode(std::unique_ptr<INode> node);

  /**
   * @brief Detach the current payload and return ownership
   * @return Unique pointer to the detached node, or nullptr if none
   */
  std::unique_ptr<INode> DetachNode();

  /**
   * @brief Get payload cast to type T
   * @tparam T Expected node type
   * @return Pointer to the payload if it is of type T, nullptr otherwise
   */
  template <typename T> T *GetNode();

  /**
   * @brief Get payload cast to type T (const)
   */
  template <typename T> const T *GetNode() const;

  /**
   * @brief Get raw payload pointer
   * @return Pointer to the attached INode, nullptr if none
   */
  INode *GetNode();

  /**
   * @brief Get raw payload pointer (const)
   */
  const INode *GetNode() const;

  /**
   * @brief Check if a payload of type T is attached
   */
  template <typename T> bool HasNode() const;

  /**
   * @brief Check if any payload is attached
   */
  bool HasNode() const { return m_node != nullptr; }

  /**
   * @brief Get count of attached payloads (0 or 1)
   */
  size_t GetNodeCount() const { return m_node ? 1 : 0; }

  // -------------------------------------------------------------------------
  // Hierarchy
  // -------------------------------------------------------------------------

  /**
   * @brief Add child node to this node's hierarchy
   * @param child Unique pointer to child node
   * @return Raw pointer to added child for convenience
   */
  SceneNode *AddChild(std::unique_ptr<SceneNode> child);

  /**
   * @brief Remove child from hierarchy
   * @param child Pointer to child to remove
   * @return Unique pointer to removed child, nullptr if not found
   */
  std::unique_ptr<SceneNode> RemoveChild(SceneNode *child);

  /**
   * @brief Get parent node
   * @return Pointer to parent, nullptr if this is root node
   */
  SceneNode *GetParent() const { return m_parent; }

  /**
   * @brief Get all children
   * @return Vector of child node pointers
   */
  std::vector<SceneNode *> GetChildren();

  /**
   * @brief Get all children (const version)
   * @return Vector of const child node pointers
   */
  std::vector<const SceneNode *> GetChildren() const;

  /**
   * @brief Find child by name (recursive search)
   * @param name Name to search for
   * @return Pointer to found child, nullptr if not found
   */
  SceneNode *FindChild(const std::string &name);

  /**
   * @brief Get number of children
   * @return Child count
   */
  size_t GetChildCount() const { return m_children.size(); }

  // -------------------------------------------------------------------------
  // Properties
  // -------------------------------------------------------------------------

  /**
   * @brief Get node name
   */
  const std::string &GetName() const { return m_name; }

  /**
   * @brief Set node name
   */
  void SetName(const std::string &name) { m_name = name; }

  /**
   * @brief Check if node is active
   */
  bool IsActive() const { return m_active; }

  /**
   * @brief Set node active state
   * @param active true to enable, false to disable
   *
   * Propagates to the attached INode payload.
   */
  void SetActive(bool active);

  /**
   * @brief Get unique node ID
   */
  uint32_t GetId() const { return m_id; }

  // -------------------------------------------------------------------------
  // Lifecycle
  // -------------------------------------------------------------------------

  /**
   * @brief Recursively update this node and all children
   * @param deltaTime Time elapsed since last frame
   */
  void Update(double deltaTime);

  /**
   * @brief Recursively render this node and all children
   * @param renderer The active renderer
   */
  void Render(class Renderer *renderer);

private:
  std::unique_ptr<INode> m_node; ///< Single payload node

  // Hierarchy
  std::vector<std::unique_ptr<SceneNode>> m_children;
  SceneNode *m_parent = nullptr;

  // Properties
  std::string m_name;
  uint32_t m_id;
  bool m_active = true;

  // Static ID counter for unique node identification
  static uint32_t s_nextId;

  /**
   * @brief Set parent pointer (called by hierarchy management)
   */
  void SetParent(SceneNode *parent) { m_parent = parent; }
};

} // namespace AIEngine

// Template implementations must be in header for proper instantiation
#include "SceneNode.inl"
