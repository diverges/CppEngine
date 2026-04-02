/**
 * SceneGraph.inl - Template Implementation File
 *
 * Template method implementations for SceneGraph component queries.
 *
 * @version 1.0.0
 * @date 2026-03-04
 */

#pragma once

namespace AIEngine {

template <typename T>
std::vector<SceneNode *> SceneGraph::FindNodesWithComponent() {
  static_assert(std::is_base_of_v<IComponent, T>,
                "T must inherit from IComponent");

  std::vector<SceneNode *> matchingNodes;

  // Use ForEachNode to traverse and collect matching nodes
  ForEachNode([&](SceneNode *node) {
    if (node->HasComponent<T>()) {
      matchingNodes.push_back(node);
    }
  });

  return matchingNodes;
}

template <typename T>
void SceneGraph::ForEachNodeWithComponent(
    std::function<void(SceneNode *, T *)> func, bool activeOnly) {
  static_assert(std::is_base_of_v<IComponent, T>,
                "T must inherit from IComponent");

  if (!func) {
    return; // Invalid function
  }

  ForEachNode(
      [&](SceneNode *node) {
        if (activeOnly && !node->IsActive()) {
          return; // Skip inactive nodes
        }

        T *component = node->GetComponent<T>();
        if (component) {
          func(node, component);
        }
      },
      activeOnly);
}

} // namespace AIEngine