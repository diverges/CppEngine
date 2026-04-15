/**
 * SceneGraph.cpp - Scene Graph Manager Implementation
 *
 * Implementation of SceneGraph management functions for scene hierarchy,
 * node registration, traversal, and optimization operations.
 *
 * @version 1.0.0
 * @date 2026-03-04
 */

#include "../graphics/Renderer.hpp"
#include <AIEngine/scene/SceneGraph.hpp>
#include <algorithm>
#include <sstream>

namespace AIEngine {

SceneGraph::SceneGraph() {
  // Create root node
  m_rootNode = std::make_unique<SceneNode>("Root");
}

SceneGraph::~SceneGraph() {}

SceneGraph::SceneGraph(SceneGraph &&other) noexcept
    : m_rootNode(std::move(other.m_rootNode)) {}

SceneGraph &SceneGraph::operator=(SceneGraph &&other) noexcept {
  if (this != &other) {
    m_rootNode = std::move(other.m_rootNode);
  }
  return *this;
}

std::unique_ptr<SceneNode> SceneGraph::CreateNode(const std::string &name) {
  auto node = std::make_unique<SceneNode>(name);
  return node;
}

std::string SceneGraph::GetDebugInfo() const {
  std::stringstream ss;

  ss << "SceneGraph Debug Info:\n";
  return ss.str();
}

} // namespace AIEngine
