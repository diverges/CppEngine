/**
 * TransformComponent.cpp - Transform Component Implementation
 *
 * Implementation of 3D transformation functionality including
 * position, rotation, scale, and matrix calculations.
 *
 * @version 1.0.0
 * @date 2026-03-04
 */

#include "../../include/AIEngine/components/TransformComponent.hpp"
#include "../../include/AIEngine/scene/SceneNode.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/string_cast.hpp>
#include <sstream>

namespace AIEngine
{

    TransformComponent::TransformComponent()
        : m_position(0.0f, 0.0f, 0.0f), m_rotation(1.0f, 0.0f, 0.0f, 0.0f) // Identity quaternion
          ,
          m_scale(1.0f, 1.0f, 1.0f), m_localMatrix(1.0f) // Identity matrix
          ,
          m_isDirty(true)
    {
    }

    TransformComponent::TransformComponent(const glm::vec3 &position,
                                           const glm::quat &rotation,
                                           const glm::vec3 &scale)
        : m_position(position), m_rotation(rotation), m_scale(scale), m_localMatrix(1.0f), m_isDirty(true)
    {
    }

    // --- Position Methods ---

    void TransformComponent::SetPosition(const glm::vec3 &position)
    {
        if (m_position != position)
        {
            m_position = position;
            MarkDirty();
        }
    }

    void TransformComponent::SetPosition(float x, float y, float z)
    {
        SetPosition(glm::vec3(x, y, z));
    }

    void TransformComponent::Translate(const glm::vec3 &offset)
    {
        SetPosition(m_position + offset);
    }

    void TransformComponent::Translate(float x, float y, float z)
    {
        Translate(glm::vec3(x, y, z));
    }

    // --- Rotation Methods ---

    void TransformComponent::SetRotation(const glm::quat &rotation)
    {
        m_rotation = glm::normalize(rotation);
        MarkDirty();
    }

    void TransformComponent::SetRotationEuler(float pitch, float yaw, float roll)
    {
        // Convert degrees to radians and create quaternion
        glm::vec3 radians = glm::radians(glm::vec3(pitch, yaw, roll));
        m_rotation = glm::quat(radians);
        MarkDirty();
    }

    void TransformComponent::LookAt(const glm::vec3 &target, const glm::vec3 &up)
    {
        glm::vec3 forward = glm::normalize(target - m_position);
        glm::vec3 right = glm::normalize(glm::cross(forward, up));
        glm::vec3 newUp = glm::cross(right, forward);

        // Create rotation matrix and convert to quaternion
        glm::mat3 rotationMatrix;
        rotationMatrix[0] = right;
        rotationMatrix[1] = newUp;
        rotationMatrix[2] = -forward; // OpenGL uses -Z as forward

        m_rotation = glm::normalize(glm::quat_cast(rotationMatrix));
        MarkDirty();
    }

    glm::vec3 TransformComponent::GetEulerAngles() const
    {
        // Convert quaternion to Euler angles and return in degrees
        glm::vec3 radians = glm::eulerAngles(m_rotation);
        return glm::degrees(radians);
    }

    void TransformComponent::Rotate(const glm::quat &rotation)
    {
        m_rotation = glm::normalize(m_rotation * rotation);
        MarkDirty();
    }

    void TransformComponent::RotateAround(const glm::vec3 &axis, float angleDegrees)
    {
        float angleRadians = glm::radians(angleDegrees);
        glm::quat rotation = glm::angleAxis(angleRadians, glm::normalize(axis));
        Rotate(rotation);
    }

    // --- Scale Methods ---

    void TransformComponent::SetScale(float uniformScale)
    {
        SetScale(glm::vec3(uniformScale));
    }

    void TransformComponent::SetScale(const glm::vec3 &scale)
    {
        if (m_scale != scale)
        {
            m_scale = scale;
            MarkDirty();
        }
    }

    void TransformComponent::SetScale(float x, float y, float z)
    {
        SetScale(glm::vec3(x, y, z));
    }

