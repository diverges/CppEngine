/**
 * Renderer.cpp - OpenGL Graphics Renderer Implementation
 *
 * Core rendering system implementation with OpenGL context management,
 * scene traversal, and frame rendering capabilities.
 *
 * @version 1.0.0
 * @date 2026-03-04
 */

#include "../../include/AIEngine/graphics/Renderer.hpp"
#include "../../include/AIEngine/components/RenderComponent.hpp"
#include "../../include/AIEngine/components/TransformComponent.hpp"
#include "../../include/AIEngine/graphics/Shader.hpp"
#include "../../include/AIEngine/platform/Window.hpp"
#include "../../include/AIEngine/scene/SceneNode.hpp"
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include <chrono>
#include <iostream>

namespace AIEngine {

Renderer::Renderer(Window *window)
    : m_window(window), m_initialized(false), m_viewMatrix(glm::mat4(1.0f)),
      m_projectionMatrix(glm::mat4(1.0f)), m_fps(0.0f), m_frameCount(0),
      m_triangleCount(0), m_lastFrameTime(0.0), m_defaultShader(nullptr) {
  // Constructor - actual initialization happens in Initialize()
}

Renderer::~Renderer() {
  // Cleanup OpenGL resources
  if (m_initialized) {
    std::cout << "Renderer: Cleaning up OpenGL resources\n";

    // Cleanup cube geometry
    if (m_cubeVAO != 0) {
      glDeleteVertexArrays(1, &m_cubeVAO);
    }
    if (m_cubeVBO != 0) {
      glDeleteBuffers(1, &m_cubeVBO);
    }
    if (m_cubeEBO != 0) {
      glDeleteBuffers(1, &m_cubeEBO);
    }
  }
}

bool Renderer::Initialize() {
  std::cout << "Renderer: Initializing OpenGL renderer...\n";

  // Initialize GLEW for OpenGL function loading
  if (glewInit() != GLEW_OK) {
    std::cerr << "Renderer: Failed to initialize GLEW\n";
    return false;
  }

  std::cout << "Renderer: GLEW initialized successfully\n";
  std::cout << "Renderer: OpenGL Version: " << glGetString(GL_VERSION) << "\n";

  // Setup OpenGL state
  SetupOpenGLState();

  // Create default shader with embedded source for now
  std::string vertexSource = R"(
#version 330 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec4 aColor;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;
out vec4 Color;

void main() {
    FragPos = vec3(uModel * vec4(aPosition, 1.0));
    Normal = mat3(transpose(inverse(uModel))) * aNormal;
    TexCoord = aTexCoord;
    Color = aColor;
    
    gl_Position = uProjection * uView * vec4(FragPos, 1.0);
}
)";

  std::string fragmentSource = R"(
#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;  
in vec4 Color;

uniform vec3 uLightPos;
uniform vec3 uLightColor;
uniform vec3 uViewPos;

void main() {
    // Ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * uLightColor;
    
    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(uLightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * uLightColor;
    
    // Specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(uViewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * uLightColor;
    
    vec3 result = (ambient + diffuse + specular) * Color.rgb;
    FragColor = vec4(result, Color.a);
}
)";

  m_defaultShader =
      std::make_unique<Shader>(vertexSource, fragmentSource, true);
  if (!m_defaultShader->IsValid()) {
    std::cerr << "Renderer: Failed to create default shader\n";
    return false;
  }

  // Setup default projection matrix (perspective)
  if (m_window) {
    float aspectRatio = m_window->GetAspectRatio();
    m_projectionMatrix =
        glm::perspective(glm::radians(45.0f), // 45 degree FOV
                         aspectRatio,         // Aspect ratio from window
                         0.1f,                // Near plane
                         100.0f               // Far plane
        );
    std::cout << "Renderer: Perspective projection set (FOV=45°, aspect="
              << aspectRatio << ")\n";
  }

  // Setup default view matrix (camera at origin looking down -Z)
  m_viewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), // Camera position
                             glm::vec3(0.0f, 0.0f, 0.0f), // Look at origin
                             glm::vec3(0.0f, 1.0f, 0.0f)  // Up vector
  );

  auto now = std::chrono::high_resolution_clock::now();
  m_lastFrameTime =
      std::chrono::duration<double>(now.time_since_epoch()).count();

  m_initialized = true;
  std::cout << "Renderer: OpenGL renderer initialized successfully\n";
  return true;
}

void Renderer::BeginFrame() {
  if (!m_initialized) {
    return;
  }

  // Reset frame statistics
  m_triangleCount = 0;
  m_frameCount++;

  // Clear color and depth buffers
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // Dark blue-gray background
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Use default shader for rendering
  if (m_defaultShader && m_defaultShader->IsValid()) {
    m_defaultShader->Use();

    // Set transformation matrices
    m_defaultShader->SetMat4("uView", m_viewMatrix);
    m_defaultShader->SetMat4("uProjection", m_projectionMatrix);

    // Set lighting uniforms
    m_defaultShader->SetVec3("uLightPos", glm::vec3(2.0f, 2.0f, 2.0f));
    m_defaultShader->SetVec3("uLightColor", glm::vec3(1.0f, 1.0f, 1.0f));
    m_defaultShader->SetVec3("uViewPos", glm::vec3(0.0f, 0.0f, 3.0f));
  }

  // Update frame timing for FPS calculation
  UpdateFrameStats();
}

