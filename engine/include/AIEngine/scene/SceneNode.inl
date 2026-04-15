/**
 * SceneNode.inl - Template Implementation File
 *
 * Template method implementations for single-payload node management.
 * This file is included at the end of SceneNode.hpp.
 *
 * @version 1.0.0
 * @date 2026-03-04
 */

#pragma once

namespace AIEngine {

template <typename T, typename... Args> T *SceneNode::SetNode(Args &&...args) {
  static_assert(std::is_base_of_v<INode, T>, "T must inherit from INode");

  // Detach existing node first
  if (m_node) {
    m_node->OnDetach(this);
    m_node->SetOwner(nullptr);
  }

  // Create and attach new node
  auto newNode = std::make_unique<T>(std::forward<Args>(args)...);
  T *nodePtr = newNode.get();
  m_node = std::move(newNode);

  nodePtr->SetOwner(this);
  nodePtr->OnAttach(this);

  return nodePtr;
}

template <typename T> T *SceneNode::GetNode() {
  static_assert(std::is_base_of_v<INode, T>, "T must inherit from INode");

  if (m_node && m_node->GetTypeID() == T::GetStaticTypeID()) {
    return static_cast<T *>(m_node.get());
  }
  return nullptr;
}

template <typename T> const T *SceneNode::GetNode() const {
  static_assert(std::is_base_of_v<INode, T>, "T must inherit from INode");

  if (m_node && m_node->GetTypeID() == T::GetStaticTypeID()) {
    return static_cast<const T *>(m_node.get());
  }
  return nullptr;
}

template <typename T> bool SceneNode::HasNode() const {
  static_assert(std::is_base_of_v<INode, T>, "T must inherit from INode");

  return m_node && m_node->GetTypeID() == T::GetStaticTypeID();
}

} // namespace AIEngine