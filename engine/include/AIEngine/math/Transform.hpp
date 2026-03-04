/**
 * Transform.hpp - Mathematics Transform Utilities
 *
 * Utility functions and helpers for 3D transformations, matrix operations,
 * coordinate system conversions, and common mathematical calculations
 * used throughout the engine.
 *
 * @version 1.0.0
 * @date 2026-03-04
 */

#pragma once

#include <cmath>
#include <algorithm>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>

namespace AIEngine
{
    namespace Math
    {

        // --- Constants ---

        const float PI = 3.14159265359f;
        const float TWO_PI = 6.28318530718f;
        const float HALF_PI = 1.57079632679f;
        const float DEG_TO_RAD = 0.01745329252f;
        const float RAD_TO_DEG = 57.2957795131f;
        const float EPSILON = 1e-6f;

        // Standard direction vectors
        const glm::vec3 FORWARD = glm::vec3(0.0f, 0.0f, -1.0f); // -Z (OpenGL convention)
        const glm::vec3 BACK = glm::vec3(0.0f, 0.0f, 1.0f);     // +Z
        const glm::vec3 RIGHT = glm::vec3(1.0f, 0.0f, 0.0f);    // +X
        const glm::vec3 LEFT = glm::vec3(-1.0f, 0.0f, 0.0f);    // -X
        const glm::vec3 UP = glm::vec3(0.0f, 1.0f, 0.0f);       // +Y
        const glm::vec3 DOWN = glm::vec3(0.0f, -1.0f, 0.0f);    // -Y

        // --- Angle Conversions ---

        /**
         * @brief Convert degrees to radians
         * @param degrees Angle in degrees
         * @return Angle in radians
         */
        inline float ToRadians(float degrees)
        {
            return degrees * DEG_TO_RAD;
        }

        /**
         * @brief Convert radians to degrees
         * @param radians Angle in radians
         * @return Angle in degrees
         */
        inline float ToDegrees(float radians)
        {
            return radians * RAD_TO_DEG;
        }

        /**
         * @brief Convert degrees vector to radians
         * @param degrees Vector of angles in degrees
         * @return Vector of angles in radians
         */
        inline glm::vec3 ToRadians(const glm::vec3 &degrees)
        {
            return degrees * DEG_TO_RAD;
        }

        /**
         * @brief Convert radians vector to degrees
         * @param radians Vector of angles in radians
         * @return Vector of angles in degrees
         */
        inline glm::vec3 ToDegrees(const glm::vec3 &radians)
        {
            return radians * RAD_TO_DEG;
        }

        // --- Matrix Creation ---

        /**
         * @brief Create translation matrix
         * @param position Translation vector
         * @return 4x4 translation matrix
         */
        inline glm::mat4 CreateTranslation(const glm::vec3 &position)
        {
            return glm::translate(glm::mat4(1.0f), position);
        }

        /**
         * @brief Create rotation matrix from Euler angles
         * @param pitch Rotation around X axis (degrees)
         * @param yaw Rotation around Y axis (degrees)
         * @param roll Rotation around Z axis (degrees)
         * @return 4x4 rotation matrix
         */
        inline glm::mat4 CreateRotation(float pitch, float yaw, float roll)
        {
            glm::vec3 radians = ToRadians(glm::vec3(pitch, yaw, roll));
            return glm::eulerAngleYXZ(radians.y, radians.x, radians.z);
        }

        /**
         * @brief Create rotation matrix from quaternion
         * @param rotation Rotation quaternion
         * @return 4x4 rotation matrix
         */
        inline glm::mat4 CreateRotation(const glm::quat &rotation)
        {
            return glm::mat4_cast(rotation);
        }

        /**
         * @brief Create scale matrix
         * @param scale Scale vector
         * @return 4x4 scale matrix
         */
        inline glm::mat4 CreateScale(const glm::vec3 &scale)
        {
            return glm::scale(glm::mat4(1.0f), scale);
        }

        /**
         * @brief Create uniform scale matrix
         * @param uniformScale Scale factor for all axes
         * @return 4x4 scale matrix
         */
        inline glm::mat4 CreateScale(float uniformScale)
        {
            return CreateScale(glm::vec3(uniformScale));
        }

        /**
         * @brief Create complete TRS (Translate-Rotate-Scale) matrix
         * @param position Translation vector
         * @param rotation Rotation quaternion
         * @param scale Scale vector
         * @return Combined 4x4 transformation matrix
         */
        inline glm::mat4 CreateTRS(const glm::vec3 &position,
                                   const glm::quat &rotation,
                                   const glm::vec3 &scale)
        {
            return CreateTranslation(position) * CreateRotation(rotation) * CreateScale(scale);
        }

