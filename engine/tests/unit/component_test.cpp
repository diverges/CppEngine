/**
 * Component System Tests
 *
 * Unit tests for the component system architecture including
 * component creation, type safety, and lifecycle management.
 *
 * @version 1.0.0
 * @date 2026-03-04
 */

#include <AIEngine/core/Component.hpp>
#include <memory>

#include "doctest.h"

// Mock component classes for testing
namespace AIEngine {
// Mock transform component for testing
class MockTransformComponent : public Component<MockTransformComponent> {
   public:
    MockTransformComponent() : x(0.0f), y(0.0f), z(0.0f) {}

    void OnUpdate(double deltaTime) override {
        updateCallCount++;
        lastDeltaTime = deltaTime;
    }

    void OnAttach(SceneNode* owner) override {
        m_attached = true;
        Component::OnAttach(owner);
    }

    void OnDetach(SceneNode* owner) override {
        m_attached = false;
        Component::OnDetach(owner);
    }

    // Test data
    float x, y, z;
    int updateCallCount = 0;
    double lastDeltaTime = 0.0;
    bool m_attached = false;
};

// Mock render component for testing
class MockRenderComponent : public Component<MockRenderComponent> {
   public:
    MockRenderComponent() : visible(true), meshId(0) {}

    void OnRender(class Renderer* renderer) override {
        (void)renderer;  // Suppress unused warning
        renderCallCount++;
    }

    bool visible;
    int meshId;
    int renderCallCount = 0;
};
}  // namespace AIEngine

TEST_SUITE("Component System") {
    TEST_CASE("Component type identification") {
        using namespace AIEngine;

        SUBCASE("Type IDs are consistent") {
            auto id1 = MockTransformComponent::GetStaticTypeID();
            auto id2 = MockTransformComponent::GetStaticTypeID();
            CHECK(id1 == id2);
        }

        SUBCASE("Different component types have different IDs") {
            auto transformId = MockTransformComponent::GetStaticTypeID();
            auto renderId = MockRenderComponent::GetStaticTypeID();
            CHECK(transformId != renderId);
        }

        SUBCASE("Instance and static type IDs match") {
            MockTransformComponent component;
            auto instanceId = component.GetTypeID();
            auto staticId = MockTransformComponent::GetStaticTypeID();
            CHECK(instanceId == staticId);
        }
    }

    TEST_CASE("Component lifecycle") {
        using namespace AIEngine;

        MockTransformComponent component;

        SUBCASE("Initial component state") {
            CHECK(component.IsActive() == true);
            CHECK(component.GetOwner() == nullptr);
            CHECK(component.updateCallCount == 0);
        }

        SUBCASE("Component activation/deactivation") {
            component.SetActive(false);
            CHECK(component.IsActive() == false);

            component.SetActive(true);
            CHECK(component.IsActive() == true);
        }

        SUBCASE("Component update behavior") {
            double deltaTime = 0.016;  // ~60 FPS
            component.OnUpdate(deltaTime);

            CHECK(component.updateCallCount == 1);
            CHECK(component.lastDeltaTime == deltaTime);
        }

        SUBCASE("Component attachment simulation") {
            // Note: We can't create a real SceneNode here without more infrastructure
            // so we simulate the attachment process
            SceneNode* mockNode = reinterpret_cast<SceneNode*>(0x1234);  // Mock pointer

            component.OnAttach(mockNode);
            CHECK(component.m_attached == true);

            component.OnDetach(mockNode);
            CHECK(component.m_attached == false);
        }
    }

    TEST_CASE("Component polymorphism") {
        using namespace AIEngine;

        SUBCASE("Component can be used polymorphically") {
            auto component = std::make_unique<MockTransformComponent>();
            IComponent* basePtr = component.get();

            // Should be able to call virtual methods through base pointer
            CHECK_NOTHROW(basePtr->OnUpdate(0.016));
            CHECK_NOTHROW(basePtr->SetActive(false));
            CHECK(basePtr->IsActive() == false);
        }

        SUBCASE("Type information preserved through polymorphism") {
            auto component = std::make_unique<MockTransformComponent>();
            IComponent* basePtr = component.get();

            auto baseTypeId = basePtr->GetTypeID();
            auto derivedTypeId = MockTransformComponent::GetStaticTypeID();

            CHECK(baseTypeId == derivedTypeId);
        }
    }

    TEST_CASE("Component collections") {
        using namespace AIEngine;

        SUBCASE("Different component types in same collection") {
            std::vector<std::unique_ptr<IComponent>> components;

            components.push_back(std::make_unique<MockTransformComponent>());
            components.push_back(std::make_unique<MockRenderComponent>());

            CHECK(components.size() == 2);

            // Verify components have different types
            auto type1 = components[0]->GetTypeID();
            auto type2 = components[1]->GetTypeID();
            CHECK(type1 != type2);
        }

        SUBCASE("Component iteration and updates") {
            std::vector<std::unique_ptr<IComponent>> components;
            components.push_back(std::make_unique<MockTransformComponent>());
            components.push_back(std::make_unique<MockRenderComponent>());

            // Simulate frame update
            double deltaTime = 0.016;
            for (auto& component : components) {
                if (component->IsActive()) {
                    component->OnUpdate(deltaTime);
                }
            }

            // Verify update was called
            auto* transform = dynamic_cast<MockTransformComponent*>(components[0].get());
            CHECK(transform != nullptr);
            CHECK(transform->updateCallCount == 1);
        }
    }
}

// Educational note: Component system testing demonstrates:
// - Type-safe component identification and polymorphism
// - Component lifecycle management (attach/detach/update)
// - Collection handling for heterogeneous component types
// - Virtual method dispatch through base pointers