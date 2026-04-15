/**
 * SceneNode.cpp - Scene Graph Node Implementation
 *
 * Implementation of SceneNode non-template methods including
 * hierarchy management, lifecycle methods, and utility functions.
 *
 * @version 1.0.0
 * @date 2026-03-04
 */

#include <AIEngine/scene/SceneNode.hpp>
#include "../graphics/Renderer.hpp"
#include <algorithm>

namespace AIEngine {

// Static ID counter initialization
uint32_t SceneNode::s_nextId = 1;

SceneNode::SceneNode(const std::string &name)
    : m_name(name), m_id(s_nextId++) {}

SceneNode::~SceneNode() {
  // Notify payload of detachment
  if (m_node) {
    m_node->OnDetach(this);
  }

  // Clear children (automatic cleanup via unique_ptr)
  m_children.clear();
}

SceneNode::SceneNode(SceneNode &&other) noexcept
    : m_node(std::move(other.m_node)), m_children(std::move(other.m_children)),
      m_parent(other.m_parent), m_name(std::move(other.m_name)),
      m_id(other.m_id), m_active(other.m_active) {

  // Update payload ownership
  if (m_node) {
    m_node->SetOwner(this);
  }

  // Update child parent pointers
  for (auto &child : m_children) {
    child->SetParent(this);
  }

  // Clear moved-from object
  other.m_parent = nullptr;
  other.m_id = 0;
}

SceneNode &SceneNode::operator=(SceneNode &&other) noexcept {
  if (this != &other) {
    // Clean up current payload
    if (m_node) {
      m_node->OnDetach(this);
    }

    // Move data
    m_node = std::move(other.m_node);
    m_children = std::move(other.m_children);
    m_parent = other.m_parent;
    m_name = std::move(other.m_name);
    m_id = other.m_id;
    m_active = other.m_active;

    // Update ownership
    if (m_node) {
      m_node->SetOwner(this);
    }

    for (auto &child : m_children) {
      child->SetParent(this);
    }

    // Clear moved-from object
    other.m_parent = nullptr;
    other.m_id = 0;
  }
  return *this;
}

void SceneNode::AttachNode(std::unique_ptr<INode> node) {
  if (m_node) {
    m_node->OnDetach(this);
    m_node->SetOwner(nullptr);
  }
  m_node = std::move(node);
  if (m_node) {
    m_node->SetOwner(this);
    m_node->OnAttach(this);
  }
}

std::unique_ptr<INode> SceneNode::DetachNode() {
  if (m_node) {
    m_node->OnDetach(this);
    m_node->SetOwner(nullptr);
  }
  return std::move(m_node);
}

INode *SceneNode::GetNode() { return m_node.get(); }

const INode *SceneNode::GetNode() const { return m_node.get(); }

void SceneNode::Update(double deltaTime) {
  if (!m_active) {
    return; // Skip inactive nodes
  }

  // Update all children recursively
  for (auto &child : m_children) {
    child->Update(deltaTime);
  }
}

void SceneNode::Render(Renderer *renderer) {
  if (!m_active || !renderer) {
    return; // Skip inactive nodes or invalid renderer
  }

  // Render all children recursively
  for (auto &child : m_children) {
    child->Render(renderer);
  }
}

void SceneNode::SetActive(bool active) {
  if (m_active == active) {
    return; // No change
  }

  m_active = active;

  // Propagate to payload
  if (m_node) {
    m_node->SetActive(active);
  }

  // Note: Children maintain their own active state
}

SceneNode *SceneNode::AddChild(std::unique_ptr<SceneNode> child) {
  if (!child) {
    return nullptr;
  }

  // Remove child from previous parent if it has one
  if (child->m_parent) {
    child->m_parent->RemoveChild(child.get());
  }

  // Set parent-child relationship
  child->SetParent(this);
  SceneNode *childPtr = child.get();

  // Add to children list
  m_children.push_back(std::move(child));

  return childPtr;
}

std::unique_ptr<SceneNode> SceneNode::RemoveChild(SceneNode *child) {
  if (!child) {
    return nullptr;
  }

  auto it = std::find_if(m_children.begin(), m_children.end(),
                         [child](const std::unique_ptr<SceneNode> &ptr) {
                           return ptr.get() == child;
                         });

  if (it != m_children.end()) {
    auto removedChild = std::move(*it);
    removedChild->SetParent(nullptr);
    m_children.erase(it);
    return removedChild;
  }

  return nullptr;
}

std::vector<SceneNode *> SceneNode::GetChildren() {
  std::vector<SceneNode *> children;
  children.reserve(m_children.size());

  for (auto &child : m_children) {
    children.push_back(child.get());
  }

  return children;
}

std::vector<const SceneNode *> SceneNode::GetChildren() const {
  std::vector<const SceneNode *> children;
  children.reserve(m_children.size());

  for (const auto &child : m_children) {
    children.push_back(child.get());
  }

  return children;
}

SceneNode *SceneNode::FindChild(const std::string &name) {
  // Search immediate children first
  for (auto &child : m_children) {
    if (child->GetName() == name) {
      return child.get();
    }
  }

  // Recursive search in children
  for (auto &child : m_children) {
    SceneNode *found = child->FindChild(name);
    if (found) {
      return found;
    }
  }

  return nullptr; // Not found
}

} // namespace AIEngine
