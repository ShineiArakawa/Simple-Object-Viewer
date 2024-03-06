#pragma once

#include <OpenGL.hpp>
#include <array>
#include <memory>
#include <string>
#include <vector>

namespace simview {
namespace model {
inline static const std::array<glm::vec3, 3> BARY_CENTER = {
    glm::vec3(1.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f),
    glm::vec3(0.0f, 0.0f, 1.0f)};

struct Vertex {
  Vertex() = default;
  Vertex(
      const glm::vec3& position_,
      const glm::vec3& color_,
      const glm::vec3& normal_,
      const glm::vec3& bary_,
      const glm::vec2& uv_,
      const float id_)
      : position(position_), color(color_), normal(normal_), bary(bary_), uv(uv_), id(id_){};

  glm::vec3 position;
  glm::vec3 color;
  glm::vec3 normal;
  glm::vec3 bary;
  glm::vec2 uv;
  float id;
};

struct MaterialGroup {
  MaterialGroup()
      : vertices(std::shared_ptr<std::vector<Vertex>>(new std::vector<Vertex>)),
        indices(std::shared_ptr<std::vector<uint32_t>>(new std::vector<uint32_t>)){};

  std::shared_ptr<std::vector<Vertex>> vertices = nullptr;
  std::shared_ptr<std::vector<uint32_t>> indices = nullptr;

  float shininess;          // Ns
  glm::vec3 ambientColor;   // Ka
  glm::vec3 diffuseColor;   // Kd
  glm::vec3 specularColor;  // Ks
  glm::vec3 emissiveColor;  // Ke
  float refractiveIndex;    // Ni
  float opacity;            // d

  std::string ambientTexturePath;             // map_Ka
  std::string diffuseTexturePath;             // map_Kd
  std::string specularTexturePath;            // map_Ks
  std::string specularReflectionTexturePath;  // map_Ns
  std::string opacityTexturePath;             // map_d
  std::string roughnessTexturePath;           // disp
  std::string bumpTexturePath;                // map_Bump
  std::string reflectionDiffuseTexturePath;   // refl
};

using MaterialGroup_t = std::shared_ptr<MaterialGroup>;
using MaterialGroups_t = std::shared_ptr<std::vector<MaterialGroup_t>>;
}  // namespace model
}  // namespace simview