        // --- Quaternion Utilities ---

        /**
         * @brief Create quaternion from Euler angles
         * @param pitch Rotation around X axis (degrees)
         * @param yaw Rotation around Y axis (degrees)
         * @param roll Rotation around Z axis (degrees)
         * @return Rotation quaternion
         */
        inline glm::quat CreateQuaternion(float pitch, float yaw, float roll)
        {
            glm::vec3 radians = ToRadians(glm::vec3(pitch, yaw, roll));
            return glm::quat(radians);
        }

        /**
         * @brief Create quaternion for rotation around axis
         * @param axis Normalized rotation axis
         * @param angleDegrees Rotation angle in degrees
         * @return Rotation quaternion
         */
        inline glm::quat CreateQuaternion(const glm::vec3 &axis, float angleDegrees)
        {
            return glm::angleAxis(ToRadians(angleDegrees), glm::normalize(axis));
        }

        /**
         * @brief Create look-at quaternion
         * @param from Origin position
         * @param to Target position
         * @param up Up direction vector
         * @return Rotation quaternion that looks from 'from' to 'to'
         */
        glm::quat CreateLookAtQuaternion(const glm::vec3 &from,
                                         const glm::vec3 &to,
                                         const glm::vec3 &up = UP);

        /**
         * @brief Convert quaternion to Euler angles
         * @param rotation Input quaternion
         * @return Euler angles in degrees (pitch, yaw, roll)
         */
        glm::vec3 QuaternionToEuler(const glm::quat &rotation);

        /**
         * @brief Spherical linear interpolation between quaternions
         * @param from Start quaternion
         * @param to End quaternion
         * @param t Interpolation factor (0.0 to 1.0)
         * @return Interpolated quaternion
         */
        inline glm::quat SlerpQuaternion(const glm::quat &from,
                                         const glm::quat &to,
                                         float t)
        {
            return glm::slerp(from, to, t);
        }

        // --- Vector Utilities ---

        /**
         * @brief Check if vector is approximately zero
         * @param v Vector to check
         * @param epsilon Tolerance value
         * @return true if vector length is within epsilon of zero
         */
        inline bool IsZero(const glm::vec3 &v, float epsilon = EPSILON)
        {
            float len = glm::length(v);
            return len * len < epsilon * epsilon;
        }

        /**
         * @brief Check if vectors are approximately equal
         * @param a First vector
         * @param b Second vector
         * @param epsilon Tolerance value
         * @return true if vectors are within epsilon distance
         */
        inline bool AreEqual(const glm::vec3 &a, const glm::vec3 &b, float epsilon = EPSILON)
        {
            return IsZero(a - b, epsilon);
        }

        /**
         * @brief Safe vector normalization (returns zero vector if input is zero)
         * @param v Vector to normalize
         * @return Normalized vector or zero vector if input is zero
         */
        inline glm::vec3 SafeNormalize(const glm::vec3 &v)
        {
            float len = glm::length(v);
            return (len > EPSILON) ? v / len : glm::vec3(0.0f);
        }

        /**
         * @brief Linear interpolation between vectors
         * @param from Start vector
         * @param to End vector
         * @param t Interpolation factor (0.0 to 1.0)
         * @return Interpolated vector
         */
        inline glm::vec3 Lerp(const glm::vec3 &from, const glm::vec3 &to, float t)
        {
            return glm::mix(from, to, t);
        }

        /**
         * @brief Spherical linear interpolation between vectors
         * @param from Start vector (should be normalized)
         * @param to End vector (should be normalized)
         * @param t Interpolation factor (0.0 to 1.0)
         * @return Interpolated vector
         */
        glm::vec3 Slerp(const glm::vec3 &from, const glm::vec3 &to, float t);

        /**
         * @brief Clamp vector components to range
         * @param v Vector to clamp
         * @param minVal Minimum value for each component
         * @param maxVal Maximum value for each component
         * @return Vector with clamped components
         */
        inline glm::vec3 Clamp(const glm::vec3 &v, float minVal, float maxVal)
        {
            return glm::clamp(v, glm::vec3(minVal), glm::vec3(maxVal));
        }

        // --- Matrix Decomposition ---

        /**
         * @brief Extract translation from transformation matrix
         * @param matrix 4x4 transformation matrix
         * @return Translation vector
         */
        inline glm::vec3 ExtractTranslation(const glm::mat4 &matrix)
        {
            return glm::vec3(matrix[3]);
        }

        /**
         * @brief Extract scale from transformation matrix
         * @param matrix 4x4 transformation matrix
         * @return Scale vector
         */
        glm::vec3 ExtractScale(const glm::mat4 &matrix);

