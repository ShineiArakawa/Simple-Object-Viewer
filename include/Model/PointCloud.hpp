#pragma once

#include <Model/Primitives.hpp>
#include <Model/Sphere.hpp>
#include <Util/ObjectLoader.hpp>

class PointCloud : public Primitives {
 private:
  inline static const int NUM_DIVISIONS = 3;

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
  GLuint _textureId;

 protected:
  // nothing
 public:
  inline static const std::string KEY_MODEL_POINT_CLOUD = "Point Cloud";
  inline static const std::string KEY_MODEL_POINT_CLOUD_NAME = "Name";
  inline static const std::string KEY_MODEL_POINT_CLOUD_OBJ_PATH = "ObjPath";
  inline static const std::string KEY_MODEL_POINT_CLOUD_OFFSET = "Offset";
  inline static const std::string KEY_MODEL_POINT_CLOUD_SCALE = "Scale";
  inline static const std::string KEY_MODEL_POINT_CLOUD_NUM_DIVS = "nDivs";
  inline static const std::string KEY_MODEL_POINT_CLOUD_POINT_SIZE = "pointSize";

 private:
  // nothing
 protected:
  // nothing
 public:
  PointCloud(const std::string &, const float, const float, const float, const float, const float);
  ~PointCloud(){};

  void update() override{};
  void initVAO() override;
  void paintGL(const glm::mat4 &mvpMat) override;

  std::string getObjectType() override { return KEY_MODEL_POINT_CLOUD; };
};
