/**
 * TransformNode.cpp - Transform Node Implementation
 *
 * Implementation of 3D transformation functionality including
 * position, rotation, scale, and matrix calculations.
 *
 * @version 1.0.0
 * @date 2026-03-04
 */

#include <AIEngine/nodes/TransformNode.hpp>
#include <AIEngine/scene/SceneNode.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/string_cast.hpp>
#include <sstream>

namespace AIEngine {

TransformNode::TransformNode()
    : m_position(0.0f, 0.0f, 0.0f),
      m_rotation(1.0f, 0.0f, 0.0f, 0.0f) // Identity quaternion
      ,
      m_scale(1.0f, 1.0f, 1.0f), m_localMatrix(1.0f) // Identity matrix
      ,
      m_isDirty(true) {}

TransformNode::TransformNode(const glm::vec3 &position,
                             const glm::quat &rotation, const glm::vec3 &scale)
    : m_position(position), m_rotation(rotation), m_scale(scale),
      m_localMatrix(1.0f), m_isDirty(true) {}

// --- Position Methods ---

void TransformNode::SetPosition(const glm::vec3 &position) {
  if (m_position != position) {
    m_position = position;
    MarkDirty();
  }
}

void TransformNode::SetPosition(float x, float y, float z) {
  SetPosition(glm::vec3(x, y, z));
}

void TransformNode::Translate(const glm::vec3 &offset) {
  SetPosition(m_position + offset);
}

void TransformNode::Translate(float x, float y, float z) {
  Translate(glm::vec3(x, y, z));
}

// --- Rotation Methods ---

void TransformNode::SetRotation(const glm::quat &rotation) {
  m_rotation = glm::normalize(rotation);
  MarkDirty();
}

void TransformNode::SetRotationEuler(float pitch, float yaw, float roll) {
  // Convert degrees to radians and create quaternion
  glm::vec3 radians = glm::radians(glm::vec3(pitch, yaw, roll));
  m_rotation = glm::quat(radians);
  MarkDirty();
}

void TransformNode::LookAt(const glm::vec3 &target, const glm::vec3 &up) {
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

glm::vec3 TransformNode::GetEulerAngles() const {
  // Convert quaternion to Euler angles and return in degrees
  glm::vec3 radians = glm::eulerAngles(m_rotation);
  return glm::degrees(radians);
}

void TransformNode::Rotate(const glm::quat &rotation) {
  m_rotation = glm::normalize(m_rotation * rotation);
  MarkDirty();
}

void TransformNode::RotateAround(const glm::vec3 &axis, float angleDegrees) {
  float angleRadians = glm::radians(angleDegrees);
  glm::quat rotation = glm::angleAxis(angleRadians, glm::normalize(axis));
  Rotate(rotation);
}

// --- Scale Methods ---

void TransformNode::SetScale(float uniformScale) {
  SetScale(glm::vec3(uniformScale));
}

void TransformNode::SetScale(const glm::vec3 &scale) {
  if (m_scale != scale) {
    m_scale = scale;
    MarkDirty();
  }
}

void TransformNode::SetScale(float x, float y, float z) {
  SetScale(glm::vec3(x, y, z));
}

void TransformNode::Scale(const glm::vec3 &scaleMultiplier) {
  SetScale(m_scale * scaleMultiplier);
}

// --- Matrix Methods ---

const glm::mat4 &TransformNode::GetLocalMatrix() const {
  UpdateMatrixIfDirty();
  return m_localMatrix;
}

glm::mat4 TransformNode::GetWorldMatrix() const {
  glm::mat4 worldMatrix = GetLocalMatrix();

  // If this node belongs to a scene node, check for parent transforms
  if (GetOwner() && GetOwner()->GetParent()) {
    SceneNode *parent = GetOwner()->GetParent();

    // Look for parent transform node
    if (auto *parentTransform = parent->GetNode<TransformNode>()) {
      // Multiply by parent's world matrix (parent transforms apply first)
      worldMatrix = parentTransform->GetWorldMatrix() * worldMatrix;
    }
  }

  return worldMatrix;
}

glm::mat4 TransformNode::GetInverseWorldMatrix() const {
  return glm::inverse(GetWorldMatrix());
}

// --- Direction Vectors ---

glm::vec3 TransformNode::GetForward() const {
  // Forward is typically -Z in OpenGL coordinate system
  glm::vec4 forward =
      glm::mat4_cast(m_rotation) * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
  return glm::normalize(glm::vec3(forward));
}

glm::vec3 TransformNode::GetRight() const {
  // Right is typically +X
  glm::vec4 right =
      glm::mat4_cast(m_rotation) * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
  return glm::normalize(glm::vec3(right));
}

glm::vec3 TransformNode::GetUp() const {
  // Up is typically +Y
  glm::vec4 up = glm::mat4_cast(m_rotation) * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
  return glm::normalize(glm::vec3(up));
}

// --- Node Interface ---

void TransformNode::OnAttach(SceneNode *owner) {
  Node::OnAttach(owner);
  // No special attachment logic needed for transforms
}

// --- Utility Methods ---

void TransformNode::Reset() {
  m_position = glm::vec3(0.0f, 0.0f, 0.0f);
  m_rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f); // Identity
  m_scale = glm::vec3(1.0f, 1.0f, 1.0f);
  MarkDirty();
}

void TransformNode::CopyFrom(const TransformNode &other) {
  m_position = other.m_position;
  m_rotation = other.m_rotation;
  m_scale = other.m_scale;
  MarkDirty();
}

std::string TransformNode::ToString() const {
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

void TransformNode::UpdateMatrixIfDirty() const {
  if (m_isDirty) {
    m_localMatrix = CalculateMatrix();
    m_isDirty = false;
  }
}

glm::mat4 TransformNode::CalculateMatrix() const {
  // Standard TRS (Translate * Rotate * Scale) matrix calculation

  // Scale matrix
  glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), m_scale);

  // Rotation matrix from quaternion
  glm::mat4 rotationMatrix = glm::mat4_cast(m_rotation);

  // Translation matrix
  glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), m_position);

  // Combine: Translation * Rotation * Scale
  // (Order matters: scale happens first in object space, then rotate, then
  // translate)
  return translationMatrix * rotationMatrix * scaleMatrix;
}

} // namespace AIEngine
