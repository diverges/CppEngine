/**
 * SceneNode.cpp - Scene Graph Node Implementation
 *
 * Implementation of SceneNode non-template methods including
 * hierarchy management, lifecycle methods, and utility functions.
 *
 * @version 1.0.0
 * @date 2026-03-04
 */

#include "../../include/AIEngine/scene/SceneNode.hpp"
#include "../../include/AIEngine/graphics/Renderer.hpp"
#include <algorithm>

namespace AIEngine {

// Static ID counter initialization
uint32_t SceneNode::s_nextId = 1;

SceneNode::SceneNode(const std::string &name)
    : m_name(name), m_id(s_nextId++) {}

SceneNode::~SceneNode() {
  // Notify all components of detachment
  for (auto &[typeId, component] : m_components) {
    component->OnDetach(this);
  }

  // Clear components (automatic cleanup via unique_ptr)
  m_components.clear();

  // Clear children (automatic cleanup via unique_ptr)
  m_children.clear();
}

SceneNode::SceneNode(SceneNode &&other) noexcept
    : m_components(std::move(other.m_components)),
      m_children(std::move(other.m_children)), m_parent(other.m_parent),
      m_name(std::move(other.m_name)), m_id(other.m_id),
      m_active(other.m_active) {

  // Update component ownership
  for (auto &[typeId, component] : m_components) {
    component->SetOwner(this);
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
    // Clean up current resources
    for (auto &[typeId, component] : m_components) {
      component->OnDetach(this);
    }

    // Move data
    m_components = std::move(other.m_components);
    m_children = std::move(other.m_children);
    m_parent = other.m_parent;
    m_name = std::move(other.m_name);
    m_id = other.m_id;
    m_active = other.m_active;

    // Update ownership
    for (auto &[typeId, component] : m_components) {
      component->SetOwner(this);
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

std::vector<IComponent *> SceneNode::GetAllComponents() {
  std::vector<IComponent *> components;
  components.reserve(m_components.size());

  for (auto &[typeId, component] : m_components) {
    components.push_back(component.get());
  }

  return components;
}

std::vector<const IComponent *> SceneNode::GetAllComponents() const {
  std::vector<const IComponent *> components;
  components.reserve(m_components.size());

  for (const auto &[typeId, component] : m_components) {
    components.push_back(component.get());
  }

  return components;
}

void SceneNode::Update(double deltaTime) {
  if (!m_active) {
    return; // Skip inactive nodes
  }

  // Update all active components
  for (auto &[typeId, component] : m_components) {
    if (component->IsActive()) {
      component->OnUpdate(deltaTime);
    }
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

  // Render all active components
  for (auto &[typeId, component] : m_components) {
    if (component->IsActive()) {
      component->OnRender(renderer);
    }
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

  // Propagate to components
  for (auto &[typeId, component] : m_components) {
    component->SetActive(active);
  }

  // Note: Children maintain their own active state
  // (parent deactivation doesn't permanently disable children)
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