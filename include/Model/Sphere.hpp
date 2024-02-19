#pragma once

#include <Model/Primitives.hpp>
#include <Util/ObjectLoader.hpp>

class Sphere : public Primitives {
 private:
  float _offsetX;
  float _offsetY;
  float _offsetZ;
  float _scaleX;
  float _scaleY;
  float _scaleZ;
  int _nDivs;
  int _indexBufferSize;
  GLuint _vaoId;
  GLuint _vertexBufferId;
  GLuint _indexBufferId;

 protected:
  // nothing
 public:
  inline static const std::string KEY_MODEL_SPHERE = "Sphere";
  inline static const std::string KEY_MODEL_SPHERE_CENTER = "Center";
  inline static const std::string KEY_MODEL_SPHERE_SCALE = "Scale";
  inline static const std::string KEY_MODEL_NUM_DIVS = "nDivs";

 private:
  // nothing
 protected:
  // nothing
 public:
  Sphere(const int nDivs, const float offsetX, const float offsetY, const float offsetZ, const float scaleX, const float scaleY, const float scaleZ);
  ~Sphere();
  void update() override{};
  void initVAO() override;
  void paintGL(const glm::mat4 &mvpMat) override;
  std::string getObjectType() override { return KEY_MODEL_SPHERE; };

  static void createSphere(const int nDivs, std::shared_ptr<std::vector<Vertex>> vertices, std::shared_ptr<std::vector<unsigned int>> indices);
  static void createSphere(const int nDivs, const glm::vec3 color, std::shared_ptr<std::vector<Vertex>> vertices, std::shared_ptr<std::vector<unsigned int>> indices);
};
