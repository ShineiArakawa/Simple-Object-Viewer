#include <SimView/Model/WireFrame.hpp>

namespace simview {
namespace model {

using namespace util;

WireFrame::WireFrame(const VertexArray_t& vertices, const IndexArray_t& indices)
    : _vaoId(),
      _vertexBufferId(),
      _indexBufferId(),
      _indexBufferSize() {
  initVAO(vertices, indices);
}

WireFrame::~WireFrame() = default;

void WireFrame::initVAO(const VertexArray_t& vertices, const IndexArray_t& indices) {
  auto lineVertices = std::make_shared<std::vector<LineVertex>>();
  IndexArray_t lineIndices = std::make_shared<std::vector<uint32_t>>();
  int idx = 0;

  const size_t nTriangles = indices->size() / 3ULL;
  for (size_t iTriangle = 0ULL; iTriangle < nTriangles; ++iTriangle) {
    const size_t offset = 3ULL * iTriangle;

    const LineVertex vertex0((*vertices)[(*indices)[offset + 0]].position);
    const LineVertex vertex1((*vertices)[(*indices)[offset + 1]].position);
    const LineVertex vertex2((*vertices)[(*indices)[offset + 2]].position);

    lineVertices->push_back(vertex0);
    lineVertices->push_back(vertex1);
    lineVertices->push_back(vertex2);

    lineIndices->push_back(idx + 0);
    lineIndices->push_back(idx + 1);

    lineIndices->push_back(idx + 1);
    lineIndices->push_back(idx + 2);

    lineIndices->push_back(idx + 2);
    lineIndices->push_back(idx + 0);

    idx += 3;
  }

  // Create VAO
  glGenVertexArrays(1, &_vaoId);
  glBindVertexArray(_vaoId);

  // Create vertex buffer object
  glGenBuffers(1, &_vertexBufferId);
  glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferId);
  glBufferData(GL_ARRAY_BUFFER, sizeof(LineVertex) * lineVertices->size(), lineVertices->data(), GL_STATIC_DRAW);

  // Setup attributes for vertex buffer object
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(LineVertex), (void*)offsetof(LineVertex, position));

  // Create index buffer object
  glGenBuffers(1, &_indexBufferId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufferId);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * lineIndices->size(), lineIndices->data(), GL_STATIC_DRAW);

  _indexBufferSize = (int)lineIndices->size();

  // Temporarily disable VAO
  glBindVertexArray(0);
}

void WireFrame::draw(const float& lineWidth) const {
  glBindVertexArray(_vaoId);

  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_BLEND);

  glLineWidth(lineWidth);

  glDrawElements(GL_LINES, _indexBufferSize, GL_UNSIGNED_INT, 0);

  glDisable(GL_LINE_SMOOTH);
  glDisable(GL_BLEND);

  glBindVertexArray(0);
}

}  // namespace model
}  // namespace simview
