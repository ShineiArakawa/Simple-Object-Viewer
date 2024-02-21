#pragma once

#include <Model/Primitives.hpp>
#include <Util/Logging.hpp>
#include <Util/ObjectLoader.hpp>

class PointCloud : public Primitives {
 private:
  std::string _filePath;
  float _offsetX;
  float _offsetY;
  float _offsetZ;
  float _scale;
  float _pointSize;

  int _indexBufferSize;
  GLuint _vaoId;
  GLuint _vertexBufferId;
  GLuint _indexBufferId;

 protected:
  // nothing
 public:
  inline static const std::string KEY_MODEL_POINT_CLOUD = "Point Cloud";
  inline static const std::string KEY_MODEL_POINT_CLOUD_NAME = "Name";
  inline static const std::string KEY_MODEL_POINT_CLOUD_OBJ_PATH = "ObjPath";
  inline static const std::string KEY_MODEL_POINT_CLOUD_OFFSET = "Offset";
  inline static const std::string KEY_MODEL_POINT_CLOUD_SCALE = "Scale";
  inline static const std::string KEY_MODEL_POINT_CLOUD_POINT_SIZE = "Point size";

 private:
  // nothing
 protected:
  // nothing
 public:
  PointCloud(const std::string&, const float, const float, const float, const float, const float);
  ~PointCloud() = default;

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

  std::string getObjectType() override { return KEY_MODEL_POINT_CLOUD; };
};
