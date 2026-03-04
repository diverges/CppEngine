/**
 * Renderer.hpp - OpenGL Graphics Renderer Declaration
 *
 * Core rendering system with OpenGL context management,
 * scene traversal, and frame rendering capabilities.
 *
 * @version 1.0.0
 * @date 2026-03-04
 */

#pragma once

#include "../scene/SceneGraph.hpp"
#include "../platform/Window.hpp"
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace AIEngine
{

    // Forward declarations
    class SceneNode;
    class Window;
    class Mesh;
    class Shader;

    /**
     * @brief OpenGL renderer for 3D graphics
     *
     * Manages OpenGL context, handles scene traversal,
     * and renders 3D objects with proper transforms.
     */
    class Renderer
    {
    public:
        /**
         * @brief Initialize OpenGL renderer
         * @param window Window with valid OpenGL context
         */
        explicit Renderer(Window *window);

        /**
         * @brief Destructor - cleanup OpenGL resources
         */
        ~Renderer();

        // Prevent copying due to OpenGL resource management
        Renderer(const Renderer &) = delete;
        Renderer &operator=(const Renderer &) = delete;
        Renderer(Renderer &&) = delete;
        Renderer &operator=(Renderer &&) = delete;

        /**
         * @brief Initialize OpenGL state and resources
         * @return True if initialization successful
         */
        bool Initialize();

        /**
         * @brief Begin frame rendering
         */
        void BeginFrame();

        /**
         * @brief Render scene graph
         * @param sceneGraph Scene to render
         */
        void RenderScene(const SceneGraph *sceneGraph);

        /**
         * @brief Render a single scene node
         * @param node Node to render
         * @param transform Parent transform matrix
         */
        void RenderNode(const SceneNode *node, const glm::mat4 &transform = glm::mat4(1.0f));

        /**
         * @brief End frame and present
         */
        void EndFrame();

        /**
         * @brief Set view matrix (camera)
         * @param view View transformation matrix
         */
        void SetViewMatrix(const glm::mat4 &view);

        /**
         * @brief Set projection matrix
         * @param projection Projection transformation matrix
         */
        void SetProjectionMatrix(const glm::mat4 &projection);

        /**
         * @brief Get current frame rate
         * @return FPS as floating point
         */
        float GetFPS() const { return m_fps; }

        /**
         * @brief Get number of triangles rendered this frame
         * @return Triangle count
         */
        uint32_t GetTriangleCount() const { return m_triangleCount; }

    private:
        void SetupOpenGLState();
        void UpdateFrameStats();
        void InitializeCubeGeometry();
        void DrawCube();

        Window *m_window;
        bool m_initialized;

        // Graphics resources
        std::unique_ptr<Shader> m_defaultShader;

        // Cube rendering resources
        unsigned int m_cubeVAO = 0;
        unsigned int m_cubeVBO = 0;
        unsigned int m_cubeEBO = 0;

        // Transformation matrices
        glm::mat4 m_viewMatrix;
        glm::mat4 m_projectionMatrix;

        // Frame statistics
        float m_fps;
        uint32_t m_frameCount;
        uint32_t m_triangleCount;
        double m_lastFrameTime;
    };

}