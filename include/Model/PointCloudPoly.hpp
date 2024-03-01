#pragma once

#include <Model/Primitives.hpp>
#include <Model/Sphere.hpp>
#include <Util/ObjectLoader.hpp>

class PointCloudPoly : public Primitives {
 private:
  inline static const int NUM_DIVISIONS = 3;

  std::string _filePath;
  float _offsetX;
  float _offsetY;
  float _offsetZ;
  float _scale;
  float _pointSize;
  bool _isDoubled;

  int _indexBufferSize;
  GLuint _vaoId;
  GLuint _vertexBufferId;
  GLuint _indexBufferId;

 protected:
  // nothing
 public:
  inline static const std::string KEY_MODEL_POINT_CLOUD_POLY = "Point Cloud with Polygon";
  inline static const std::string KEY_MODEL_POINT_CLOUD_POLY_NAME = "Name";
  inline static const std::string KEY_MODEL_POINT_CLOUD_POLY_OBJ_PATH = "ObjPath";
  inline static const std::string KEY_MODEL_POINT_CLOUD_POLY_OFFSET = "Offset";
  inline static const std::string KEY_MODEL_POINT_CLOUD_POLY_SCALE = "Scale";
  inline static const std::string KEY_MODEL_POINT_CLOUD_POLY_NUM_DIVS = "nDivs";
  inline static const std::string KEY_MODEL_POINT_CLOUD_POLY_POINT_SIZE = "Point size";

 private:
  // nothing
 protected:
  // nothing
 public:
  PointCloudPoly(const std::string&, const float, const float, const float, const float, const float, const bool);
  ~PointCloudPoly() = default;

  void update() override{};
  void initVAO() override;
  void paintGL(const glm::mat4& mvMat,
               const glm::mat4& mvpMat,
               const glm::mat4& lightMat,
               const glm::vec3& lightPos,
               const float& shininess,
               const float& ambientIntensity,
               const glm::vec3& wireFrameColor,
               const float& wireFrameWidth,
               const GLuint& depthTextureId,
               const glm::mat4& lightMvpMat) override;
  void drawGL(const int& index = 0) override;
  void drawAllGL(const glm::mat4& lightMvpMat) override;

  std::string getObjectType() override { return KEY_MODEL_POINT_CLOUD_POLY; };
};
