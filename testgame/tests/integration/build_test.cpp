/**
 * build_test.cpp - Integration Build Test
 *
 * Verifies that the test game application can successfully link
 * against the AIEngine library and access all required functionality.
 *
 * @version 1.0.0
 * @date 2026-03-04
 */

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../../../engine/deps/doctest/doctest.h"

// Test includes to verify linking
#include <AIEngine/AIEngine.hpp>
#include <memory>

TEST_SUITE("Engine Integration Tests") {

  TEST_CASE("Engine library can be included and compiled") {
    // This test verifies that all headers compile without error
    CHECK(true); // If we reach here, compilation succeeded

    // Test version information access
    const char *version = AIEngine::GetVersion();
    CHECK(version != nullptr);
    CHECK(std::string(version) == "1.0.0");

    // Test version compatibility
    CHECK(AIEngine::IsVersionCompatible(1, 0) == true);
    CHECK(AIEngine::IsVersionCompatible(0, 9) == false);
    CHECK(AIEngine::IsVersionCompatible(2, 0) == false);
  }

  TEST_CASE("EngineConfig can be created and validated") {
    AIEngine::EngineConfig config;

    // Test default configuration
    CHECK(config.Validate() == true);

    // Test configuration properties
    CHECK(config.windowWidth > 0);
    CHECK(config.windowHeight > 0);
    CHECK(config.targetFrameRate > 0.0);
    CHECK(!config.windowTitle.empty());

    // Test custom configuration
    config.windowTitle = "Test Window";
    config.windowWidth = 1920;
    config.windowHeight = 1080;
    config.targetFrameRate = 144.0;

    CHECK(config.Validate() == true);
    CHECK(config.windowTitle == "Test Window");
    CHECK(config.windowWidth == 1920);
    CHECK(config.windowHeight == 1080);
    CHECK(config.targetFrameRate == 144.0);
  }

  TEST_CASE("Engine can be constructed without crashing") {
    AIEngine::EngineConfig config;
    config.windowTitle = "Integration Test";

    // Engine construction should not throw
    CHECK_NOTHROW({
      AIEngine::Engine engine(config);

      // Test basic engine properties
      CHECK(engine.IsInitialized() == false);
      CHECK(engine.GetConfig().windowTitle == "Integration Test");
      CHECK(engine.GetDeltaTime() > 0.0 || engine.GetDeltaTime() == 0.0);
      CHECK(engine.GetUptime() >= 0.0);
      CHECK(engine.ShouldClose() == false);

      // Note: We don't call Initialize() in this test since it requires
      // graphics context which may not be available in test environment
    });
  }

  TEST_CASE("SceneGraph can be created and managed") {
    AIEngine::SceneGraph sceneGraph;

    // Test root node access
    auto *rootNode = sceneGraph.GetRootNode();
    CHECK(rootNode != nullptr);
    CHECK(rootNode->GetName() == "Root");
    CHECK(rootNode->IsActive() == true);
    CHECK(rootNode->GetComponentCount() == 0);
    CHECK(rootNode->GetChildCount() == 0);

    // Test node creation
    auto node = sceneGraph.CreateNode("TestNode");
    CHECK(node != nullptr);
    CHECK(node->GetName() == "TestNode");
    CHECK(node->GetId() > 0);

    // Test node hierarchy
    auto *addedNode = rootNode->AddChild(std::move(node));
    CHECK(addedNode != nullptr);
    CHECK(addedNode->GetParent() == rootNode);
    CHECK(rootNode->GetChildCount() == 1);

    // Test scene statistics
    CHECK(sceneGraph.GetNodeCount() >= 2); // Root + added node
  }

  TEST_CASE("Components can be created and managed") {
    AIEngine::SceneNode node("TestNode");

    // Test TransformComponent
    auto *transform = node.AddComponent<AIEngine::TransformComponent>();
    CHECK(transform != nullptr);
    CHECK(transform->GetOwner() == &node);
    CHECK(node.HasComponent<AIEngine::TransformComponent>() == true);
    CHECK(node.GetComponentCount() == 1);

    // Test component properties
    CHECK(transform->GetPosition() == glm::vec3(0.0f, 0.0f, 0.0f));
    CHECK(transform->GetScale() == glm::vec3(1.0f, 1.0f, 1.0f));

    // Test component modification
    transform->SetPosition(1.0f, 2.0f, 3.0f);
    CHECK(transform->GetPosition() == glm::vec3(1.0f, 2.0f, 3.0f));

    // Test RenderComponent
    auto *render = node.AddComponent<AIEngine::RenderComponent>();
    CHECK(render != nullptr);
    CHECK(render->GetOwner() == &node);
    CHECK(node.HasComponent<AIEngine::RenderComponent>() == true);
    CHECK(node.GetComponentCount() == 2);

    // Test render properties
    CHECK(render->IsVisible() == true);
    CHECK(render->GetRenderLayer() == 0);
    CHECK(render->HasMesh() == false);

    render->SetMeshId("test_mesh");
    CHECK(render->HasMesh() == true);
    CHECK(render->GetMeshId() == "test_mesh");
  }

  TEST_CASE("Math utilities are available") {
    using namespace AIEngine::Math;

    // Test constants
    CHECK(PI > 3.14f);
    CHECK(PI < 3.15f);
    CHECK(DEG_TO_RAD > 0.017f);
    CHECK(RAD_TO_DEG > 57.0f);

    // Test angle conversion
    float degrees = 90.0f;
    float radians = ToRadians(degrees);
    float backToDegrees = ToDegrees(radians);

    CHECK(radians > 1.57f);
    CHECK(radians < 1.58f);
    CHECK(abs(backToDegrees - degrees) < 0.001f);

    // Test vector operations
    glm::vec3 vec1(1.0f, 0.0f, 0.0f);
    glm::vec3 vec2(0.0f, 1.0f, 0.0f);

    CHECK(IsZero(glm::vec3(0.0f)) == true);
    CHECK(IsZero(vec1) == false);

    glm::vec3 normalized = SafeNormalize(vec1);
    CHECK(glm::length(normalized) > 0.99f);
    CHECK(glm::length(normalized) < 1.01f);

    // Test matrix creation
    glm::mat4 translation = CreateTranslation(glm::vec3(1.0f, 2.0f, 3.0f));
    glm::vec4 transformed = translation * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

    CHECK(abs(transformed.x - 1.0f) < 0.001f);
    CHECK(abs(transformed.y - 2.0f) < 0.001f);
    CHECK(abs(transformed.z - 3.0f) < 0.001f);
  }

  TEST_CASE("Component type system works correctly") {
    using namespace AIEngine;

    // Test type ID generation
    auto transformTypeId = TransformComponent::GetStaticTypeID();
    auto renderTypeId = RenderComponent::GetStaticTypeID();

    CHECK(transformTypeId != nullptr);
    CHECK(renderTypeId != nullptr);
    CHECK(transformTypeId != renderTypeId);

    // Test instance type matching
    SceneNode node("TestNode");
    auto *transform = node.AddComponent<TransformComponent>();

    CHECK(transform->GetTypeID() == transformTypeId);
  }

  TEST_CASE("Update and render cycles can be called") {
    AIEngine::SceneGraph sceneGraph;
    auto *rootNode = sceneGraph.GetRootNode();

    auto node = sceneGraph.CreateNode("TestNode");
    node->AddComponent<AIEngine::TransformComponent>();
    node->AddComponent<AIEngine::RenderComponent>();

    rootNode->AddChild(std::move(node));

    // Test update cycle
    CHECK_NOTHROW(sceneGraph.Update(0.016)); // 60 FPS delta time

    // Test render cycle (with null renderer - should not crash)
    CHECK_NOTHROW(sceneGraph.Render(nullptr));
  }
}

