/**
 * SceneGraph.cpp - Scene Graph Manager Implementation
 *
 * Implementation of SceneGraph management functions for scene hierarchy,
 * node registration, traversal, and optimization operations.
 *
 * @version 1.0.0
 * @date 2026-03-04
 */

#include "../../include/AIEngine/scene/SceneGraph.hpp"
#include "../../include/AIEngine/graphics/Renderer.hpp"
#include <algorithm>
#include <sstream>

namespace AIEngine
{

    SceneGraph::SceneGraph()
    {
        // Create root node
        m_rootNode = std::make_unique<SceneNode>("Root");
        RegisterNode(m_rootNode.get());
    }

    SceneGraph::~SceneGraph()
    {
        // Clear all nodes (automatic cleanup via unique_ptr)
        Clear();
        m_nodeRegistry.clear();
        m_nameRegistry.clear();
    }

    SceneGraph::SceneGraph(SceneGraph &&other) noexcept
        : m_rootNode(std::move(other.m_rootNode)), m_nodeRegistry(std::move(other.m_nodeRegistry)), m_nameRegistry(std::move(other.m_nameRegistry)), m_nodeCount(other.m_nodeCount), m_statisticsDirty(other.m_statisticsDirty)
    {

        other.m_nodeCount = 0;
        other.m_statisticsDirty = true;
    }

    SceneGraph &SceneGraph::operator=(SceneGraph &&other) noexcept
    {
        if (this != &other)
        {
            // Clean up current resources
            Clear();
            m_nodeRegistry.clear();
            m_nameRegistry.clear();

            // Move resources
            m_rootNode = std::move(other.m_rootNode);
            m_nodeRegistry = std::move(other.m_nodeRegistry);
            m_nameRegistry = std::move(other.m_nameRegistry);
            m_nodeCount = other.m_nodeCount;
            m_statisticsDirty = other.m_statisticsDirty;

            other.m_nodeCount = 0;
            other.m_statisticsDirty = true;
        }
        return *this;
    }

    std::unique_ptr<SceneNode> SceneGraph::CreateNode(const std::string &name)
    {
        auto node = std::make_unique<SceneNode>(name);

        // Register for lookups (note: this creates a temporary entry that will be
        // updated when the node is actually added to the scene)
        RegisterNode(node.get());

        return node;
    }

    void SceneGraph::Update(double deltaTime)
    {
        if (m_rootNode)
        {
            m_rootNode->Update(deltaTime);
        }

        // Mark statistics as potentially dirty after update
        m_statisticsDirty = true;
    }

    void SceneGraph::Render(Renderer *renderer)
    {
        if (m_rootNode && renderer)
        {
            m_rootNode->Render(renderer);
        }
    }

    SceneNode *SceneGraph::FindNodeById(uint32_t id)
    {
        auto it = m_nodeRegistry.find(id);
        return (it != m_nodeRegistry.end()) ? it->second : nullptr;
    }

    SceneNode *SceneGraph::FindNodeByName(const std::string &name)
    {
        auto it = m_nameRegistry.find(name);
        if (it != m_nameRegistry.end() && !it->second.empty())
        {
            return it->second.front(); // Return first match
        }
        return nullptr;
    }

    std::vector<SceneNode *> SceneGraph::FindAllNodesByName(const std::string &name)
    {
        auto it = m_nameRegistry.find(name);
        return (it != m_nameRegistry.end()) ? it->second : std::vector<SceneNode *>();
    }

    void SceneGraph::ForEachNode(std::function<void(SceneNode *)> func, bool activeOnly)
    {
        if (!func || !m_rootNode)
        {
            return;
        }

        TraverseNodes(m_rootNode.get(), func, activeOnly);
    }

    void SceneGraph::ForEachNode(std::function<void(const SceneNode *)> func, bool activeOnly) const
    {
        if (!func || !m_rootNode)
        {
            return;
        }

        TraverseNodes(m_rootNode.get(), func, activeOnly);
    }

    size_t SceneGraph::GetNodeCount() const
    {
        if (m_statisticsDirty)
        {
            UpdateStatistics();
        }
        return m_nodeCount;
    }

    size_t SceneGraph::GetActiveNodeCount() const
    {
        size_t activeCount = 0;

        if (m_rootNode)
        {
            ForEachNode([&](const SceneNode *node)
                        {
                if (node->IsActive()) {
                    activeCount++;
                } });
        }

        return activeCount;
    }

