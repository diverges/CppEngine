/**
 * SceneNode.inl - Template Implementation File
 *
 * Template method implementations for SceneNode component management.
 * This file is included at the end of SceneNode.hpp.
 *
 * @version 1.0.0
 * @date 2026-03-04
 */

#pragma once

namespace AIEngine {

template <typename T, typename... Args>
T *SceneNode::AddComponent(Args &&...args) {
  static_assert(std::is_base_of_v<IComponent, T>,
                "T must inherit from IComponent");

  auto typeId = T::GetStaticTypeID();

  // Check if component already exists
  auto it = m_components.find(typeId);
  if (it != m_components.end()) {
    // Component already exists, return existing
    return static_cast<T *>(it->second.get());
  }

  // Create new component
  auto component = std::make_unique<T>(std::forward<Args>(args)...);
  T *componentPtr = component.get();

  // Add to component map
  m_components[typeId] = std::move(component);

  // Set component ownership and notify attachment
  componentPtr->SetOwner(this);
  componentPtr->OnAttach(this);

  return componentPtr;
}

template <typename T> T *SceneNode::GetComponent() {
  static_assert(std::is_base_of_v<IComponent, T>,
                "T must inherit from IComponent");

  auto typeId = T::GetStaticTypeID();
  auto it = m_components.find(typeId);

  if (it != m_components.end()) {
    return static_cast<T *>(it->second.get());
  }

  return nullptr;
}

template <typename T> const T *SceneNode::GetComponent() const {
  static_assert(std::is_base_of_v<IComponent, T>,
                "T must inherit from IComponent");

  auto typeId = T::GetStaticTypeID();
  auto it = m_components.find(typeId);

  if (it != m_components.end()) {
    return static_cast<const T *>(it->second.get());
  }

  return nullptr;
}

template <typename T> bool SceneNode::RemoveComponent() {
  static_assert(std::is_base_of_v<IComponent, T>,
                "T must inherit from IComponent");

  auto typeId = T::GetStaticTypeID();
  auto it = m_components.find(typeId);

  if (it != m_components.end()) {
    // Notify component of detachment
    it->second->OnDetach(this);

    // Remove from map
    m_components.erase(it);
    return true;
  }

  return false;
}

template <typename T> bool SceneNode::HasComponent() const {
  static_assert(std::is_base_of_v<IComponent, T>,
                "T must inherit from IComponent");

  auto typeId = T::GetStaticTypeID();
  return m_components.find(typeId) != m_components.end();
}

} // namespace AIEngine