// Performance and stress tests
TEST_SUITE("Performance Tests") {

  TEST_CASE("Large scene management") {
    AIEngine::SceneGraph sceneGraph;
    auto *rootNode = sceneGraph.GetRootNode();

    // Create a reasonable number of nodes for testing
    const int nodeCount = 100;

    for (int i = 0; i < nodeCount; ++i) {
      auto node = sceneGraph.CreateNode("Node" + std::to_string(i));
      node->AddComponent<AIEngine::TransformComponent>();

      if (i % 2 == 0) {
        node->AddComponent<AIEngine::RenderComponent>();
      }

      rootNode->AddChild(std::move(node));
    }

    CHECK(sceneGraph.GetNodeCount() >= nodeCount + 1); // +1 for root

    // Test that update doesn't take too long
    auto start = std::chrono::high_resolution_clock::now();
    sceneGraph.Update(0.016);
    auto end = std::chrono::high_resolution_clock::now();

    auto duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    CHECK(duration.count() < 100); // Should complete in under 100ms
  }

  TEST_CASE("Component lookup performance") {
    AIEngine::SceneNode node("TestNode");

    // Add many components (mix of different types)
    for (int i = 0; i < 10; ++i) {
      node.AddComponent<AIEngine::TransformComponent>();
      // Note: Adding same type multiple times returns same instance
    }

    // Test lookup speed
    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < 1000; ++i) {
      auto *transform = node.GetComponent<AIEngine::TransformComponent>();
      (void)transform; // Suppress unused warning
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    CHECK(duration.count() < 10000); // Should complete in under 10ms
  }
}

// Educational note: Integration tests verify:
// - Successful library linking and symbol resolution
// - Correct API usage patterns for engine consumption
// - Performance characteristics of key operations
// - Error handling and edge cases in integration scenarios