    void TransformComponent::Scale(const glm::vec3 &scaleMultiplier)
    {
        SetScale(m_scale * scaleMultiplier);
    }

    // --- Matrix Methods ---

    const glm::mat4 &TransformComponent::GetLocalMatrix() const
    {
        UpdateMatrixIfDirty();
        return m_localMatrix;
    }

    glm::mat4 TransformComponent::GetWorldMatrix() const
    {
        glm::mat4 worldMatrix = GetLocalMatrix();

        // If this component belongs to a node, check for parent transforms
        if (GetOwner() && GetOwner()->GetParent())
        {
            SceneNode *parent = GetOwner()->GetParent();

            // Look for parent transform component
            if (auto *parentTransform = parent->GetComponent<TransformComponent>())
            {
                // Multiply by parent's world matrix (parent transforms apply first)
                worldMatrix = parentTransform->GetWorldMatrix() * worldMatrix;
            }
        }

        return worldMatrix;
    }

    glm::mat4 TransformComponent::GetInverseWorldMatrix() const
    {
        return glm::inverse(GetWorldMatrix());
    }

    // --- Direction Vectors ---

    glm::vec3 TransformComponent::GetForward() const
    {
        // Forward is typically -Z in OpenGL coordinate system
        glm::vec4 forward = glm::mat4_cast(m_rotation) * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
        return glm::normalize(glm::vec3(forward));
    }

    glm::vec3 TransformComponent::GetRight() const
    {
        // Right is typically +X
        glm::vec4 right = glm::mat4_cast(m_rotation) * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
        return glm::normalize(glm::vec3(right));
    }

    glm::vec3 TransformComponent::GetUp() const
    {
        // Up is typically +Y
        glm::vec4 up = glm::mat4_cast(m_rotation) * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
        return glm::normalize(glm::vec3(up));
    }

    // --- Component Interface ---

    void TransformComponent::OnAttach(SceneNode *owner)
    {
        Component::OnAttach(owner);
        // No special attachment logic needed for transforms
    }

    void TransformComponent::OnUpdate(double deltaTime)
    {
        (void)deltaTime; // Suppress unused parameter warning

        // Transform components are typically passive - they don't need
        // to update themselves each frame unless there's animation logic

        // Future enhancements could include:
        // - Animation interpolation
        // - Physics integration
        // - Constraint solving
    }

    // --- Utility Methods ---

    void TransformComponent::Reset()
    {
        m_position = glm::vec3(0.0f, 0.0f, 0.0f);
        m_rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f); // Identity
        m_scale = glm::vec3(1.0f, 1.0f, 1.0f);
        MarkDirty();
    }

    void TransformComponent::CopyFrom(const TransformComponent &other)
    {
        m_position = other.m_position;
        m_rotation = other.m_rotation;
        m_scale = other.m_scale;
        MarkDirty();
    }

    std::string TransformComponent::ToString() const
    {
        std::stringstream ss;
        ss << "Transform(\n";
        ss << "  Position: " << glm::to_string(m_position) << "\n";
        ss << "  Rotation: " << glm::to_string(m_rotation) << "\n";
        ss << "  Scale: " << glm::to_string(m_scale) << "\n";
        ss << "  Euler: " << glm::to_string(GetEulerAngles()) << "\n";
        ss << ")";
        return ss.str();
    }

    // --- Private Methods ---

    void TransformComponent::UpdateMatrixIfDirty() const
    {
        if (m_isDirty)
        {
            m_localMatrix = CalculateMatrix();
            m_isDirty = false;
        }
    }

    glm::mat4 TransformComponent::CalculateMatrix() const
    {
        // Standard TRS (Translate * Rotate * Scale) matrix calculation

        // Scale matrix
        glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), m_scale);

        // Rotation matrix from quaternion
        glm::mat4 rotationMatrix = glm::mat4_cast(m_rotation);

        // Translation matrix
        glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), m_position);

        // Combine: Translation * Rotation * Scale
        // (Order matters: scale happens first in object space, then rotate, then translate)
        return translationMatrix * rotationMatrix * scaleMatrix;
    }

} // namespace AIEngine