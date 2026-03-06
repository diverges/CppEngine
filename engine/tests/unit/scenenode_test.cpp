/**
 * scenenode_test.cpp - SceneNode Unit Tests
 *
 * Comprehensive testing of SceneNode functionality including component
 * management, hierarchy operations, and lifecycle behavior.
 *
 * @version 1.0.0
 * @date 2026-03-04
 */

#include <AIEngine/components/RenderComponent.hpp>
#include <AIEngine/components/TransformComponent.hpp>
#include <AIEngine/scene/SceneNode.hpp>
#include <memory>

#include "doctest.h"

// Mock component for testing
namespace AIEngine {
class MockComponent : public Component<MockComponent> {
   public:
    MockComponent() : value(42) {}
    explicit MockComponent(int v) : value(v) {}

    void OnAttach(SceneNode* owner) override {
        Component::OnAttach(owner);
        attachCalled = true;
        attachedOwner = owner;
    }

    void OnDetach(SceneNode* owner) override {
        Component::OnDetach(owner);
        detachCalled = true;
        detachedOwner = owner;
    }

    void OnUpdate(double deltaTime) override {
        updateCalled = true;
        lastDeltaTime = deltaTime;
        updateCount++;
    }

    int value;
    bool attachCalled = false;
    bool detachCalled = false;
    bool updateCalled = false;
    double lastDeltaTime = 0.0;
    int updateCount = 0;
    SceneNode* attachedOwner = nullptr;
    SceneNode* detachedOwner = nullptr;
};
}  // namespace AIEngine

