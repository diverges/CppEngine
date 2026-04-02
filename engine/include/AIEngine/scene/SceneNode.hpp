/**
 * SceneNode.hpp - Scene Graph Node Declaration
 *
 * The SceneNode class represents individual objects in the scene hierarchy.
 * Each node can have multiple components attached and forms parent-child
 * relationships to create complex scenes.
 *
 * @version 1.0.0
 * @date 2026-03-04
 */

#pragma once

#include "../core/Component.hpp"
#include <algorithm>
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace AIEngine {

/**
 * @brief Scene Graph Node
 *
 * Represents an object in the scene hierarchy that can have components
 * attached for various behaviors (transform, rendering, physics, etc.).
 * Nodes can form parent-child relationships to create complex scenes.
 *
 * Features:
 * - Component attachment and management
 * - Hierarchical parent-child relationships
 * - Active/inactive state propagation
 * - Automatic component lifecycle management
 * - Type-safe component retrieval
 *
 * @example Basic Usage
 * @code
 * // Create a scene node
 * auto node = std::make_unique<SceneNode>("Player");
 *
 * // Add components
 * auto* transform = node->AddComponent<TransformComponent>();
 * transform->SetPosition(10.0f, 5.0f, 2.0f);
 *
 * auto* render = node->AddComponent<RenderComponent>();
 * render->SetMeshId(playerMeshId);
 *
 * // Create hierarchy
 * auto weapon = std::make_unique<SceneNode>("Weapon");
 * weapon->AddComponent<TransformComponent>();
 * weapon->AddComponent<RenderComponent>();
 * node->AddChild(std::move(weapon));
 *
 * // Update the hierarchy
 * node->Update(deltaTime);
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
   * @brief Virtual destructor for proper cleanup
   */
  virtual ~SceneNode();

  // Prevent copying to avoid component ownership issues
  SceneNode(const SceneNode &) = delete;
  SceneNode &operator=(const SceneNode &) = delete;

  // Allow moving for container storage
  SceneNode(SceneNode &&) noexcept;
  SceneNode &operator=(SceneNode &&) noexcept;

  /**
   * @brief Add component to this node
   * @tparam T Component type to add
   * @tparam Args Constructor argument types
   * @param args Arguments to pass to component constructor
   * @return Pointer to created component, nullptr on failure
   *
   * Creates a new component of type T and attaches it to this node.
   * If a component of this type already exists, returns the existing one.
   */
  template <typename T, typename... Args> T *AddComponent(Args &&...args);

  /**
   * @brief Get component of specified type
   * @tparam T Component type to retrieve
   * @return Pointer to component if found, nullptr otherwise
   */
  template <typename T> T *GetComponent();

  /**
   * @brief Get component of specified type (const version)
   * @tparam T Component type to retrieve
   * @return Const pointer to component if found, nullptr otherwise
   */
  template <typename T> const T *GetComponent() const;

  /**
   * @brief Remove component of specified type
   * @tparam T Component type to remove
   * @return true if component was removed, false if not found
   */
  template <typename T> bool RemoveComponent();

  /**
   * @brief Check if node has component of specified type
   * @tparam T Component type to check
   * @return true if component exists
   */
  template <typename T> bool HasComponent() const;

  /**
   * @brief Get all components attached to this node
   * @return Vector of component pointers
   */
  std::vector<IComponent *> GetAllComponents();

  /**
   * @brief Get all components (const version)
   * @return Vector of const component pointers
   */
  std::vector<const IComponent *> GetAllComponents() const;

  /**
   * @brief Update node and all components (called each frame)
   * @param deltaTime Time elapsed since last frame
   */
  void Update(double deltaTime);

  /**
   * @brief Render node and all components
   * @param renderer The active renderer
   */
  void Render(class Renderer *renderer);

  /**
   * @brief Get node name
   * @return Node identifier string
   */
  const std::string &GetName() const { return m_name; }

  /**
   * @brief Set node name
   * @param name New name for the node
   */
  void SetName(const std::string &name) { m_name = name; }

  /**
   * @brief Check if node is active
   * @return true if node processes updates and rendering
   */
  bool IsActive() const { return m_active; }

  /**
   * @brief Set node active state
   * @param active true to enable, false to disable
   *
   * Inactive nodes skip Update() and Render() calls for themselves
   * and their children, and their components remain inactive.
   */
  void SetActive(bool active);

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
   * @brief Get number of components attached
   * @return Component count
   */
  size_t GetComponentCount() const { return m_components.size(); }

  /**
   * @brief Get number of children
   * @return Child count
   */
  size_t GetChildCount() const { return m_children.size(); }

  /**
   * @brief Get unique node ID
   * @return Node identifier
   */
  uint32_t GetId() const { return m_id; }

private:
  // Component storage using type ID as key
  std::unordered_map<ComponentTypeID, std::unique_ptr<IComponent>> m_components;

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
   * @param parent New parent node
   */
  void SetParent(SceneNode *parent) { m_parent = parent; }
};

} // namespace AIEngine

// Template implementations must be in header for proper instantiation
#include "SceneNode.inl"