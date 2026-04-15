/**
 * Node.hpp - Base Node Interface
 *
 * Defines the base INode interface for the node system,
 * enabling composition-based entity architecture where SceneNodes
 * can have multiple nodes attached for different behaviors.
 *
 * @version 1.0.0
 * @date 2026-03-04
 */

#pragma once

#include <memory>
#include <string>
#include <typeinfo>

namespace AIEngine {

// Forward declarations
class SceneNode;

/**
 * @brief Node Type ID for runtime type identification
 *
 * Uses type_info for runtime node identification
 * without requiring RTTI in all cases.
 */
using NodeTypeID = const std::type_info *;

/**
 * @brief Get node type ID for template type
 * @tparam T Node type
 * @return Type identifier for runtime node queries
 */
template <typename T> NodeTypeID GetNodeTypeID() { return &typeid(T); }

/**
 * @brief Base Node Interface
 *
 * All nodes must inherit from INode to be attachable
 * to SceneNodes. Nodes provide specific functionality like
 * transforms, rendering, physics, audio, etc.
 *
 * Nodes follow composition over inheritance pattern:
 * - Each node handles one specific aspect
 * - SceneNodes combine multiple nodes
 * - Loose coupling between node types
 *
 * @example Creating a Custom Node
 * @code
 * class HealthNode : public AIEngine::INode {
 * public:
 *     HealthNode() : m_health(100), m_maxHealth(100) {}
 *
 *     void OnAttach(SceneNode* owner) override {
 *         m_owner = owner;
 *         // Initialize health UI, etc.
 *     }
 *
 *     int GetHealth() const { return static_cast<int>(m_health); }
 *     void TakeDamage(int damage) { m_health -= damage; }
 *
 * private:
 *     float m_health;
 *     float m_maxHealth;
 *     SceneNode* m_owner = nullptr;
 * };
 * @endcode
 */
class INode {
public:
  /**
   * @brief Virtual destructor for proper cleanup
   */
  virtual ~INode() = default;

  /**
   * @brief Called when node is attached to a SceneNode
   * @param owner The SceneNode this node is attached to
   *
   * Override this to perform node-specific initialization
   * that requires access to the owning node or other nodes.
   */
  virtual void OnAttach(SceneNode *owner) { (void)owner; }

  /**
   * @brief Called when node is detached from a SceneNode
   * @param owner The SceneNode this node was attached to
   *
   * Override this to perform cleanup when node is removed.
   */
  virtual void OnDetach(SceneNode *owner) { (void)owner; }

  /**
   * @brief Get the type ID for this node instance
   * @return Type identifier
   */
  virtual NodeTypeID GetTypeID() const = 0;

  /**
   * @brief Get the type name for this node instance
   * @return Human-readable type name
   */
  virtual std::string GetTypeName() const = 0;

  /**
   * @brief Check whether this node is active
   * @return true if active
   */
  virtual bool IsActive() const { return m_active; }

  /**
   * @brief Set active state for this node
   * @param active New active state
   */
  virtual void SetActive(bool active) { m_active = active; }

  /**
   * @brief Get the SceneNode this node is attached to
   * @return Pointer to owner node, nullptr if not attached
   */
  SceneNode *GetOwner() const { return m_owner; }

protected:
  /**
   * @brief Set owner node (called by SceneNode attachment system)
   * @param owner The node that owns this node
   */
  void SetOwner(SceneNode *owner) { m_owner = owner; }

  // Allow SceneNode to manage node ownership
  friend class SceneNode;

private:
  SceneNode *m_owner = nullptr; ///< Owning scene node
  bool m_active = true;         ///< Node active state
};

/**
 * @brief Templated Node Base Class
 *
 * Provides automatic type ID generation for node types.
 * All concrete nodes should inherit from this rather than
 * INode directly.
 *
 * @tparam Derived The derived node class (CRTP)
 *
 * @example Using Node Base
 * @code
 * class TransformNode : public AIEngine::Node<TransformNode> {
 * public:
 *     // Node implementation...
 * };
 * @endcode
 */
template <typename Derived> class Node : public INode {
public:
  /**
   * @brief Get type ID for this node type
   * @return Type identifier for Derived node
   */
  NodeTypeID GetTypeID() const override { return GetNodeTypeID<Derived>(); }

  /**
   * @brief Get type name for this node type
   * @return Human-readable type name
   */
  std::string GetTypeName() const override { return typeid(Derived).name(); }

  /**
   * @brief Static type ID getter for class-level queries
   * @return Type identifier for Derived node
   */
  static NodeTypeID GetStaticTypeID() { return GetNodeTypeID<Derived>(); }

  /**
   * @brief Static type name getter
   * @return Human-readable type name
   */
  static std::string GetStaticTypeName() { return typeid(Derived).name(); }
};

// Type aliases for common node operations
using NodePtr = std::shared_ptr<INode>;
using NodeUniquePtr = std::unique_ptr<INode>;

} // namespace AIEngine

// Educational notes:
// - CRTP (Curiously Recurring Template Pattern) provides automatic type ID
// generation
// - Virtual destructor enables polymorphic deletion through base pointer
// - OnAttach/OnDetach provide node lifecycle hooks
// - Active/inactive state allows temporarily disabling nodes
// - Type-safe node system prevents casting errors at runtime