    void SceneGraph::Clear()
    {
        if (m_rootNode)
        {
            // Remove all children from root (keeps root node itself)
            auto children = m_rootNode->GetChildren();
            for (auto *child : children)
            {
                UnregisterNode(child);
                m_rootNode->RemoveChild(child);
            }
        }

        // Clear registries except for root node
        m_nodeRegistry.clear();
        m_nameRegistry.clear();

        // Re-register root node
        if (m_rootNode)
        {
            RegisterNode(m_rootNode.get());
        }

        m_statisticsDirty = true;
    }

    bool SceneGraph::ValidateIntegrity() const
    {
        if (!m_rootNode)
        {
            return false; // Must have root node
        }

        bool valid = true;

        // Check that all registered nodes are reachable from root
        ForEachNode([&](const SceneNode *node)
                    {
            // Verify node is in registry
            auto it = m_nodeRegistry.find(node->GetId());
            if (it == m_nodeRegistry.end() || it->second != node) {
                valid = false;
            }
            
            // Verify parent-child relationships
            const SceneNode* parent = node->GetParent();
            if (parent) {
                auto children = parent->GetChildren();
                auto childIt = std::find(children.begin(), children.end(), node);
                if (childIt == children.end()) {
                    valid = false; // Child not found in parent's children list
                }
            } });

        return valid;
    }

    std::string SceneGraph::GetDebugInfo() const
    {
        std::stringstream ss;

        size_t totalNodes = GetNodeCount();
        size_t activeNodes = GetActiveNodeCount();
        size_t totalComponents = 0;

        // Count components
        ForEachNode([&](const SceneNode *node)
                    { totalComponents += node->GetComponentCount(); });

        ss << "SceneGraph Debug Info:\n";
        ss << "  Total Nodes: " << totalNodes << "\n";
        ss << "  Active Nodes: " << activeNodes << "\n";
        ss << "  Total Components: " << totalComponents << "\n";
        ss << "  Registered Names: " << m_nameRegistry.size() << "\n";

        return ss.str();
    }

    void SceneGraph::Optimize()
    {
        // Basic optimization: Update statistics and validate integrity
        UpdateStatistics();

        // Future optimizations could include:
        // - Spatial sorting for rendering
        // - Component type grouping
        // - Frustum culling preparation
        // - Memory pool optimization
    }

    void SceneGraph::RegisterNode(SceneNode *node)
    {
        if (!node)
        {
            return;
        }

        // Register by ID
        m_nodeRegistry[node->GetId()] = node;

        // Register by name
        const std::string &name = node->GetName();
        m_nameRegistry[name].push_back(node);

        m_statisticsDirty = true;
    }

    void SceneGraph::UnregisterNode(SceneNode *node)
    {
        if (!node)
        {
            return;
        }

        // Unregister by ID
        m_nodeRegistry.erase(node->GetId());

        // Unregister by name
        const std::string &name = node->GetName();
        auto nameIt = m_nameRegistry.find(name);
        if (nameIt != m_nameRegistry.end())
        {
            auto &nodeList = nameIt->second;
            nodeList.erase(std::remove(nodeList.begin(), nodeList.end(), node), nodeList.end());

            // Remove empty name entry
            if (nodeList.empty())
            {
                m_nameRegistry.erase(nameIt);
            }
        }

        m_statisticsDirty = true;
    }

    void SceneGraph::TraverseNodes(SceneNode *node, std::function<void(SceneNode *)> func, bool activeOnly)
    {
        if (!node || (activeOnly && !node->IsActive()))
        {
            return;
        }

        // Apply function to current node
        func(node);

        // Recursively traverse children
        auto children = node->GetChildren();
        for (auto *child : children)
        {
            TraverseNodes(child, func, activeOnly);
        }
    }

    void SceneGraph::TraverseNodes(const SceneNode *node, std::function<void(const SceneNode *)> func, bool activeOnly) const
    {
        if (!node || (activeOnly && !node->IsActive()))
        {
            return;
        }

        // Apply function to current node
        func(node);

        // Recursively traverse children
        auto children = node->GetChildren();
        for (auto *child : children)
        {
            TraverseNodes(child, func, activeOnly);
        }
    }

    void SceneGraph::UpdateStatistics() const
    {
        m_nodeCount = 0;

        // Count all nodes by traversal
        ForEachNode([&](const SceneNode *node)
                    {
            (void)node; // Suppress unused warning
            m_nodeCount++; });

        m_statisticsDirty = false;
    }

} // namespace AIEngine