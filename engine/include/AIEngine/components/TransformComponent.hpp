/**
 * TransformComponent.hpp - 3D Transform Component Declaration
 * 
 * Provides position, rotation, and scale transformation for scene nodes.
 * Integrates with GLM mathematics library for efficient matrix operations.
 * 
 * @version 1.0.0
 * @date 2026-03-04
 */

#pragma once

#include "../core/Component.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/quaternion.hpp>

namespace AIEngine {

    /**
     * @brief 3D Transform Component
     * 
     * Handles position, rotation, and scale transformations for scene objects.
     * Provides matrix calculations and coordinate space transformations.
     * 
     * Features:
     * - Position (translation) in 3D space
     * - Rotation using quaternions for smooth interpolation
     * - Scale (non-uniform scaling supported)
     * - Cached transform matrices for performance
     * - Local and world coordinate transformations
     * - Dirty flag optimization for matrix recalculation
     * 
     * @example Basic Transform Usage
     * @code
     * // Add transform to a scene node
     * auto* transform = node->AddComponent<TransformComponent>();
     * 
     * // Set position
     * transform->SetPosition(10.0f, 5.0f, -3.0f);
     * 
     * // Set rotation (45 degrees around Y axis)
     * transform->SetRotation(glm::rotate(glm::quat(1,0,0,0), glm::radians(45.0f), glm::vec3(0,1,0)));
     * 
     * // Set scale
     * transform->SetScale(2.0f, 1.0f, 2.0f);
     * 
     * // Get final transform matrix for rendering
     * glm::mat4 matrix = transform->GetWorldMatrix();
     * @endcode
     */
    class TransformComponent : public Component<TransformComponent> {
    public:
        /**
         * @brief Construct transform with identity values
         */
        TransformComponent();

        /**
         * @brief Construct transform with initial values
         * @param position Initial position
         * @param rotation Initial rotation quaternion
         * @param scale Initial scale
         */
        TransformComponent(const glm::vec3& position, 
                          const glm::quat& rotation = glm::quat(1,0,0,0), 
                          const glm::vec3& scale = glm::vec3(1.0f));

        /**
         * @brief Virtual destructor
         */
        virtual ~TransformComponent() = default;

        // --- Position Methods ---

        /**
         * @brief Set position in world coordinates
         * @param position New position vector
         */
        void SetPosition(const glm::vec3& position);

        /**
         * @brief Set position using individual components
         * @param x X coordinate
         * @param y Y coordinate  
         * @param z Z coordinate
         */
        void SetPosition(float x, float y, float z);

        /**
         * @brief Get current position
         * @return Position vector
         */
        const glm::vec3& GetPosition() const { return m_position; }

        /**
         * @brief Translate by offset (additive)
         * @param offset Translation vector to add
         */
        void Translate(const glm::vec3& offset);

        /**
         * @brief Translate using individual components
         * @param x X offset
         * @param y Y offset
         * @param z Z offset
         */
        void Translate(float x, float y, float z);

        // --- Rotation Methods ---

        /**
         * @brief Set rotation using quaternion
         * @param rotation New rotation quaternion
         */
        void SetRotation(const glm::quat& rotation);

        /**
         * @brief Set rotation using Euler angles (degrees)
         * @param pitch Rotation around X axis (degrees)
         * @param yaw Rotation around Y axis (degrees)  
         * @param roll Rotation around Z axis (degrees)
         */
        void SetRotationEuler(float pitch, float yaw, float roll);

        /**
         * @brief Set rotation to look at target position
         * @param target Position to look at
         * @param up Up vector (default: world up)
         */
        void LookAt(const glm::vec3& target, const glm::vec3& up = glm::vec3(0, 1, 0));

        /**
         * @brief Get current rotation quaternion
         * @return Rotation quaternion
         */
        const glm::quat& GetRotation() const { return m_rotation; }

        /**
         * @brief Get rotation as Euler angles (degrees)
         * @return Vector containing (pitch, yaw, roll) in degrees
         */
        glm::vec3 GetEulerAngles() const;

        /**
         * @brief Rotate by additional rotation (multiplicative)
         * @param rotation Additional rotation quaternion
         */
        void Rotate(const glm::quat& rotation);

