/**
 * Shader.cpp - OpenGL Shader Program Management Implementation
 *
 * GLSL shader compilation, linking, and uniform management
 * implementation for OpenGL rendering pipeline.
 *
 * @version 1.0.0
 * @date 2026-03-04
 */

#include "Shader.hpp"
#include <GL/glew.h>
#include <fstream>
#include <iostream>
#include <sstream>

namespace AIEngine {

Shader::Shader(const std::string &vertexPath, const std::string &fragmentPath)
    : m_program(0), m_valid(false) {

  std::cout << "Shader: Loading shaders from files:\n";
  std::cout << "  - Vertex: " << vertexPath << "\n";
  std::cout << "  - Fragment: " << fragmentPath << "\n";

  // Read shader source files
  std::string vertexSource = ReadShaderFile(vertexPath);
  std::string fragmentSource = ReadShaderFile(fragmentPath);

  if (vertexSource.empty() || fragmentSource.empty()) {
    std::cerr << "Shader: Failed to read shader files\n";
    return;
  }

  // Compile shaders
  uint32_t vertexShader, fragmentShader;
  if (!CompileShader(vertexSource, GL_VERTEX_SHADER, vertexShader) ||
      !CompileShader(fragmentSource, GL_FRAGMENT_SHADER, fragmentShader)) {
    return;
  }

  // Link program
  if (LinkProgram(vertexShader, fragmentShader)) {
    m_valid = true;
    std::cout << "Shader: Program compiled and linked successfully\n";
  }

  // Cleanup individual shaders (they're linked into the program now)
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
}

Shader::Shader(const std::string &vertexSrc, const std::string &fragmentSrc,
               bool fromSource)
    : m_program(0), m_valid(false) {

  (void)fromSource; // Suppress unused parameter warning

  std::cout << "Shader: Creating shader from source strings\n";

  // Compile shaders
  uint32_t vertexShader, fragmentShader;
  if (!CompileShader(vertexSrc, GL_VERTEX_SHADER, vertexShader) ||
      !CompileShader(fragmentSrc, GL_FRAGMENT_SHADER, fragmentShader)) {
    return;
  }

  // Link program
  if (LinkProgram(vertexShader, fragmentShader)) {
    m_valid = true;
    std::cout << "Shader: Program compiled and linked successfully\n";
  }

  // Cleanup individual shaders
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
}

Shader::~Shader() {
  if (m_program != 0) {
    glDeleteProgram(m_program);
    std::cout << "Shader: Program deleted\n";
  }
}

Shader::Shader(Shader &&other) noexcept
    : m_program(other.m_program), m_valid(other.m_valid),
      m_uniformCache(std::move(other.m_uniformCache)) {

  other.m_program = 0;
  other.m_valid = false;
}

Shader &Shader::operator=(Shader &&other) noexcept {
  if (this != &other) {
    if (m_program != 0) {
      glDeleteProgram(m_program);
    }

    m_program = other.m_program;
    m_valid = other.m_valid;
    m_uniformCache = std::move(other.m_uniformCache);

    other.m_program = 0;
    other.m_valid = false;
  }
  return *this;
}

void Shader::Use() const {
  if (m_valid) {
    glUseProgram(m_program);
  } else {
    std::cerr << "Shader: Attempted to use invalid shader program\n";
  }
}

bool Shader::CompileShader(const std::string &source, uint32_t type,
                           uint32_t &shaderID) {
  shaderID = glCreateShader(type);
  const char *sourceCStr = source.c_str();
  glShaderSource(shaderID, 1, &sourceCStr, nullptr);
  glCompileShader(shaderID);

  // Check compilation status
  GLint success;
  glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
  if (!success) {
    GLint logLength;
    glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &logLength);

    std::string log(logLength, ' ');
    glGetShaderInfoLog(shaderID, logLength, nullptr, &log[0]);

    std::cerr << "Shader: Compilation failed for "
              << (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
              << " shader:\n"
              << log << "\n";

    glDeleteShader(shaderID);
    return false;
  }

  std::cout << "Shader: " << (type == GL_VERTEX_SHADER ? "Vertex" : "Fragment")
            << " shader compiled successfully\n";
  return true;
}

bool Shader::LinkProgram(uint32_t vertexShader, uint32_t fragmentShader) {
  m_program = glCreateProgram();
  glAttachShader(m_program, vertexShader);
  glAttachShader(m_program, fragmentShader);
  glLinkProgram(m_program);

  // Check linking status
  GLint success;
  glGetProgramiv(m_program, GL_LINK_STATUS, &success);
  if (!success) {
    GLint logLength;
    glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &logLength);

    std::string log(logLength, ' ');
    glGetProgramInfoLog(m_program, logLength, nullptr, &log[0]);

    std::cerr << "Shader: Program linking failed:\n" << log << "\n";

    glDeleteProgram(m_program);
    m_program = 0;
    return false;
  }

  return true;
}

std::string Shader::ReadShaderFile(const std::string &filepath) {
  std::ifstream file(filepath);
  if (!file.is_open()) {
    std::cerr << "Shader: Failed to open file: " << filepath << "\n";
    return "";
  }

  std::stringstream stream;
  stream << file.rdbuf();
  file.close();

  std::string content = stream.str();
  std::cout << "Shader: Read " << content.length() << " characters from "
            << filepath << "\n";

  return content;
}

int Shader::GetUniformLocation(const std::string &name) {
  auto it = m_uniformCache.find(name);
  if (it != m_uniformCache.end()) {
    return it->second;
  }

  int location = glGetUniformLocation(m_program, name.c_str());
  m_uniformCache[name] = location;

  if (location == -1) {
    std::cerr << "Shader: Warning - uniform '" << name << "' not found\n";
  }

  return location;
}

void Shader::SetBool(const std::string &name, bool value) {
  glUniform1i(GetUniformLocation(name), value ? 1 : 0);
}

void Shader::SetInt(const std::string &name, int value) {
  glUniform1i(GetUniformLocation(name), value);
}

void Shader::SetFloat(const std::string &name, float value) {
  glUniform1f(GetUniformLocation(name), value);
}

void Shader::SetVec2(const std::string &name, const glm::vec2 &value) {
  glUniform2fv(GetUniformLocation(name), 1, &value[0]);
}

void Shader::SetVec3(const std::string &name, const glm::vec3 &value) {
  glUniform3fv(GetUniformLocation(name), 1, &value[0]);
}

void Shader::SetVec4(const std::string &name, const glm::vec4 &value) {
  glUniform4fv(GetUniformLocation(name), 1, &value[0]);
}

void Shader::SetMat3(const std::string &name, const glm::mat3 &value) {
  glUniformMatrix3fv(GetUniformLocation(name), 1, 0, &value[0][0]);
}

void Shader::SetMat4(const std::string &name, const glm::mat4 &value) {
  glUniformMatrix4fv(GetUniformLocation(name), 1, 0, &value[0][0]);
}

} // namespace AIEngine
