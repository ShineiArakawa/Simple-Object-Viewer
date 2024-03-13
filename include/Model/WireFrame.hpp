#pragma once

#include <OpenGL.hpp>
#include <Util/DataStructure.hpp>
#include <Util/ObjectLoader.hpp>
#include <memory>
#include <string>
#include <vector>

namespace simview {
namespace model {

class WireFrame {
 private:
  GLuint _vaoId;
  GLuint _vertexBufferId;
  GLuint _indexBufferId;
  int _indexBufferSize;

  inline static const glm::vec3 COLOR = glm::vec3(15.0f / 255.0f,
                                                  230.0f / 255.0f,
                                                  130.0f / 255.0f);

 protected:
  // nothing
 public:
  inline static const std::string KEY_MODEL_WIRE_FRAME = "Wire frame";

 private:
  void initVAO(const VertexArray_t&);

 protected:
  // nothing
 public:
  WireFrame(const VertexArray_t&);
  ~WireFrame();
  void draw() const;
};

using WireFrame_t = std::shared_ptr<WireFrame>;

}  // namespace model
}  // namespace simview