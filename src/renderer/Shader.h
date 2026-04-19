/** 
* @file Shader.h
 * @since 20th April 2026
 * @author Dev Patel
 *
 * Shader class responsible for loading , compiling and managing
 * OpenGL GLSL shader programs on the GPU.
 *
*/

#pragma once

#include "glm/fwd.hpp"
#include <cstdint>
#include <filesystem>
#include <string>
#include <string_view>
#include <unordered_map>


#include <glad/glad.h>
#include <glm/glm.hpp>

namespace Caliber {

class Shader {
public:
  // Constructor loads and compile files from their locations

  Shader(const std::filesystem::path &vertPath,
         const std::filesystem::path &fragPath);

  // Deleting Copy Constructor.
  Shader(const Shader &) = delete;
  Shader &operator=(const Shader &) = delete;

  // Move Constructor
  Shader(Shader &&other) noexcept;
  Shader &operator=(Shader &&other) noexcept;

  void bind() const { glUseProgram(m_id); }
  void unbind() const { glUseProgram(0); }

  // uniform setters
  void setInt(std::string_view name, int value) const;
  void setFloat(std::string_view name, float value) const;
  void setVec3(std::string_view name, const glm::vec3 &value) const;
  void setVec4(std::string_view name, const glm::vec4 &value) const;
  void setMat4(std::string_view name, const glm::mat4 &value) const;

  [[nodiscard]] uint32_t getID() const { return m_id; }

  ~Shader();

private:
  uint32_t m_id{0};

  // cache uniform locations so we don't call UniformLocation Every Frame :)
  mutable std::unordered_map<std::string, int> m_uniformCache;

  // Helpers
  [[nodiscard]] static std::string readFile(const std::filesystem::path &path);
  [[nodiscard]] static uint32_t compileShader(GLenum type, const std::string& source);
  [[nodiscard]] static uint32_t linkProgram(uint32_t vert, uint32_t frag);
  [[nodiscard]] int getUniformLocation(std::string_view name) const;
};

} // namespace Caliber