        /**
         * @brief Extract rotation quaternion from transformation matrix
         * @param matrix 4x4 transformation matrix
         * @return Rotation quaternion
         */
        glm::quat ExtractRotation(const glm::mat4 &matrix);

        /**
         * @brief Decompose transformation matrix into TRS components
         * @param matrix 4x4 transformation matrix
         * @param translation Output translation vector
         * @param rotation Output rotation quaternion
         * @param scale Output scale vector
         * @return true if decomposition was successful
         */
        bool DecomposeTRS(const glm::mat4 &matrix,
                          glm::vec3 &translation,
                          glm::quat &rotation,
                          glm::vec3 &scale);

        // --- Coordinate System Conversion ---

        /**
         * @brief Convert point from local to world coordinates
         * @param localPoint Point in local space
         * @param worldMatrix World transformation matrix
         * @return Point in world space
         */
        inline glm::vec3 LocalToWorld(const glm::vec3 &localPoint,
                                      const glm::mat4 &worldMatrix)
        {
            glm::vec4 worldPoint = worldMatrix * glm::vec4(localPoint, 1.0f);
            return glm::vec3(worldPoint);
        }

        /**
         * @brief Convert direction from local to world coordinates
         * @param localDirection Direction in local space
         * @param worldMatrix World transformation matrix
         * @return Direction in world space (not affected by translation)
         */
        inline glm::vec3 LocalDirectionToWorld(const glm::vec3 &localDirection,
                                               const glm::mat4 &worldMatrix)
        {
            glm::vec4 worldDirection = worldMatrix * glm::vec4(localDirection, 0.0f);
            return glm::normalize(glm::vec3(worldDirection));
        }

        /**
         * @brief Convert point from world to local coordinates
         * @param worldPoint Point in world space
         * @param worldMatrix World transformation matrix
         * @return Point in local space
         */
        inline glm::vec3 WorldToLocal(const glm::vec3 &worldPoint,
                                      const glm::mat4 &worldMatrix)
        {
            glm::mat4 inverse = glm::inverse(worldMatrix);
            glm::vec4 localPoint = inverse * glm::vec4(worldPoint, 1.0f);
            return glm::vec3(localPoint);
        }

        // --- Utility Functions ---

        /**
         * @brief Create perspective projection matrix
         * @param fov Field of view in degrees
         * @param aspectRatio Width / Height ratio
         * @param nearPlane Near clipping plane distance
         * @param farPlane Far clipping plane distance
         * @return Perspective projection matrix
         */
        inline glm::mat4 CreatePerspective(float fov, float aspectRatio,
                                           float nearPlane, float farPlane)
        {
            return glm::perspective(ToRadians(fov), aspectRatio, nearPlane, farPlane);
        }

        /**
         * @brief Create orthographic projection matrix
         * @param left Left clipping plane
         * @param right Right clipping plane
         * @param bottom Bottom clipping plane
         * @param top Top clipping plane
         * @param nearPlane Near clipping plane
         * @param farPlane Far clipping plane
         * @return Orthographic projection matrix
         */
        inline glm::mat4 CreateOrtho(float left, float right,
                                     float bottom, float top,
                                     float nearPlane, float farPlane)
        {
            return glm::ortho(left, right, bottom, top, nearPlane, farPlane);
        }

        /**
         * @brief Create look-at view matrix
         * @param eye Camera position
         * @param center Target position to look at
         * @param up Up direction vector
         * @return View matrix for camera transformation
         */
        inline glm::mat4 CreateLookAt(const glm::vec3 &eye,
                                      const glm::vec3 &center,
                                      const glm::vec3 &up = UP)
        {
            return glm::lookAt(eye, center, up);
        }

        /**
         * @brief Calculate distance between two points
         * @param a First point
         * @param b Second point
         * @return Distance between points
         */
        inline float Distance(const glm::vec3 &a, const glm::vec3 &b)
        {
            return glm::length(b - a);
        }

        /**
         * @brief Calculate squared distance (faster, no square root)
         * @param a First point
         * @param b Second point
         * @return Squared distance between points
         */
        inline float DistanceSquared(const glm::vec3 &a, const glm::vec3 &b)
        {
            glm::vec3 diff = b - a;
            float len = glm::length(diff);
            return len * len;
        }

        /**
         * @brief Wrap angle to range [0, 360) degrees
         * @param angle Angle in degrees
         * @return Equivalent angle in [0, 360) range
         */
        float WrapAngle360(float angle);

        /**
         * @brief Wrap angle to range [-180, 180) degrees
         * @param angle Angle in degrees
         * @return Equivalent angle in [-180, 180) range
         */
        float WrapAngle180(float angle);

    } // namespace Math
} // namespace AIEngine