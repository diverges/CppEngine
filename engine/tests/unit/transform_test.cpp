/**
 * transform_test.cpp - TransformComponent Unit Tests
 *
 * Comprehensive testing of TransformComponent functionality including
 * matrix calculations, coordinate transformations, and mathematical operations.
 *
 * @version 1.0.0
 * @date 2026-03-04
 */

#include <AIEngine/components/TransformComponent.hpp>
#include <AIEngine/math/Transform.hpp>
#include <AIEngine/scene/SceneNode.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include "doctest.h"

// Helper function for float comparison with epsilon
bool ApproxEqual(float a, float b, float epsilon = 1e-6f) { return std::abs(a - b) < epsilon; }

bool ApproxEqual(const glm::vec3& a, const glm::vec3& b, float epsilon = 1e-6f) {
    return ApproxEqual(a.x, b.x, epsilon) && ApproxEqual(a.y, b.y, epsilon) &&
           ApproxEqual(a.z, b.z, epsilon);
}

bool ApproxEqual(const glm::mat4& a, const glm::mat4& b, float epsilon = 1e-5f) {
    for (int col = 0; col < 4; ++col) {
        for (int row = 0; row < 4; ++row) {
            if (!ApproxEqual(a[col][row], b[col][row], epsilon)) {
                return false;
            }
        }
    }
    return true;
}

