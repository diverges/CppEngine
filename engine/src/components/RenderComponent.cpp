/**
 * RenderComponent.cpp - Render Component Implementation
 *
 * Implementation of visual rendering functionality including
 * mesh management, visibility control, and rendering pipeline integration.
 *
 * @version 1.0.0
 * @date 2026-03-04
 */

#include "../../include/AIEngine/components/RenderComponent.hpp"
#include "../../include/AIEngine/components/TransformComponent.hpp"
#include "../../include/AIEngine/graphics/Renderer.hpp"
#include "../../include/AIEngine/scene/SceneNode.hpp"
#include <algorithm>
#include <sstream>

namespace AIEngine {

RenderComponent::RenderComponent() {
  // Initialize with default values (already done in header initializers)
}

RenderComponent::RenderComponent(const std::string &meshId)
    : m_meshId(meshId) {}

// --- Mesh Management ---

void RenderComponent::SetMeshId(const std::string &meshId) {
  if (m_meshId != meshId) {
    m_meshId = meshId;
    // Could trigger mesh loading or validation here in full implementation
  }
}

// --- Visibility Control ---

void RenderComponent::SetVisible(bool visible) { m_visible = visible; }

// --- Rendering Properties ---

void RenderComponent::SetRenderLayer(int layer) { m_renderLayer = layer; }

void RenderComponent::SetColor(float r, float g, float b) {
  m_color[0] = std::clamp(r, 0.0f, 1.0f);
  m_color[1] = std::clamp(g, 0.0f, 1.0f);
  m_color[2] = std::clamp(b, 0.0f, 1.0f);
}

void RenderComponent::SetColor(const float color[3]) {
  SetColor(color[0], color[1], color[2]);
}

void RenderComponent::GetColor(float color[3]) const {
  color[0] = m_color[0];
  color[1] = m_color[1];
  color[2] = m_color[2];
}

void RenderComponent::SetAlpha(float alpha) {
  m_alpha = std::clamp(alpha, 0.0f, 1.0f);
}

// --- Material Properties ---

void RenderComponent::SetMaterialId(const std::string &materialId) {
  m_materialId = materialId;
}

void RenderComponent::SetTextureId(const std::string &textureId) {
  m_textureId = textureId;
}

// --- Component Interface ---

void RenderComponent::OnAttach(SceneNode *owner) {
  Component::OnAttach(owner);

  // Initialize any rendering resources specific to this node
  // In full implementation, could:
  // - Register with renderer for efficient batching
  // - Validate mesh and material resources
  // - Setup rendering state
}

void RenderComponent::OnUpdate(double deltaTime) {
  (void)deltaTime; // Suppress unused parameter warning

  // Render components typically don't need per-frame updates
  // unless they have animation or dynamic properties

  // Future enhancements could include:
  // - Texture animation (UV scrolling, frame-based animation)
  // - Color animation or fading effects
  // - Material property animation
  // - LOD (Level of Detail) calculations based on distance
  // - Visibility culling preparation
}

void RenderComponent::OnRender(Renderer *renderer) {
  if (!IsReadyToRender() || !renderer) {
    return;
  }

  // Perform culling check
  if (ShouldCull(renderer)) {
    return; // Skip rendering if culled
  }

  // Delegate to internal render implementation
  PerformRender(renderer);

  // Update statistics
  m_renderCount++;
}

// --- Utility Methods ---

void RenderComponent::Reset() {
  m_meshId.clear();
  m_materialId.clear();
  m_textureId.clear();

  m_visible = true;
  m_renderLayer = 0;

  SetColor(1.0f, 1.0f, 1.0f);
  m_alpha = 1.0f;

  m_cullingEnabled = true;
  m_castShadows = true;
  m_receiveShadows = true;

  ResetRenderStats();
}

void RenderComponent::CopyFrom(const RenderComponent &other) {
  m_meshId = other.m_meshId;
  m_materialId = other.m_materialId;
  m_textureId = other.m_textureId;

  m_visible = other.m_visible;
  m_renderLayer = other.m_renderLayer;

  m_color[0] = other.m_color[0];
  m_color[1] = other.m_color[1];
  m_color[2] = other.m_color[2];
  m_alpha = other.m_alpha;

  m_cullingEnabled = other.m_cullingEnabled;
  m_castShadows = other.m_castShadows;
  m_receiveShadows = other.m_receiveShadows;

  // Don't copy render stats
  ResetRenderStats();
}

bool RenderComponent::IsReadyToRender() const {
  return m_visible && HasMesh() && IsActive() && GetOwner() != nullptr;
}

std::string RenderComponent::ToString() const {
  std::stringstream ss;
  ss << "RenderComponent(\n";
  ss << "  Mesh: '" << m_meshId << "'\n";
  ss << "  Material: '" << m_materialId << "'\n";
  ss << "  Texture: '" << m_textureId << "'\n";
  ss << "  Visible: " << (m_visible ? "true" : "false") << "\n";
  ss << "  Layer: " << m_renderLayer << "\n";
  ss << "  Color: (" << m_color[0] << ", " << m_color[1] << ", " << m_color[2]
     << ")\n";
  ss << "  Alpha: " << m_alpha << "\n";
  ss << "  Culling: " << (m_cullingEnabled ? "enabled" : "disabled") << "\n";
  ss << "  Shadows: Cast=" << (m_castShadows ? "yes" : "no")
     << ", Receive=" << (m_receiveShadows ? "yes" : "no") << "\n";
  ss << "  RenderCount: " << m_renderCount << "\n";
  ss << ")";
  return ss.str();
}

// --- Private Methods ---

void RenderComponent::PerformRender(Renderer *renderer) {
  if (!renderer) {
    return;
  }

  // Get transform component for world matrix
  TransformComponent *transform = nullptr;
  if (GetOwner()) {
    transform = GetOwner()->GetComponent<TransformComponent>();
  }

  // In a full implementation, this would:

  // 1. Set up rendering state
  // - Bind mesh geometry
  // - Set shader program
  // - Configure blending for transparency
  // - Set up texture units

  // 2. Set shader uniforms
  // - World matrix (from transform component)
  // - View and projection matrices (from renderer/camera)
  // - Material properties (color, alpha, etc.)
  // - Lighting parameters

  // 3. Issue draw call
  // - Draw indexed triangles
  // - Handle instancing if applicable
  // - Apply any post-processing effects

  // For now, we'll just log the render attempt
  // This gets replaced with actual OpenGL/rendering API calls

  // Example pseudo-code for actual implementation:
  /*
  // Set world transform
  if (transform) {
      glm::mat4 worldMatrix = transform->GetWorldMatrix();
      renderer->SetWorldMatrix(worldMatrix);
  }

  // Set material properties
  renderer->SetColor(m_color[0], m_color[1], m_color[2], m_alpha);

  // Bind resources
  if (!m_materialId.empty()) {
      renderer->BindMaterial(m_materialId);
  } else if (!m_textureId.empty()) {
      renderer->BindTexture(m_textureId);
  }

  // Draw mesh
  renderer->DrawMesh(m_meshId);
  */

  // Placeholder: In minimal implementation, we just validate parameters
  (void)renderer;  // Suppress unused warning
  (void)transform; // Suppress unused warning

  // Actual rendering will be implemented when graphics system is complete
}

bool RenderComponent::ShouldCull(Renderer *renderer) const {
  if (!m_cullingEnabled || !renderer) {
    return false; // No culling
  }

  // In full implementation, this would perform:
  // - Frustum culling (check if object is in camera view)
  // - Distance culling (too far to see)
  // - Occlusion culling (hidden behind other objects)
  // - Size culling (too small to see)

  // For now, we don't cull anything
  return false;

  // Example pseudo-code for frustum culling:
  /*
  if (GetOwner()) {
      auto* transform = GetOwner()->GetComponent<TransformComponent>();
      if (transform) {
          glm::vec3 position = transform->GetPosition();
          // Check if position is inside camera frustum
          return !renderer->IsPositionInFrustum(position);
      }
  }
  return false;
  */
}

} // namespace AIEngine