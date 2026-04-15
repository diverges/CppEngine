/**
 * Mesh.cpp - 3D Mesh with Vertex Buffer Management Implementation
 *
 * Manages vertex data, indices, and OpenGL buffer objects
 * for efficient GPU-based mesh rendering.
 *
 * @version 1.0.0
 * @date 2026-03-04
 */

#include "Mesh.hpp"
#include <iostream>

namespace AIEngine {

Mesh::Mesh() : m_vao(0), m_vbo(0), m_ebo(0), m_uploaded(false) {
  // Empty mesh - data to be set later
}

Mesh::Mesh(const std::vector<Vertex> &vertices,
           const std::vector<uint32_t> &indices)
    : m_vertices(vertices), m_indices(indices), m_vao(0), m_vbo(0), m_ebo(0),
      m_uploaded(false) {
  // Mesh data provided - ready for GPU upload
}

Mesh::~Mesh() { CleanupGPUResources(); }

Mesh::Mesh(Mesh &&other) noexcept
    : m_vertices(std::move(other.m_vertices)),
      m_indices(std::move(other.m_indices)), m_vao(other.m_vao),
      m_vbo(other.m_vbo), m_ebo(other.m_ebo), m_uploaded(other.m_uploaded) {
  // Clear moved-from object
  other.m_vao = 0;
  other.m_vbo = 0;
  other.m_ebo = 0;
  other.m_uploaded = false;
}

Mesh &Mesh::operator=(Mesh &&other) noexcept {
  if (this != &other) {
    // Cleanup current resources
    CleanupGPUResources();

    // Move resources
    m_vertices = std::move(other.m_vertices);
    m_indices = std::move(other.m_indices);
    m_vao = other.m_vao;
    m_vbo = other.m_vbo;
    m_ebo = other.m_ebo;
    m_uploaded = other.m_uploaded;

    // Clear moved-from object
    other.m_vao = 0;
    other.m_vbo = 0;
    other.m_ebo = 0;
    other.m_uploaded = false;
  }
  return *this;
}

void Mesh::SetVertices(const std::vector<Vertex> &vertices) {
  m_vertices = vertices;
  m_uploaded = false; // Mark for re-upload if already uploaded
}

void Mesh::SetIndices(const std::vector<uint32_t> &indices) {
  m_indices = indices;
  m_uploaded = false; // Mark for re-upload if already uploaded
}

bool Mesh::UploadToGPU() {
  if (!IsValid()) {
    std::cerr << "Mesh: Cannot upload invalid mesh data to GPU\n";
    return false;
  }

  // TODO: OpenGL implementation
  // For now, simulate successful upload
  std::cout << "Mesh: Uploading " << m_vertices.size() << " vertices and "
            << m_indices.size() << " indices to GPU\n";

  // TODO: Generate VAO, VBO, EBO
  // glGenVertexArrays(1, &m_vao);
  // glGenBuffers(1, &m_vbo);
  // glGenBuffers(1, &m_ebo);

  // TODO: Upload vertex data
  // glBindVertexArray(m_vao);
  // glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  // glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex),
  //              m_vertices.data(), GL_STATIC_DRAW);

  // TODO: Upload index data
  // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
  // glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(uint32_t),
  //              m_indices.data(), GL_STATIC_DRAW);

  // TODO: Setup vertex attributes
  // SetupVertexAttributes();

  // TODO: Unbind
  // glBindVertexArray(0);

  // Simulate successful upload
  m_vao = 1; // Non-zero indicates uploaded
  m_uploaded = true;

  std::cout << "Mesh: GPU upload successful\n";
  return true;
}

void Mesh::Bind() const {
  if (!m_uploaded) {
    std::cerr << "Mesh: Cannot bind - mesh not uploaded to GPU\n";
    return;
  }

  // TODO: Bind VAO
  // glBindVertexArray(m_vao);
}

void Mesh::Unbind() const {
  // TODO: Unbind VAO
  // glBindVertexArray(0);
}

bool Mesh::IsValid() const {
  return !m_vertices.empty() && !m_indices.empty() &&
         (m_indices.size() % 3 == 0); // Must be divisible by 3 for triangles
}

void Mesh::CleanupGPUResources() {
  if (m_uploaded) {
    std::cout << "Mesh: Cleaning up GPU resources\n";

    // TODO: Delete OpenGL buffers
    // if (m_vao != 0) {
    //     glDeleteVertexArrays(1, &m_vao);
    //     m_vao = 0;
    // }
    // if (m_vbo != 0) {
    //     glDeleteBuffers(1, &m_vbo);
    //     m_vbo = 0;
    // }
    // if (m_ebo != 0) {
    //     glDeleteBuffers(1, &m_ebo);
    //     m_ebo = 0;
    // }

    m_uploaded = false;
  }
}

void Mesh::SetupVertexAttributes() {
  // TODO: Setup vertex attribute pointers
  // Position (location = 0)
  // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
  //                       (void*)offsetof(Vertex, position));
  // glEnableVertexAttribArray(0);

  // Normal (location = 1)
  // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
  //                       (void*)offsetof(Vertex, normal));
  // glEnableVertexAttribArray(1);

  // Texture coordinates (location = 2)
  // glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
  //                       (void*)offsetof(Vertex, texCoord));
  // glEnableVertexAttribArray(2);

  // Color (location = 3)
  // glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
  //                       (void*)offsetof(Vertex, color));
  // glEnableVertexAttribArray(3);
}

} // namespace AIEngine