        /**
         * @brief Rotate around axis by angle
         * @param axis Normalized rotation axis
         * @param angleDegrees Rotation angle in degrees
         */
        void RotateAround(const glm::vec3& axis, float angleDegrees);

        // --- Scale Methods ---

        /**
         * @brief Set uniform scale
         * @param uniformScale Scale factor for all axes
         */
        void SetScale(float uniformScale);

        /**
         * @brief Set non-uniform scale
         * @param scale Scale vector
         */
        void SetScale(const glm::vec3& scale);

        /**
         * @brief Set scale using individual components
         * @param x X scale factor
         * @param y Y scale factor
         * @param z Z scale factor
         */
        void SetScale(float x, float y, float z);

        /**
         * @brief Get current scale
         * @return Scale vector
         */
        const glm::vec3& GetScale() const { return m_scale; }

        /**
         * @brief Apply additional scale (multiplicative)
         * @param scaleMultiplier Scale factors to multiply
         */
        void Scale(const glm::vec3& scaleMultiplier);

        // --- Matrix Methods ---

        /**
         * @brief Get local transform matrix
         * @return 4x4 transformation matrix for local space
         */
        const glm::mat4& GetLocalMatrix() const;

        /**
         * @brief Get world transform matrix
         * @return 4x4 transformation matrix for world space
         * 
         * Accounts for parent transformations if this node has a parent
         * in the scene hierarchy.
         */
        glm::mat4 GetWorldMatrix() const;

        /**
         * @brief Get inverse world transform matrix
         * @return Inverse of world transformation matrix
         * 
         * Useful for converting from world coordinates to local coordinates.
         */
        glm::mat4 GetInverseWorldMatrix() const;

        // --- Direction Vectors ---

        /**
         * @brief Get forward direction vector
         * @return Forward direction in local space (typically -Z)
         */
        glm::vec3 GetForward() const;

        /**
         * @brief Get right direction vector  
         * @return Right direction in local space (typically +X)
         */
        glm::vec3 GetRight() const;

        /**
         * @brief Get up direction vector
         * @return Up direction in local space (typically +Y)
         */
        glm::vec3 GetUp() const;

        // --- Component Interface ---

        /**
         * @brief Called when component is attached to a node
         * @param owner The scene node this component belongs to
         */
        void OnAttach(SceneNode* owner) override;

        /**
         * @brief Called each frame for component updates
         * @param deltaTime Time elapsed since last frame
         */
        void OnUpdate(double deltaTime) override;

        // --- Utility Methods ---

        /**
         * @brief Reset transform to identity
         * 
         * Sets position to (0,0,0), rotation to identity, scale to (1,1,1).
         */
        void Reset();

        /**
         * @brief Copy transform values from another transform
         * @param other Transform component to copy from
         */
        void CopyFrom(const TransformComponent& other);

        /**
         * @brief Check if transform has changed since last matrix calculation
         * @return true if transform needs matrix recalculation
         */
        bool IsDirty() const { return m_isDirty; }

        /**
         * @brief Get string representation for debugging
         * @return String containing position, rotation, and scale values
         */
        std::string ToString() const;

    private:
        // Transform data
        glm::vec3 m_position{0.0f, 0.0f, 0.0f};    ///< Position in world space
        glm::quat m_rotation{1.0f, 0.0f, 0.0f, 0.0f}; ///< Rotation quaternion  
        glm::vec3 m_scale{1.0f, 1.0f, 1.0f};           ///< Scale factors

        // Cached matrices for performance
        mutable glm::mat4 m_localMatrix{1.0f};     ///< Cached local transform matrix
        mutable bool m_isDirty = true;             ///< Flag indicating matrix needs recalculation

        /**
         * @brief Mark transform as dirty (needs matrix recalculation)
         */
        void MarkDirty() { m_isDirty = true; }

        /**
         * @brief Recalculate local transform matrix if dirty
         */
        void UpdateMatrixIfDirty() const;

        /**
         * @brief Calculate transform matrix from position, rotation, scale
         * @return Computed transformation matrix
         */
        glm::mat4 CalculateMatrix() const;
    };

} // namespace AIEngine