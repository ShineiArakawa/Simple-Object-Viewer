#pragma once

#include <SimView/Model/Primitives.hpp>
#include <SimView/Util/Logging.hpp>
#include <SimView/Util/ObjectLoader.hpp>

namespace simview {
namespace model {

class PointCloud : public Primitive {
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
  PointCloud(const std::string& filePath = "",
             const float offsetX = 0.0f,
             const float offsetY = 0.0f,
             const float offsetZ = 0.0f,
             const float scale = 1.0f,
             const float pointSize = 0.1f);
  ~PointCloud() = default;

  void update() override{};
  void initVAO() override;
  void initVAO(const VertexArray_t&,
               const IndexArray_t&);
  void initVAO(const std::shared_ptr<std::vector<vec3f_t>> positions,
               const std::shared_ptr<std::vector<vec3f_t>> colors = nullptr,
               const std::shared_ptr<std::vector<int>> ids = nullptr);
  void paintGL(
      const TransformationContext& transCtx,  // transCtx
      const LightingContext& lightingCtx,     // lightingCtx
      const RenderingContext& renderingCtx    // renderingCtx
      ) override;
  void drawGL(const int& index = 0) override;
  void drawAllGL(const glm::mat4& lightMvpMat) override;

  std::string getObjectType() override { return KEY_MODEL_POINT_CLOUD; };
};

using PointCloud_t = std::shared_ptr<PointCloud>;

}  // namespace model
}  // namespace simview