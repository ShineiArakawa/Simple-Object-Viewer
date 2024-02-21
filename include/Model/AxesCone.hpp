#pragma once

#include <Model/Primitives.hpp>
#include <Util/ObjectLoader.hpp>

class AxesCone : public Primitives {
 private:
  float _offsetX;
  float _offsetY;
  float _offsetZ;
  float _scale;
  glm::vec3 _color;
  int _nDivs;

  int _indexBufferSize;
  GLuint _vaoId;
  GLuint _vertexBufferId;
  GLuint _indexBufferId;

 protected:
  // nothing
 public:
  inline static const std::string KEY_MODEL_AXES_CONE = "Axes Cone";

  inline static const float CONE_HIGHT = 0.2f;
  inline static const float CONE_RADIUS = 0.2f;
  inline static const float CYLINDER_RADIUS = 0.1f;

 private:
  // nothing
 protected:
  // nothing
 public:
  AxesCone(const int nDivs, const float offsetX, const float offsetY, const float offsetZ, const float scale);
  ~AxesCone();
  void update() override{};
  void initVAO() override;
  void paintGL(const glm::mat4& mvMat,
               const glm::mat4& mvpMat,
               const glm::mat4& normMat,
               const glm::mat4& lightMat,
               const glm::vec3& lightPos,
               const float& shininess,
               const float& ambientIntensity,
               const glm::vec3& wireFrameColor,
               const float& wireFrameWidth) override;
  std::string getObjectType() override { return KEY_MODEL_AXES_CONE; };
  void createXorientedCone(
      std::shared_ptr<std::vector<Vertex>>& vertices,
      std::shared_ptr<std::vector<unsigned int>>& indices,
      const glm::vec3& color) const;
};
