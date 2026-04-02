/**
 * Shader.hpp - OpenGL Shader Program Management
 *
 * GLSL shader compilation, linking, and uniform management
 * for OpenGL rendering pipeline.
 *
 * @version 1.0.0
 * @date 2026-03-04
 */

#pragma once

#include <cstdint>
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>

namespace AIEngine {

/**
 * @brief OpenGL shader program wrapper
 *
 * Manages GLSL shader compilation, program linking,
 * and uniform variable access for GPU rendering.
 */
class Shader {
public:
  /**
   * @brief Create shader from file paths
   * @param vertexPath Path to vertex shader file
   * @param fragmentPath Path to fragment shader file
   */
  Shader(const std::string &vertexPath, const std::string &fragmentPath);

  /**
   * @brief Create shader from source strings
   * @param vertexSrc Vertex shader source code
   * @param fragmentSrc Fragment shader source code
   * @param fromSource Tag to differentiate from file constructor
   */
  Shader(const std::string &vertexSrc, const std::string &fragmentSrc,
         bool fromSource);

  /**
   * @brief Destructor - cleanup OpenGL resources
   */
  ~Shader();

  // Prevent copying due to OpenGL resource management
  Shader(const Shader &) = delete;
  Shader &operator=(const Shader &) = delete;

  // Allow moving for container storage
  Shader(Shader &&other) noexcept;
  Shader &operator=(Shader &&other) noexcept;

  /**
   * @brief Use this shader program for rendering
   */
  void Use() const;

  /**
   * @brief Get OpenGL program ID
   * @return Program handle
   */
  uint32_t GetProgram() const { return m_program; }

  /**
   * @brief Check if shader compiled and linked successfully
   * @return True if ready for use
   */
  bool IsValid() const { return m_valid; }

  // Uniform setting methods
  void SetBool(const std::string &name, bool value);
  void SetInt(const std::string &name, int value);
  void SetFloat(const std::string &name, float value);
  void SetVec2(const std::string &name, const glm::vec2 &value);
  void SetVec3(const std::string &name, const glm::vec3 &value);
  void SetVec4(const std::string &name, const glm::vec4 &value);
  void SetMat3(const std::string &name, const glm::mat3 &value);
  void SetMat4(const std::string &name, const glm::mat4 &value);

private:
  bool CompileShader(const std::string &source, uint32_t type,
                     uint32_t &shaderID);
  bool LinkProgram(uint32_t vertexShader, uint32_t fragmentShader);
  std::string ReadShaderFile(const std::string &filepath);
  int GetUniformLocation(const std::string &name);

  uint32_t m_program;
  bool m_valid;

  // Cache uniform locations for performance
  mutable std::unordered_map<std::string, int> m_uniformCache;
};

} // namespace AIEngine