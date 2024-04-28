#include <SimView/Model/WireFrame.hpp>

namespace simview {
namespace model {

using namespace util;

WireFrame::WireFrame(const VertexArray_t& vertices)
    : _vaoId(),
      _vertexBufferId(),
      _indexBufferId(),
      _indexBufferSize() {
  initVAO(vertices);
}

WireFrame::~WireFrame() = default;

void WireFrame::initVAO(const VertexArray_t& vertices) {
  VertexArray_t lineVertices = std::make_shared<std::vector<Vertex>>();
  IndexArray_t lineIndices = std::make_shared<std::vector<uint32_t>>();
  int idx = 0;

  glm::vec3 normal(0.0f), bary(0.0f);
  glm::vec2 uv(0.0f);

  for (const auto& vertex : *vertices) {
    Vertex lineVertex(vertex.position,
                      COLOR,
                      vertex.normal,
                      vertex.bary,
                      vertex.uv,
                      vertex.id);
    lineVertices->push_back(lineVertex);
    lineIndices->push_back(idx++);
  }

  // Create VAO
  glGenVertexArrays(1, &_vaoId);
  glBindVertexArray(_vaoId);

  // Create vertex buffer object
  glGenBuffers(1, &_vertexBufferId);
  glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferId);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * lineVertices->size(), lineVertices->data(), GL_STATIC_DRAW);

  // Setup attributes for vertex buffer object
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));

  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bary));

  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

  glEnableVertexAttribArray(5);
  glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, id));

  // Create index buffer object
  glGenBuffers(1, &_indexBufferId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufferId);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * lineIndices->size(), lineIndices->data(), GL_STATIC_DRAW);

  _indexBufferSize = (int)lineIndices->size();

  // Temporarily disable VAO
  glBindVertexArray(0);
}

void WireFrame::draw() const {
  glBindVertexArray(_vaoId);
  glDrawElements(GL_LINES, _indexBufferSize, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

}  // namespace model
}  // namespace simview
