#pragma once

#include <SimView/OpenGL.hpp>
#include <SimView/Util/DataStructure.hpp>
#include <SimView/Util/ObjectLoader.hpp>
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

 protected:
  // nothing
 public:
  inline static const std::string KEY_MODEL_WIRE_FRAME = "Wire frame";

 private:
  void initVAO(const VertexArray_t&, const IndexArray_t&);

 protected:
  // nothing
 public:
  WireFrame(const VertexArray_t&, const IndexArray_t&);
  ~WireFrame();
  void draw(const float& lineWidth) const;
};

using WireFrame_t = std::shared_ptr<WireFrame>;

}  // namespace model
}  // namespace simview