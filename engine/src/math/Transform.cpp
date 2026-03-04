/**
 * Transform.cpp - Mathematics Transform Utilities Implementation
 *
 * Implementation of complex transformation functions that require
 * more computation than simple inline operations.
 *
 * @version 1.0.0
 * @date 2026-03-04
 */

#include <cmath>
#include <algorithm>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

#include "../../include/AIEngine/math/Transform.hpp"

namespace AIEngine
{
    namespace Math
    {

        glm::quat CreateLookAtQuaternion(const glm::vec3 &from,
                                         const glm::vec3 &to,
                                         const glm::vec3 &up)
        {
            glm::vec3 forward = glm::normalize(to - from);
            glm::vec3 right = glm::normalize(glm::cross(forward, up));
            glm::vec3 newUp = glm::cross(right, forward);

            // Create rotation matrix from basis vectors
            glm::mat3 rotationMatrix;
            rotationMatrix[0] = right;
            rotationMatrix[1] = newUp;
            rotationMatrix[2] = -forward; // OpenGL uses -Z as forward

            // Convert rotation matrix to quaternion
            return glm::normalize(glm::quat_cast(rotationMatrix));
        }

        glm::vec3 QuaternionToEuler(const glm::quat &rotation)
        {
            // Convert quaternion to Euler angles using GLM
            glm::vec3 radians = glm::eulerAngles(rotation);
            return ToDegrees(radians);
        }

        glm::vec3 Slerp(const glm::vec3 &from, const glm::vec3 &to, float t)
        {
            // Clamp t to [0, 1]
            t = std::clamp(t, 0.0f, 1.0f);

            // Calculate angle between vectors
            float dot = glm::dot(glm::normalize(from), glm::normalize(to));
            dot = std::clamp(dot, -1.0f, 1.0f); // Handle numerical precision issues

            float angle = std::acos(dot);

            // If vectors are nearly parallel, use linear interpolation
            if (std::abs(angle) < EPSILON)
            {
                return Lerp(from, to, t);
            }

            // Spherical interpolation
            float sinAngle = std::sin(angle);
            float factor1 = std::sin((1.0f - t) * angle) / sinAngle;
            float factor2 = std::sin(t * angle) / sinAngle;

            return factor1 * from + factor2 * to;
        }

        glm::vec3 ExtractScale(const glm::mat4 &matrix)
        {
            // Extract scale from the lengths of the first three column vectors
            glm::vec3 scale;
            scale.x = glm::length(glm::vec3(matrix[0]));
            scale.y = glm::length(glm::vec3(matrix[1]));
            scale.z = glm::length(glm::vec3(matrix[2]));

            // Check for negative scale (determinant < 0)
            if (glm::determinant(matrix) < 0)
            {
                scale.x = -scale.x;
            }

            return scale;
        }

        glm::quat ExtractRotation(const glm::mat4 &matrix)
        {
            // Remove scale from matrix to get pure rotation
            glm::vec3 scale = ExtractScale(matrix);

            // Normalize the matrix columns to remove scale
            glm::mat3 rotationMatrix;
            rotationMatrix[0] = glm::vec3(matrix[0]) / scale.x;
            rotationMatrix[1] = glm::vec3(matrix[1]) / scale.y;
            rotationMatrix[2] = glm::vec3(matrix[2]) / scale.z;

            // Convert to quaternion
            return glm::normalize(glm::quat_cast(rotationMatrix));
        }

        bool DecomposeTRS(const glm::mat4 &matrix,
                          glm::vec3 &translation,
                          glm::quat &rotation,
                          glm::vec3 &scale)
        {
            // Use GLM's decompose function
            glm::vec3 skew;
            glm::vec4 perspective;

            bool success = glm::decompose(matrix, scale, rotation, translation, skew, perspective);

            if (success)
            {
                // Normalize the quaternion to ensure it's valid
                rotation = glm::normalize(rotation);
            }

            return success;
        }

        float WrapAngle360(float angle)
        {
            // Wrap angle to range [0, 360)
            angle = std::fmod(angle, 360.0f);
            if (angle < 0.0f)
            {
                angle += 360.0f;
            }
            return angle;
        }

        float WrapAngle180(float angle)
        {
            // Wrap angle to range [-180, 180)
            angle = WrapAngle360(angle);
            if (angle >= 180.0f)
            {
                angle -= 360.0f;
            }
            return angle;
        }

    } // namespace Math
} // namespace AIEngine