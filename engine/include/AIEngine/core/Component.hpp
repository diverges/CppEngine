/**
 * Component.hpp - Base Component Interface
 *
 * Defines the base IComponent interface for the component system,
 * enabling composition-based entity architecture where SceneNodes
 * can have multiple components attached for different behaviors.
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
 * @brief Component Type ID for runtime type identification
 *
 * Uses type_info for runtime component identification
 * without requiring RTTI in all cases.
 */
using ComponentTypeID = const std::type_info *;

/**
 * @brief Get component type ID for template type
 * @tparam T Component type
 * @return Type identifier for runtime component queries
 */
template <typename T> ComponentTypeID GetComponentTypeID() {
  return &typeid(T);
}

/**
 * @brief Base Component Interface
 *
 * All components must inherit from IComponent to be attachable
 * to SceneNodes. Components provide specific functionality like
 * transforms, rendering, physics, audio, etc.
 *
 * Components follow composition over inheritance pattern:
 * - Each component handles one specific aspect
 * - SceneNodes combine multiple components
 * - Loose coupling between component types
 *
 * @example Creating a Custom Component
 * @code
 * class HealthComponent : public AIEngine::IComponent {
 * public:
 *     HealthComponent() : m_health(100), m_maxHealth(100) {}
 *
 *     void OnAttach(SceneNode* owner) override {
 *         m_owner = owner;
 *         // Initialize health UI, etc.
 *     }
 *
 *     void OnUpdate(double deltaTime) override {
 *         // Health regeneration logic
 *         if (m_health < m_maxHealth) {
 *             m_health += 10.0f * deltaTime; // Regen 10/sec
 *         }
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
class IComponent {
public:
  /**
   * @brief Virtual destructor for proper cleanup
   */
  virtual ~IComponent() = default;

  /**
   * @brief Called when component is attached to a SceneNode
   * @param owner The SceneNode this component is attached to
   *
   * Override this to perform component-specific initialization
   * that requires access to the owning node or other components.
   */
  virtual void OnAttach(SceneNode *owner) { (void)owner; }

  /**
   * @brief Called when component is detached from a SceneNode
   * @param owner The SceneNode this component was attached to
   *
   * Override this to perform cleanup when component is removed.
   */
  virtual void OnDetach(SceneNode *owner) { (void)owner; }

  /**
   * @brief Update component state each frame
   * @param deltaTime Time elapsed since last frame (seconds)
   *
   * Override this to implement frame-by-frame component logic
   * like animation, physics updates, AI behavior, etc.
   */
  virtual void OnUpdate(double deltaTime) { (void)deltaTime; }

  /**
   * @brief Called during rendering phase
   * @param renderer The active renderer for drawing operations
   *
   * Override this for components that need to perform rendering.
   * Note: Most rendering should go through RenderComponent,
   * this is for special rendering needs.
   */
  virtual void OnRender(class Renderer *renderer) { (void)renderer; }

  /**
   * @brief Get component type ID for runtime identification
   * @return Type identifier for this component
   *
   * Used internally by the component system for type-safe
   * component retrieval and management.
   */
  virtual ComponentTypeID GetTypeID() const = 0;

  /**
   * @brief Get human-readable component name
   * @return Name string for debugging and logging
   */
  virtual std::string GetTypeName() const = 0;

  /**
   * @brief Check if component is currently active
   * @return true if component should process updates/rendering
   */
  virtual bool IsActive() const { return m_active; }

  /**
   * @brief Set component active state
   * @param active true to enable, false to disable component
   *
   * Inactive components skip OnUpdate() and OnRender() calls.
   */
  virtual void SetActive(bool active) { m_active = active; }

  /**
   * @brief Get the SceneNode this component is attached to
   * @return Pointer to owner node, nullptr if not attached
   */
  SceneNode *GetOwner() const { return m_owner; }

protected:
  /**
   * @brief Set owner node (called by SceneNode attachment system)
   * @param owner The node that owns this component
   */
  void SetOwner(SceneNode *owner) { m_owner = owner; }

  // Allow SceneNode to manage component ownership
  friend class SceneNode;

private:
  SceneNode *m_owner = nullptr; ///< Owning scene node
  bool m_active = true;         ///< Component active state
};

/**
 * @brief Templated Component Base Class
 *
 * Provides automatic type ID generation for component types.
 * All concrete components should inherit from this rather than
 * IComponent directly.
 *
 * @tparam Derived The derived component class (CRTP)
 *
 * @example Using Component Base
 * @code
 * class TransformComponent : public AIEngine::Component<TransformComponent> {
 * public:
 *     // Component implementation...
 *     void OnUpdate(double deltaTime) override {
 *         // Transform-specific update logic
 *     }
 * };
 * @endcode
 */
template <typename Derived> class Component : public IComponent {
public:
  /**
   * @brief Get type ID for this component type
   * @return Type identifier for Derived component
   */
  ComponentTypeID GetTypeID() const override {
    return GetComponentTypeID<Derived>();
  }

  /**
   * @brief Get type name for this component type
   * @return Human-readable type name
   */
  std::string GetTypeName() const override { return typeid(Derived).name(); }

  /**
   * @brief Static type ID getter for class-level queries
   * @return Type identifier for Derived component
   */
  static ComponentTypeID GetStaticTypeID() {
    return GetComponentTypeID<Derived>();
  }

  /**
   * @brief Static type name getter
   * @return Human-readable type name
   */
  static std::string GetStaticTypeName() { return typeid(Derived).name(); }
};

// Type aliases for common component operations
using ComponentPtr = std::shared_ptr<IComponent>;
using ComponentUniquePtr = std::unique_ptr<IComponent>;

} // namespace AIEngine

// Educational notes:
// - CRTP (Curiously Recurring Template Pattern) provides automatic type ID
// generation
// - Virtual destructor enables polymorphic deletion through base pointer
// - OnAttach/OnDetach provide component lifecycle hooks
// - Active/inactive state allows temporarily disabling components
// - Type-safe component system prevents casting errors at runtime