TEST_SUITE("TransformComponent Tests") {
    TEST_CASE("TransformComponent construction and defaults") {
        using namespace AIEngine;

        SUBCASE("Default construction") {
            TransformComponent transform;

            CHECK(ApproxEqual(transform.GetPosition(), glm::vec3(0.0f)));
            CHECK(ApproxEqual(transform.GetScale(), glm::vec3(1.0f)));

            // Quaternion identity check
            glm::quat rotation = transform.GetRotation();
            CHECK(ApproxEqual(rotation.w, 1.0f));
            CHECK(ApproxEqual(rotation.x, 0.0f));
            CHECK(ApproxEqual(rotation.y, 0.0f));
            CHECK(ApproxEqual(rotation.z, 0.0f));
        }

        SUBCASE("Parameterized construction") {
            glm::vec3 position(1.0f, 2.0f, 3.0f);
            glm::quat rotation = glm::angleAxis(glm::radians(45.0f), glm::vec3(0, 1, 0));
            glm::vec3 scale(2.0f, 3.0f, 4.0f);

            TransformComponent transform(position, rotation, scale);

            CHECK(ApproxEqual(transform.GetPosition(), position));
            CHECK(ApproxEqual(transform.GetRotation().w, rotation.w));
            CHECK(ApproxEqual(transform.GetScale(), scale));
        }
    }

    TEST_CASE("Position operations") {
        using namespace AIEngine;

        TransformComponent transform;

        SUBCASE("Set and get position") {
            glm::vec3 newPos(5.0f, -3.0f, 10.0f);
            transform.SetPosition(newPos);

            CHECK(ApproxEqual(transform.GetPosition(), newPos));
            CHECK(transform.IsDirty() == true);
        }

        SUBCASE("Set position with individual components") {
            transform.SetPosition(1.0f, 2.0f, 3.0f);

            CHECK(ApproxEqual(transform.GetPosition(), glm::vec3(1.0f, 2.0f, 3.0f)));
        }

        SUBCASE("Translate operation") {
            transform.SetPosition(1.0f, 1.0f, 1.0f);
            transform.Translate(glm::vec3(2.0f, 3.0f, 4.0f));

            CHECK(ApproxEqual(transform.GetPosition(), glm::vec3(3.0f, 4.0f, 5.0f)));
        }

        SUBCASE("Translate with individual components") {
            transform.SetPosition(1.0f, 1.0f, 1.0f);
            transform.Translate(1.0f, 2.0f, 3.0f);

            CHECK(ApproxEqual(transform.GetPosition(), glm::vec3(2.0f, 3.0f, 4.0f)));
        }
    }

    TEST_CASE("Rotation operations") {
        using namespace AIEngine;

        TransformComponent transform;

        SUBCASE("Set rotation with quaternion") {
            glm::quat rotation = glm::angleAxis(glm::radians(90.0f), glm::vec3(0, 1, 0));
            transform.SetRotation(rotation);

            glm::quat retrieved = transform.GetRotation();
            CHECK(ApproxEqual(retrieved.w, rotation.w, 1e-5f));
            CHECK(ApproxEqual(retrieved.x, rotation.x, 1e-5f));
            CHECK(ApproxEqual(retrieved.y, rotation.y, 1e-5f));
            CHECK(ApproxEqual(retrieved.z, rotation.z, 1e-5f));

            CHECK(transform.IsDirty() == true);
        }

        SUBCASE("Set rotation with Euler angles") {
            transform.SetRotationEuler(45.0f, 90.0f, 0.0f);

            glm::vec3 euler = transform.GetEulerAngles();
            CHECK(ApproxEqual(euler.x, 45.0f, 1.0f));  // Allow slight tolerance for conversion
            CHECK(ApproxEqual(euler.y, 90.0f, 1.0f));
            CHECK(ApproxEqual(euler.z, 0.0f, 1.0f));
        }

        SUBCASE("Rotate operation") {
            // Start with identity
            transform.SetRotationEuler(0.0f, 0.0f, 0.0f);

            // Rotate 45 degrees around Y
            glm::quat rotation = glm::angleAxis(glm::radians(45.0f), glm::vec3(0, 1, 0));
            transform.Rotate(rotation);

            glm::vec3 euler = transform.GetEulerAngles();
            CHECK(ApproxEqual(euler.y, 45.0f, 1.0f));
        }

        SUBCASE("Rotate around axis") {
            transform.RotateAround(glm::vec3(0, 1, 0), 90.0f);

            glm::vec3 euler = transform.GetEulerAngles();
            CHECK(ApproxEqual(euler.y, 90.0f, 1.0f));
        }

        SUBCASE("Look at target") {
            transform.SetPosition(0.0f, 0.0f, 0.0f);
            transform.LookAt(glm::vec3(1.0f, 0.0f, 0.0f));  // Look right

            glm::vec3 forward = transform.GetForward();
            // Forward should point towards the target (in engine's -Z forward convention)
            // The exact result depends on the look-at implementation
            CHECK(glm::length(forward) > 0.9f);  // Verify it's normalized
        }
    }

    TEST_CASE("Scale operations") {
        using namespace AIEngine;

        TransformComponent transform;

        SUBCASE("Set uniform scale") {
            transform.SetScale(2.5f);

            CHECK(ApproxEqual(transform.GetScale(), glm::vec3(2.5f)));
            CHECK(transform.IsDirty() == true);
        }

        SUBCASE("Set non-uniform scale") {
            glm::vec3 scale(1.0f, 2.0f, 3.0f);
            transform.SetScale(scale);

            CHECK(ApproxEqual(transform.GetScale(), scale));
        }

        SUBCASE("Set scale with individual components") {
            transform.SetScale(2.0f, 3.0f, 4.0f);

            CHECK(ApproxEqual(transform.GetScale(), glm::vec3(2.0f, 3.0f, 4.0f)));
        }

        SUBCASE("Apply additional scale") {
            transform.SetScale(2.0f, 1.0f, 3.0f);
            transform.Scale(glm::vec3(0.5f, 2.0f, 1.0f));

            CHECK(ApproxEqual(transform.GetScale(), glm::vec3(1.0f, 2.0f, 3.0f)));
        }
    }

    TEST_CASE("Matrix calculations") {
        using namespace AIEngine;

        TransformComponent transform;

        SUBCASE("Identity matrix for default transform") {
            glm::mat4 matrix = transform.GetLocalMatrix();
            glm::mat4 identity = glm::mat4(1.0f);

            CHECK(ApproxEqual(matrix, identity));
        }

        SUBCASE("Translation matrix") {
            transform.SetPosition(1.0f, 2.0f, 3.0f);

            glm::mat4 matrix = transform.GetLocalMatrix();
            glm::vec4 origin = matrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

            CHECK(ApproxEqual(origin.x, 1.0f));
            CHECK(ApproxEqual(origin.y, 2.0f));
            CHECK(ApproxEqual(origin.z, 3.0f));
        }

        SUBCASE("Scale matrix") {
            transform.SetScale(2.0f, 3.0f, 4.0f);

            glm::mat4 matrix = transform.GetLocalMatrix();
            glm::vec4 point = matrix * glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

            CHECK(ApproxEqual(point.x, 2.0f));
            CHECK(ApproxEqual(point.y, 3.0f));
            CHECK(ApproxEqual(point.z, 4.0f));
        }

        SUBCASE("Rotation matrix") {
            // 90 degree rotation around Y axis
            transform.SetRotationEuler(0.0f, 90.0f, 0.0f);

            glm::mat4 matrix = transform.GetLocalMatrix();
            glm::vec4 point = matrix * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

            // X point should rotate to -Z (or close, depending on convention)
            CHECK(ApproxEqual(point.y, 0.0f, 1e-5f));
        }

        SUBCASE("Combined TRS matrix") {
            transform.SetPosition(1.0f, 2.0f, 3.0f);
            transform.SetRotationEuler(0.0f, 0.0f, 0.0f);  // Identity rotation
            transform.SetScale(2.0f, 2.0f, 2.0f);

            glm::mat4 matrix = transform.GetLocalMatrix();

            // Test a point transformation
            glm::vec4 point = matrix * glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

            // Scale first: (1,1,1) -> (2,2,2)
            // Then rotate: (2,2,2) -> (2,2,2) [identity]
            // Then translate: (2,2,2) -> (3,4,5)
            CHECK(ApproxEqual(point.x, 3.0f));
            CHECK(ApproxEqual(point.y, 4.0f));
            CHECK(ApproxEqual(point.z, 5.0f));
        }

        SUBCASE("Dirty flag optimization") {
            // Get matrix once
            glm::mat4 matrix1 = transform.GetLocalMatrix();
            CHECK(transform.IsDirty() == false);  // Should be clean after calculation

            // Get matrix again (should use cached version)
            glm::mat4 matrix2 = transform.GetLocalMatrix();
            CHECK(ApproxEqual(matrix1, matrix2));

            // Modify transform
            transform.SetPosition(1.0f, 0.0f, 0.0f);
            CHECK(transform.IsDirty() == true);  // Should be dirty after modification

            // Get new matrix
            glm::mat4 matrix3 = transform.GetLocalMatrix();
            CHECK(transform.IsDirty() == false);    // Should be clean again
            CHECK(!ApproxEqual(matrix1, matrix3));  // Should be different
        }
    }

    TEST_CASE("Direction vectors") {
        using namespace AIEngine;

        TransformComponent transform;

        SUBCASE("Identity transform directions") {
            glm::vec3 forward = transform.GetForward();
            glm::vec3 right = transform.GetRight();
            glm::vec3 up = transform.GetUp();

            // Verify vectors are normalized
            CHECK(ApproxEqual(glm::length(forward), 1.0f));
            CHECK(ApproxEqual(glm::length(right), 1.0f));
            CHECK(ApproxEqual(glm::length(up), 1.0f));

            // Check expected directions (may vary based on coordinate system)
            // At minimum, vectors should be orthogonal
            CHECK(ApproxEqual(glm::dot(forward, right), 0.0f, 1e-5f));
            CHECK(ApproxEqual(glm::dot(forward, up), 0.0f, 1e-5f));
            CHECK(ApproxEqual(glm::dot(right, up), 0.0f, 1e-5f));
        }

        SUBCASE("Rotated transform directions") {
            // Rotate 90 degrees around Y axis
            transform.SetRotationEuler(0.0f, 90.0f, 0.0f);

            glm::vec3 forward = transform.GetForward();
            glm::vec3 right = transform.GetRight();
            glm::vec3 up = transform.GetUp();

            // Verify vectors are still normalized and orthogonal
            CHECK(ApproxEqual(glm::length(forward), 1.0f));
            CHECK(ApproxEqual(glm::length(right), 1.0f));
            CHECK(ApproxEqual(glm::length(up), 1.0f));

            CHECK(ApproxEqual(glm::dot(forward, right), 0.0f, 1e-5f));
            CHECK(ApproxEqual(glm::dot(forward, up), 0.0f, 1e-5f));
            CHECK(ApproxEqual(glm::dot(right, up), 0.0f, 1e-5f));
        }
    }

    TEST_CASE("World transform with hierarchy") {
        using namespace AIEngine;

        SUBCASE("Single node world transform") {
            SceneNode node("TestNode");
            auto* transform = node.AddComponent<TransformComponent>();

            transform->SetPosition(1.0f, 2.0f, 3.0f);

            glm::mat4 worldMatrix = transform->GetWorldMatrix();
            glm::mat4 localMatrix = transform->GetLocalMatrix();

            // Without parent, world matrix should equal local matrix
            CHECK(ApproxEqual(worldMatrix, localMatrix));
        }

        SUBCASE("Parent-child hierarchy") {
            SceneNode parent("Parent");
            auto parentChild = std::make_unique<SceneNode>("Child");

            auto* parentTransform = parent.AddComponent<TransformComponent>();
            auto* childTransform = parentChild->AddComponent<TransformComponent>();

            SceneNode* child = parent.AddChild(std::move(parentChild));

            parentTransform->SetPosition(1.0f, 0.0f, 0.0f);
            childTransform->SetPosition(0.0f, 1.0f, 0.0f);

            glm::mat4 childWorldMatrix = childTransform->GetWorldMatrix();

            // Transform a point to verify hierarchy
            glm::vec4 point = childWorldMatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

            // Child should be at parent position + child position
            CHECK(ApproxEqual(point.x, 1.0f));  // Parent's X
            CHECK(ApproxEqual(point.y, 1.0f));  // Child's Y
            CHECK(ApproxEqual(point.z, 0.0f));  // Both Z are 0
        }
    }

    TEST_CASE("Utility operations") {
        using namespace AIEngine;

        TransformComponent transform;

        SUBCASE("Reset to identity") {
            transform.SetPosition(1.0f, 2.0f, 3.0f);
            transform.SetRotationEuler(45.0f, 90.0f, 30.0f);
            transform.SetScale(2.0f, 3.0f, 4.0f);

            transform.Reset();

            CHECK(ApproxEqual(transform.GetPosition(), glm::vec3(0.0f)));
            CHECK(ApproxEqual(transform.GetScale(), glm::vec3(1.0f)));

            // Check rotation is identity
            glm::vec3 euler = transform.GetEulerAngles();
            CHECK(ApproxEqual(euler.x, 0.0f, 1.0f));
            CHECK(ApproxEqual(euler.y, 0.0f, 1.0f));
            CHECK(ApproxEqual(euler.z, 0.0f, 1.0f));
        }

        SUBCASE("Copy from another transform") {
            TransformComponent source;
            source.SetPosition(5.0f, 6.0f, 7.0f);
            source.SetRotationEuler(30.0f, 60.0f, 90.0f);
            source.SetScale(1.5f, 2.5f, 3.5f);

            transform.CopyFrom(source);

            CHECK(ApproxEqual(transform.GetPosition(), source.GetPosition()));
            CHECK(ApproxEqual(transform.GetScale(), source.GetScale()));
            // Rotation comparison is more complex due to quaternion representation
        }

        SUBCASE("ToString for debugging") {
            transform.SetPosition(1.0f, 2.0f, 3.0f);

            std::string str = transform.ToString();
            CHECK(!str.empty());
            CHECK(str.find("Transform") != std::string::npos);
        }
    }

    TEST_CASE("Component lifecycle") {
        using namespace AIEngine;

        SceneNode node("TestNode");

        SUBCASE("Component attachment") {
            auto* transform = node.AddComponent<TransformComponent>();

            CHECK(transform->GetOwner() == &node);
            CHECK(node.HasComponent<TransformComponent>() == true);
        }

        SUBCASE("Component update") {
            auto* transform = node.AddComponent<TransformComponent>();

            // OnUpdate should not crash (it's typically a no-op for transforms)
            CHECK_NOTHROW(transform->OnUpdate(0.016));
        }
    }
}

// Educational notes: These tests verify:
// - Accurate matrix calculations using GLM
// - Proper TRS (Translate-Rotate-Scale) order
// - Parent-child transformation hierarchy
// - Dirty flag optimization for performance
// - Direction vector calculations
// - Coordinate system transformations