void Renderer::RenderScene(const SceneGraph *sceneGraph) {
  if (!m_initialized || !sceneGraph) {
    return;
  }

  // Render starting from root node
  const SceneNode *rootNode = sceneGraph->GetRootNode();
  if (rootNode) {
    RenderNode(rootNode, glm::mat4(1.0f));
  }
}

void Renderer::RenderNode(const SceneNode *node,
                          const glm::mat4 &parentTransform) {
  if (!node) {
    return;
  }

  // Calculate world transform for this node
  glm::mat4 worldTransform = parentTransform;

  // Apply node's local transform if it has a TransformComponent
  const auto *transformComp = node->GetComponent<TransformComponent>();
  if (transformComp) {
    worldTransform = parentTransform * transformComp->GetWorldMatrix();
  }

  // Render this node if it has a RenderComponent
  const auto *renderComp = node->GetComponent<RenderComponent>();
  if (renderComp && renderComp->IsVisible()) {
    // Set model matrix in shader
    if (m_defaultShader && m_defaultShader->IsValid()) {
      m_defaultShader->SetMat4("uModel", worldTransform);
    }

    // Draw actual cube geometry (for now, simulate with cube vertices)
    DrawCube();
    m_triangleCount += 12; // Cube has 12 triangles

    // Debug output (limited)
    static int renderCount = 0;
    if (renderCount++ < 5) { // Only show first few renders
      std::cout << "Renderer: Drawing node '" << node->GetName()
                << "' with mesh ID " << renderComp->GetMeshId()
                << " at transform [" << worldTransform[3][0] << ","
                << worldTransform[3][1] << "," << worldTransform[3][2] << "]\n";
    }
  }

  // Recursively render child nodes
  for (const auto *child : node->GetChildren()) {
    RenderNode(child, worldTransform);
  }
}

void Renderer::EndFrame() {
  if (!m_initialized) {
    return;
  }

  // TODO: Swap buffers
  // SDL_GL_SwapWindow(window);
}

void Renderer::SetViewMatrix(const glm::mat4 &view) { m_viewMatrix = view; }

void Renderer::SetProjectionMatrix(const glm::mat4 &projection) {
  m_projectionMatrix = projection;
}

void Renderer::SetupOpenGLState() {
  // Enable depth testing
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  // Set viewport
  if (m_window) {
    glViewport(0, 0, static_cast<int>(m_window->GetWidth()),
               static_cast<int>(m_window->GetHeight()));
    std::cout << "Renderer: Viewport set to " << m_window->GetWidth() << "x"
              << m_window->GetHeight() << "\n";
  }

  // Initialize cube geometry
  InitializeCubeGeometry();

  std::cout << "Renderer: OpenGL state configured\n";
}

void Renderer::InitializeCubeGeometry() {
  // Cube vertices with positions, normals, texture coords, and colors
  float vertices[] = {
      // Front face
      -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
      0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
      0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
      -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,

      // Back face
      -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
      1.0f, 0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
      1.0f, 0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
      1.0f, -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
      1.0f};

  unsigned int indices[] = {// Front face
                            0, 1, 2, 2, 3, 0,
                            // Back face
                            4, 5, 6, 6, 7, 4,
                            // Left face
                            7, 3, 0, 0, 4, 7,
                            // Right face
                            1, 5, 6, 6, 2, 1,
                            // Bottom face
                            0, 1, 5, 5, 4, 0,
                            // Top face
                            3, 2, 6, 6, 7, 3};

  // Generate and bind VAO
  glGenVertexArrays(1, &m_cubeVAO);
  glBindVertexArray(m_cubeVAO);

  // Generate and bind VBO
  glGenBuffers(1, &m_cubeVBO);
  glBindBuffer(GL_ARRAY_BUFFER, m_cubeVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // Generate and bind EBO
  glGenBuffers(1, &m_cubeEBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_cubeEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  // Set vertex attributes
  // Position (location 0)
  glVertexAttribPointer(0, 3, GL_FLOAT, 0, 12 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  // Normal (location 1)
  glVertexAttribPointer(1, 3, GL_FLOAT, 0, 12 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // Texture coordinates (location 2)
  glVertexAttribPointer(2, 2, GL_FLOAT, 0, 12 * sizeof(float),
                        (void *)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  // Color (location 3)
  glVertexAttribPointer(3, 4, GL_FLOAT, 0, 12 * sizeof(float),
                        (void *)(8 * sizeof(float)));
  glEnableVertexAttribArray(3);

  // Unbind
  glBindVertexArray(0);

  std::cout << "Renderer: Cube geometry initialized\n";
}

void Renderer::DrawCube() {
  if (m_cubeVAO == 0)
    return;

  glBindVertexArray(m_cubeVAO);
  glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

void Renderer::UpdateFrameStats() {
  auto now = std::chrono::high_resolution_clock::now();
  double currentTime =
      std::chrono::duration<double>(now.time_since_epoch()).count();

  double deltaTime = currentTime - m_lastFrameTime;
  if (deltaTime > 0.0) {
    m_fps = static_cast<float>(1.0 / deltaTime);
  }

  m_lastFrameTime = currentTime;
}

} // namespace AIEngine