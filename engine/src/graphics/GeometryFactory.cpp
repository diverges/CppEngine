/**
 * GeometryFactory.cpp - 3D Geometry Creation Utilities Implementation
 * 
 * Factory class implementation for creating common 3D geometries
 * with proper vertex data, normals, and texture coordinates.
 * 
 * @version 1.0.0
 * @date 2026-03-04
 */

#include "../../include/AIEngine/graphics/GeometryFactory.hpp"
#include <iostream>

namespace AIEngine {

    std::unique_ptr<Mesh> GeometryFactory::CreateCube(float size, const glm::vec3& color) {
        std::cout << "GeometryFactory: Creating cube with size " << size << "\n";
        
        float halfSize = size * 0.5f;
        
        // Define cube vertices with positions, normals, texture coordinates, and colors
        std::vector<Vertex> vertices = {
            // Front face (z = +halfSize)
            Vertex(glm::vec3(-halfSize, -halfSize,  halfSize), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f), color),  // Bottom-left
            Vertex(glm::vec3( halfSize, -halfSize,  halfSize), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f), color),  // Bottom-right  
            Vertex(glm::vec3( halfSize,  halfSize,  halfSize), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f), color),  // Top-right
            Vertex(glm::vec3(-halfSize,  halfSize,  halfSize), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f), color),  // Top-left
            
            // Back face (z = -halfSize)
            Vertex(glm::vec3(-halfSize, -halfSize, -halfSize), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 0.0f), color), // Bottom-left
            Vertex(glm::vec3( halfSize, -halfSize, -halfSize), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 0.0f), color), // Bottom-right
            Vertex(glm::vec3( halfSize,  halfSize, -halfSize), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 1.0f), color), // Top-right
            Vertex(glm::vec3(-halfSize,  halfSize, -halfSize), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 1.0f), color), // Top-left
            
            // Right face (x = +halfSize)
            Vertex(glm::vec3( halfSize, -halfSize, -halfSize), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f), color),  // Bottom-left
            Vertex(glm::vec3( halfSize, -halfSize,  halfSize), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f), color),  // Bottom-right
            Vertex(glm::vec3( halfSize,  halfSize,  halfSize), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f), color),  // Top-right
            Vertex(glm::vec3( halfSize,  halfSize, -halfSize), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f), color),  // Top-left
            
            // Left face (x = -halfSize)
            Vertex(glm::vec3(-halfSize, -halfSize, -halfSize), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f), color), // Bottom-left
            Vertex(glm::vec3(-halfSize, -halfSize,  halfSize), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f), color), // Bottom-right
            Vertex(glm::vec3(-halfSize,  halfSize,  halfSize), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f), color), // Top-right
            Vertex(glm::vec3(-halfSize,  halfSize, -halfSize), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f), color), // Top-left
            
            // Top face (y = +halfSize)
            Vertex(glm::vec3(-halfSize,  halfSize,  halfSize), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f), color),  // Bottom-left
            Vertex(glm::vec3( halfSize,  halfSize,  halfSize), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f), color),  // Bottom-right
            Vertex(glm::vec3( halfSize,  halfSize, -halfSize), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f), color),  // Top-right
            Vertex(glm::vec3(-halfSize,  halfSize, -halfSize), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f), color),  // Top-left
            
            // Bottom face (y = -halfSize)
            Vertex(glm::vec3(-halfSize, -halfSize, -halfSize), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 1.0f), color), // Bottom-left
            Vertex(glm::vec3( halfSize, -halfSize, -halfSize), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 1.0f), color), // Bottom-right
            Vertex(glm::vec3( halfSize, -halfSize,  halfSize), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 0.0f), color), // Top-right
            Vertex(glm::vec3(-halfSize, -halfSize,  halfSize), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f), color)  // Top-left
        };
        
        // Define triangular indices for cube faces (counter-clockwise when viewed from outside)
        std::vector<uint32_t> indices = {
            // Front face
            0, 1, 2,    2, 3, 0,
            // Back face  
            4, 6, 5,    6, 4, 7,
            // Right face
            8, 9, 10,   10, 11, 8,
            // Left face
            12, 14, 13, 14, 12, 15,
            // Top face
            16, 17, 18, 18, 19, 16,
            // Bottom face
            20, 22, 21, 22, 20, 23
        };
        
        std::cout << "GeometryFactory: Cube created with " << vertices.size() 
                  << " vertices and " << indices.size() << " indices\n";
        
        return std::make_unique<Mesh>(vertices, indices);
    }

    std::unique_ptr<Mesh> GeometryFactory::CreateColoredCube(float size, const glm::vec3 colors[6]) {
        std::cout << "GeometryFactory: Creating colored cube with size " << size << "\n";
        
        float halfSize = size * 0.5f;
        
        // Create cube with different colors for each face
        std::vector<Vertex> vertices = {
            // Front face (colors[0])
            Vertex(glm::vec3(-halfSize, -halfSize,  halfSize), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f), colors[0]),
            Vertex(glm::vec3( halfSize, -halfSize,  halfSize), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f), colors[0]),
            Vertex(glm::vec3( halfSize,  halfSize,  halfSize), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f), colors[0]),
            Vertex(glm::vec3(-halfSize,  halfSize,  halfSize), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f), colors[0]),
            
            // Back face (colors[1])
            Vertex(glm::vec3(-halfSize, -halfSize, -halfSize), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 0.0f), colors[1]),
            Vertex(glm::vec3( halfSize, -halfSize, -halfSize), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 0.0f), colors[1]),
            Vertex(glm::vec3( halfSize,  halfSize, -halfSize), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 1.0f), colors[1]),
            Vertex(glm::vec3(-halfSize,  halfSize, -halfSize), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 1.0f), colors[1]),
            
            // Right face (colors[2])
            Vertex(glm::vec3( halfSize, -halfSize, -halfSize), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f), colors[2]),
            Vertex(glm::vec3( halfSize, -halfSize,  halfSize), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f), colors[2]),
            Vertex(glm::vec3( halfSize,  halfSize,  halfSize), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f), colors[2]),
            Vertex(glm::vec3( halfSize,  halfSize, -halfSize), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f), colors[2]),
            
            // Left face (colors[3])
            Vertex(glm::vec3(-halfSize, -halfSize, -halfSize), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f), colors[3]),
            Vertex(glm::vec3(-halfSize, -halfSize,  halfSize), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f), colors[3]),
            Vertex(glm::vec3(-halfSize,  halfSize,  halfSize), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f), colors[3]),
            Vertex(glm::vec3(-halfSize,  halfSize, -halfSize), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f), colors[3]),
            
            // Top face (colors[4])
            Vertex(glm::vec3(-halfSize,  halfSize,  halfSize), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f), colors[4]),
            Vertex(glm::vec3( halfSize,  halfSize,  halfSize), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f), colors[4]),
            Vertex(glm::vec3( halfSize,  halfSize, -halfSize), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f), colors[4]),
            Vertex(glm::vec3(-halfSize,  halfSize, -halfSize), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f), colors[4]),
            
            // Bottom face (colors[5])
            Vertex(glm::vec3(-halfSize, -halfSize, -halfSize), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 1.0f), colors[5]),
            Vertex(glm::vec3( halfSize, -halfSize, -halfSize), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 1.0f), colors[5]),
            Vertex(glm::vec3( halfSize, -halfSize,  halfSize), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 0.0f), colors[5]),
            Vertex(glm::vec3(-halfSize, -halfSize,  halfSize), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f), colors[5])
        };
        
        // Same indices as regular cube
        std::vector<uint32_t> indices = {
            0, 1, 2,    2, 3, 0,      // Front
            4, 6, 5,    6, 4, 7,      // Back
            8, 9, 10,   10, 11, 8,    // Right
            12, 14, 13, 14, 12, 15,   // Left
            16, 17, 18, 18, 19, 16,   // Top
            20, 22, 21, 22, 20, 23    // Bottom
        };
        
        return std::make_unique<Mesh>(vertices, indices);
    }

    std::unique_ptr<Mesh> GeometryFactory::CreateQuad(float width, float height, const glm::vec3& color) {
        std::cout << "GeometryFactory: Creating quad " << width << "x" << height << "\n";
        
        float halfWidth = width * 0.5f;
        float halfHeight = height * 0.5f;
        
        std::vector<Vertex> vertices = {
            Vertex(glm::vec3(-halfWidth, -halfHeight, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f), color), // Bottom-left
            Vertex(glm::vec3( halfWidth, -halfHeight, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f), color), // Bottom-right
            Vertex(glm::vec3( halfWidth,  halfHeight, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f), color), // Top-right
            Vertex(glm::vec3(-halfWidth,  halfHeight, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f), color)  // Top-left
        };
        
        std::vector<uint32_t> indices = {
            0, 1, 2,  // First triangle
            2, 3, 0   // Second triangle
        };
        
        return std::make_unique<Mesh>(vertices, indices);
    }

    std::unique_ptr<Mesh> GeometryFactory::CreateTriangle(const glm::vec3& color) {
        std::cout << "GeometryFactory: Creating triangle\n";
        
        std::vector<Vertex> vertices = {
            Vertex(glm::vec3(0.0f,  0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.5f, 1.0f), color), // Top
            Vertex(glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f), color), // Bottom-left
            Vertex(glm::vec3(0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f), color)  // Bottom-right
        };
        
        std::vector<uint32_t> indices = {
            0, 1, 2  // Single triangle
        };
        
        return std::make_unique<Mesh>(vertices, indices);
    }

} // namespace AIEngine