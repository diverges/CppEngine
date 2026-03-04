/**
 * Mesh.hpp - 3D Mesh with Vertex Buffer Management
 *
 * Represents 3D geometry data with vertex attributes,
 * indices, and OpenGL buffer object management.
 *
 * @version 1.0.0
 * @date 2026-03-04
 */

#pragma once

#include <vector>
#include <cstdint>
#include <glm/glm.hpp>

namespace AIEngine
{

    /**
     * @brief Vertex data structure for 3D meshes
     */
    struct Vertex
    {
        glm::vec3 position; // 3D position (x, y, z)
        glm::vec3 normal;   // Surface normal vector
        glm::vec2 texCoord; // Texture coordinates (u, v)
        glm::vec3 color;    // Vertex color (r, g, b)

        Vertex() = default;

        Vertex(const glm::vec3 &pos, const glm::vec3 &norm = glm::vec3(0.0f, 0.0f, 1.0f),
               const glm::vec2 &tex = glm::vec2(0.0f, 0.0f), const glm::vec3 &col = glm::vec3(1.0f))
            : position(pos), normal(norm), texCoord(tex), color(col) {}
    };

    /**
     * @brief 3D mesh with vertex buffer object management
     *
     * Manages vertex data, indices, and OpenGL buffer objects
     * for efficient GPU-based mesh rendering.
     */
    class Mesh
    {
    public:
        /**
         * @brief Create empty mesh
         */
        Mesh();

        /**
         * @brief Create mesh with vertex and index data
         * @param vertices Vertex data array
         * @param indices Index data array for triangle definition
         */
        Mesh(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices);

        /**
         * @brief Destructor - cleanup OpenGL buffers
         */
        ~Mesh();

        // Prevent copying due to OpenGL resource management
        Mesh(const Mesh &) = delete;
        Mesh &operator=(const Mesh &) = delete;

        // Allow moving for container storage
        Mesh(Mesh &&other) noexcept;
        Mesh &operator=(Mesh &&other) noexcept;

        /**
         * @brief Set vertex data
         * @param vertices Vertex data array
         */
        void SetVertices(const std::vector<Vertex> &vertices);

        /**
         * @brief Set index data
         * @param indices Index data array
         */
        void SetIndices(const std::vector<uint32_t> &indices);

        /**
         * @brief Upload mesh data to GPU
         * @return True if upload successful
         */
        bool UploadToGPU();

        /**
         * @brief Bind mesh for rendering
         */
        void Bind() const;

        /**
         * @brief Unbind mesh after rendering
         */
        void Unbind() const;

        /**
         * @brief Get number of vertices
         * @return Vertex count
         */
        size_t GetVertexCount() const { return m_vertices.size(); }

        /**
         * @brief Get number of indices
         * @return Index count
         */
        size_t GetIndexCount() const { return m_indices.size(); }

        /**
         * @brief Get number of triangles
         * @return Triangle count (indices / 3)
         */
        size_t GetTriangleCount() const { return m_indices.size() / 3; }

        /**
         * @brief Check if mesh has valid data
         * @return True if mesh has vertices and indices
         */
        bool IsValid() const;

        /**
         * @brief Check if mesh data is uploaded to GPU
         * @return True if GPU buffers are created
         */
        bool IsUploaded() const { return m_vao != 0; }

        /**
         * @brief Get vertex data (read-only)
         * @return Const reference to vertices
         */
        const std::vector<Vertex> &GetVertices() const { return m_vertices; }

        /**
         * @brief Get index data (read-only)
         * @return Const reference to indices
         */
        const std::vector<uint32_t> &GetIndices() const { return m_indices; }

    private:
        void CleanupGPUResources();
        void SetupVertexAttributes();

        // Mesh data
        std::vector<Vertex> m_vertices;
        std::vector<uint32_t> m_indices;

        // OpenGL buffer objects
        uint32_t m_vao; // Vertex Array Object
        uint32_t m_vbo; // Vertex Buffer Object
        uint32_t m_ebo; // Element Buffer Object

        bool m_uploaded; // Track GPU upload status
    };

}