TEST_SUITE("SceneNode Tests") {
    TEST_CASE("SceneNode construction and basic properties") {
        using namespace AIEngine;

        SUBCASE("Default construction") {
            SceneNode node;

            CHECK(node.GetName() == "Node");
            CHECK(node.GetId() > 0);
            CHECK(node.IsActive() == true);
            CHECK(node.GetComponentCount() == 0);
            CHECK(node.GetChildCount() == 0);
            CHECK(node.GetParent() == nullptr);
        }

        SUBCASE("Named construction") {
            SceneNode node("TestNode");

            CHECK(node.GetName() == "TestNode");
            CHECK(node.GetId() > 0);
            CHECK(node.IsActive() == true);
        }

        SUBCASE("Unique IDs") {
            SceneNode node1("Node1");
            SceneNode node2("Node2");

            CHECK(node1.GetId() != node2.GetId());
        }
    }

    TEST_CASE("SceneNode component management") {
        using namespace AIEngine;

        SceneNode node("TestNode");

        SUBCASE("Add and retrieve components") {
            // Add first component
            auto* mock1 = node.AddComponent<MockComponent>();
            REQUIRE(mock1 != nullptr);
            CHECK(mock1->attachCalled == true);
            CHECK(mock1->attachedOwner == &node);
            CHECK(node.GetComponentCount() == 1);

            // Add second component
            auto* transform = node.AddComponent<TransformComponent>();
            REQUIRE(transform != nullptr);
            CHECK(node.GetComponentCount() == 2);

            // Retrieve components
            auto* retrieved1 = node.GetComponent<MockComponent>();
            auto* retrieved2 = node.GetComponent<TransformComponent>();

            CHECK(retrieved1 == mock1);
            CHECK(retrieved2 == transform);
        }

        SUBCASE("Component with constructor parameters") {
            auto* mock = node.AddComponent<MockComponent>(123);
            REQUIRE(mock != nullptr);
            CHECK(mock->value == 123);
        }

        SUBCASE("Adding duplicate component types returns existing") {
            auto* mock1 = node.AddComponent<MockComponent>();
            auto* mock2 = node.AddComponent<MockComponent>();

            CHECK(mock1 == mock2);
            CHECK(node.GetComponentCount() == 1);
        }

        SUBCASE("Has component checks") {
            CHECK(node.HasComponent<MockComponent>() == false);
            CHECK(node.HasComponent<TransformComponent>() == false);

            node.AddComponent<MockComponent>();

            CHECK(node.HasComponent<MockComponent>() == true);
            CHECK(node.HasComponent<TransformComponent>() == false);
        }

        SUBCASE("Remove components") {
            auto* mock = node.AddComponent<MockComponent>();
            auto* transform = node.AddComponent<TransformComponent>();

            CHECK(node.GetComponentCount() == 2);

            bool removed = node.RemoveComponent<MockComponent>();
            CHECK(removed == true);
            CHECK(mock->detachCalled == true);
            CHECK(mock->detachedOwner == &node);
            CHECK(node.GetComponentCount() == 1);
            CHECK(node.HasComponent<MockComponent>() == false);
            CHECK(node.HasComponent<TransformComponent>() == true);

            bool removedAgain = node.RemoveComponent<MockComponent>();
            CHECK(removedAgain == false);
        }

        SUBCASE("Get all components") {
            node.AddComponent<MockComponent>();
            node.AddComponent<TransformComponent>();

            auto components = node.GetAllComponents();
            CHECK(components.size() == 2);

            // Check that both component types are present
            bool hasMock = false, hasTransform = false;
            for (auto* comp : components) {
                if (comp->GetTypeID() == MockComponent::GetStaticTypeID()) {
                    hasMock = true;
                } else if (comp->GetTypeID() == TransformComponent::GetStaticTypeID()) {
                    hasTransform = true;
                }
            }

            CHECK(hasMock);
            CHECK(hasTransform);
        }
    }

    TEST_CASE("SceneNode active state management") {
        using namespace AIEngine;

        SceneNode node("TestNode");
        auto* mock = node.AddComponent<MockComponent>();

        SUBCASE("Set active state") {
            CHECK(node.IsActive() == true);
            CHECK(mock->IsActive() == true);

            node.SetActive(false);

            CHECK(node.IsActive() == false);
            CHECK(mock->IsActive() == false);

            node.SetActive(true);

            CHECK(node.IsActive() == true);
            CHECK(mock->IsActive() == true);
        }

        SUBCASE("Update skips inactive nodes") {
            mock->updateCalled = false;

            node.SetActive(false);
            node.Update(0.016);

            CHECK(mock->updateCalled == false);

            node.SetActive(true);
            node.Update(0.016);

            CHECK(mock->updateCalled == true);
        }
    }

    TEST_CASE("SceneNode hierarchy management") {
        using namespace AIEngine;

        SceneNode parent("Parent");

        SUBCASE("Add and remove children") {
            auto child = std::make_unique<SceneNode>("Child");
            SceneNode* childPtr = child.get();

            CHECK(parent.GetChildCount() == 0);
            CHECK(child->GetParent() == nullptr);

            // Add child
            SceneNode* addedChild = parent.AddChild(std::move(child));

            CHECK(addedChild == childPtr);
            CHECK(parent.GetChildCount() == 1);
            CHECK(childPtr->GetParent() == &parent);

            // Remove child
            auto removedChild = parent.RemoveChild(childPtr);

            CHECK(removedChild.get() == childPtr);
            CHECK(parent.GetChildCount() == 0);
            CHECK(childPtr->GetParent() == nullptr);
        }

        SUBCASE("Get children") {
            auto child1 = std::make_unique<SceneNode>("Child1");
            auto child2 = std::make_unique<SceneNode>("Child2");

            SceneNode* child1Ptr = child1.get();
            SceneNode* child2Ptr = child2.get();

            parent.AddChild(std::move(child1));
            parent.AddChild(std::move(child2));

            auto children = parent.GetChildren();
            CHECK(children.size() == 2);
            CHECK(std::find(children.begin(), children.end(), child1Ptr) != children.end());
            CHECK(std::find(children.begin(), children.end(), child2Ptr) != children.end());
        }

        SUBCASE("Find child by name") {
            auto child1 = std::make_unique<SceneNode>("Target");
            auto child2 = std::make_unique<SceneNode>("Other");

            SceneNode* targetPtr = child1.get();

            parent.AddChild(std::move(child1));
            parent.AddChild(std::move(child2));

            SceneNode* found = parent.FindChild("Target");
            CHECK(found == targetPtr);

            SceneNode* notFound = parent.FindChild("NonExistent");
            CHECK(notFound == nullptr);
        }

        SUBCASE("Reparenting") {
            SceneNode newParent("NewParent");
            auto child = std::make_unique<SceneNode>("Child");

            SceneNode* childPtr = child.get();

            // Add to first parent
            parent.AddChild(std::move(child));
            CHECK(childPtr->GetParent() == &parent);
            CHECK(parent.GetChildCount() == 1);

            // Move to new parent
            auto removedChild = parent.RemoveChild(childPtr);
            newParent.AddChild(std::move(removedChild));

            CHECK(childPtr->GetParent() == &newParent);
            CHECK(parent.GetChildCount() == 0);
            CHECK(newParent.GetChildCount() == 1);
        }
    }

    TEST_CASE("SceneNode update and render lifecycle") {
        using namespace AIEngine;

        SceneNode parent("Parent");
        auto child = std::make_unique<SceneNode>("Child");

        auto* parentMock = parent.AddComponent<MockComponent>();
        auto* childMock = child->AddComponent<MockComponent>();

        SceneNode* childPtr = child.get();
        parent.AddChild(std::move(child));

        SUBCASE("Recursive update") {
            double deltaTime = 0.016;
            parent.Update(deltaTime);

            CHECK(parentMock->updateCalled == true);
            CHECK(childMock->updateCalled == true);
            CHECK(parentMock->lastDeltaTime == deltaTime);
            CHECK(childMock->lastDeltaTime == deltaTime);
        }

        SUBCASE("Update respects active state") {
            childPtr->SetActive(false);

            parent.Update(0.016);

            CHECK(parentMock->updateCalled == true);
            CHECK(childMock->updateCalled == false);
        }

        SUBCASE("Render with mock renderer") {
            // Note: We can't test actual rendering without a real renderer
            // This tests that Render() can be called without crashing
            CHECK_NOTHROW(parent.Render(nullptr));
        }
    }

    TEST_CASE("SceneNode move semantics") {
        using namespace AIEngine;

        SUBCASE("Move construction") {
            SceneNode original("Original");
            original.AddComponent<MockComponent>(99);

            uint32_t originalId = original.GetId();

            SceneNode moved(std::move(original));

            CHECK(moved.GetName() == "Original");
            CHECK(moved.GetId() == originalId);
            CHECK(moved.HasComponent<MockComponent>() == true);

            auto* comp = moved.GetComponent<MockComponent>();
            CHECK(comp->value == 99);
            CHECK(comp->GetOwner() == &moved);  // Owner updated
        }

        SUBCASE("Move assignment") {
            SceneNode original("Original");
            SceneNode target("Target");

            original.AddComponent<MockComponent>(88);
            uint32_t originalId = original.GetId();

            target = std::move(original);

            CHECK(target.GetName() == "Original");
            CHECK(target.GetId() == originalId);
            CHECK(target.HasComponent<MockComponent>() == true);

            auto* comp = target.GetComponent<MockComponent>();
            CHECK(comp->value == 88);
            CHECK(comp->GetOwner() == &target);
        }
    }

    TEST_CASE("SceneNode name management") {
        using namespace AIEngine;

        SceneNode node("Initial");

        CHECK(node.GetName() == "Initial");

        node.SetName("Updated");
        CHECK(node.GetName() == "Updated");

        node.SetName("");
        CHECK(node.GetName() == "");
    }
}

// Educational note: These tests demonstrate:
// - Component lifecycle management (attach/detach)
// - Hierarchy operations and parent-child relationships
// - Active state propagation and update behavior
// - Move semantics and resource transfer
// - Type-safe component retrieval and management