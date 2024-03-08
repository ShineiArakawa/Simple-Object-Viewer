#include <Model/AxisAlignedBoundingBox.hpp>

namespace simview {
namespace model {

using namespace util;

AxisAlignedBoundingBox::AxisAlignedBoundingBox(const glm::vec3& minCoords,
                                               const glm::vec3& maxCoords)
    : _minCoords(minCoords),
      _maxCoords(maxCoords) {
  initVAO();
}

AxisAlignedBoundingBox::~AxisAlignedBoundingBox() = default;

void AxisAlignedBoundingBox::initVAO() {
  VertexArray_t vertices = std::make_shared<std::vector<Vertex>>();
  IndexArray_t indices = std::make_shared<std::vector<uint32_t>>();
  int idx = 0;

  glm::vec3 normal(0.0f), bary(0.0f);
  glm::vec2 uv(0.0f);

  for (int iLine = 0; iLine < 12; ++iLine) {
    Vertex v0(POSITIONS[LINES[iLine][0]], COLOR, normal, bary, uv, 0.0f);
    vertices->push_back(v0);
    indices->push_back(idx++);

    Vertex v1(POSITIONS[LINES[iLine][1]], COLOR, normal, bary, uv, 0.0f);
    vertices->push_back(v1);
    indices->push_back(idx++);
  }

  const glm::vec3 intervals = _maxCoords - _minCoords;

  ObjectLoader::moveToOrigin(vertices);
  ObjectLoader::scaleObject(vertices, intervals);
  ObjectLoader::moveToOrigin(vertices);
  ObjectLoader::translateObject(vertices, _minCoords + intervals / 2.0f);

  // Create VAO
  glGenVertexArrays(1, &_vaoId);
  glBindVertexArray(_vaoId);

  // Create vertex buffer object
  glGenBuffers(1, &_vertexBufferId);
  glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferId);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices->size(), vertices->data(), GL_STATIC_DRAW);

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
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * indices->size(), indices->data(), GL_STATIC_DRAW);

  // Temporarily disable VAO
  glBindVertexArray(0);
}

void AxisAlignedBoundingBox::draw() const {
  glBindVertexArray(_vaoId);
  glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

}  // namespace model
}  // namespace simview