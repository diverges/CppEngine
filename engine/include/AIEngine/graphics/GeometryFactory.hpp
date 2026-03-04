/**
 * GeometryFactory.hpp - 3D Geometry Creation Utilities
 * 
 * Factory class for creating common 3D geometries like cubes,
 * spheres, planes, and other primitive shapes.
 * 
 * @version 1.0.0
 * @date 2026-03-04
 */

#pragma once

#include "Mesh.hpp"
#include <memory>
#include <glm/glm.hpp>

namespace AIEngine {
    
    /**
     * @brief Factory for creating common 3D geometries
     * 
     * Provides static methods to create standard 3D shapes
     * with proper vertex data, normals, and texture coordinates.
     */
    class GeometryFactory {
    public:
        /**
         * @brief Create a unit cube mesh
         * @param size Size of the cube (default 1.0)
         * @param color Color for all vertices (default white)
         * @return Unique pointer to created mesh
         */
        static std::unique_ptr<Mesh> CreateCube(float size = 1.0f, 
                                                 const glm::vec3& color = glm::vec3(1.0f));
        
        /**
         * @brief Create a cube with individual face colors
         * @param size Size of the cube
         * @param colors Array of 6 colors for each face (front, back, right, left, top, bottom)
         * @return Unique pointer to created mesh
         */
        static std::unique_ptr<Mesh> CreateColoredCube(float size, const glm::vec3 colors[6]);
        
        /**
         * @brief Create a quad/plane mesh
         * @param width Width of the quad
         * @param height Height of the quad
         * @param color Color for all vertices
         * @return Unique pointer to created mesh
         */
        static std::unique_ptr<Mesh> CreateQuad(float width = 1.0f, float height = 1.0f,
                                                 const glm::vec3& color = glm::vec3(1.0f));
        
        /**
         * @brief Create a triangle mesh
         * @param color Color for all vertices
         * @return Unique pointer to created mesh
         */
        static std::unique_ptr<Mesh> CreateTriangle(const glm::vec3& color = glm::vec3(1.0f));
        
    private:
        // Static class - no instances
        GeometryFactory() = delete;
        ~GeometryFactory() = delete;
        GeometryFactory(const GeometryFactory&) = delete;
        GeometryFactory& operator=(const GeometryFactory&) = delete;
    };
    
}