#include <Model/PointCloud.hpp>

PointCloud::PointCloud(const std::string &filePath, const float offsetX, const float offsetY, const float offsetZ, const float scale, const float pointSize, const bool isDoubled)
    : _filePath(filePath),
      _offsetX(offsetX),
      _offsetY(offsetY),
      _offsetZ(offsetZ),
      _scale(scale),
      _pointSize(pointSize),
      _isDoubled(isDoubled) {
}

void PointCloud::initVAO() {
  std::shared_ptr<std::vector<Vertex>> points = std::make_shared<std::vector<Vertex>>();
  std::shared_ptr<std::vector<uint32_t>> pointsIndices = std::make_shared<std::vector<uint32_t>>();

  ObjectLoader::readFromFile(_filePath, points, pointsIndices, _offsetX, _offsetY, _offsetZ);
  ObjectLoader::moveToOrigin(points);
  ObjectLoader::scaleObject(points, _scale);
  ObjectLoader::move(points, _offsetX, _offsetY, _offsetZ);

  // for (const auto vert : (*points)) {
  //   std::cout << vert.position.x << ", " << vert.position.y << ", " << vert.position.z << "\n";
  // }

  std::shared_ptr<std::vector<Vertex>> vertices = std::make_shared<std::vector<Vertex>>();
  std::shared_ptr<std::vector<uint32_t>> indices = std::make_shared<std::vector<uint32_t>>();

  int index = 0;

  vertices->resize((NUM_DIVISIONS - 1) * (2 * NUM_DIVISIONS - 1) * 6 * (int)points->size());
  indices->resize((NUM_DIVISIONS - 1) * (2 * NUM_DIVISIONS - 1) * 6 * (int)points->size());

  std::shared_ptr<std::vector<Vertex>> primitiveVertices = std::make_shared<std::vector<Vertex>>();
  std::shared_ptr<std::vector<uint32_t>> primitiveIndices = std::make_shared<std::vector<uint32_t>>();
  Sphere::createSphere(NUM_DIVISIONS, primitiveVertices, primitiveIndices, _isDoubled);
  ObjectLoader::moveToOrigin(primitiveVertices);
  ObjectLoader::scaleObject(primitiveVertices, _pointSize);

  for (int iPoint = 0; iPoint < (int)points->size(); ++iPoint) {
    const Vertex point = (*points)[iPoint];

    std::shared_ptr<std::vector<Vertex>> iVertices = std::make_shared<std::vector<Vertex>>(*primitiveVertices);
    std::shared_ptr<std::vector<uint32_t>> iIndices = std::make_shared<std::vector<uint32_t>>(*primitiveIndices);

    ObjectLoader::move(iVertices, point.position[0], point.position[1], point.position[2]);

    for (int iVertex = 0; iVertex < (int)iVertices->size(); ++iVertex) {
      (*vertices)[index + iVertex] = (*iVertices)[iVertex];
      (*indices)[index + iVertex] = index + (*iIndices)[iVertex];
    }

    index += (int)iVertices->size();
  }

  // Create VAO
  glGenVertexArrays(1, &_vaoId);
  glBindVertexArray(_vaoId);

  // Create vertex buffer object
  glGenBuffers(1, &_vertexBufferId);
  glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferId);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices->size(), vertices->data(), GL_STATIC_DRAW);

  // Setup attributes for vertex buffer object
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, position));

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, color));

  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, normal));

  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, uv));

  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, id));

  // Create index buffer object
  glGenBuffers(1, &_indexBufferId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufferId);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices->size(), indices->data(), GL_STATIC_DRAW);

  _indexBufferSize = (int)indices->size();

  // Temporarily disable VAO
  glBindVertexArray(0);
}

void PointCloud::paintGL(const glm::mat4 &mvMat,
                         const glm::mat4 &mvpMat,
                         const glm::mat4 &normMat,
                         const glm::mat4 &lightMat,
                         const glm::vec3 &lightPos,
                         const float &shininess,
                         const float &ambientIntensity) {
  if (_isVisible) {
    GLuint uid;
    const glm::mat4 &mvtMat = mvMat * glm::translate(_position);
    const glm::mat4 &mvptMat = mvpMat * glm::translate(_position);

    bindShader(mvtMat, mvptMat, normMat, lightMat, lightPos, shininess, ambientIntensity, getRenderType());

    // Enable VAO
    glBindVertexArray(_vaoId);

    // Draw triangles
    glDrawElements(GL_TRIANGLES, _indexBufferSize, GL_UNSIGNED_INT, 0);

    // Disable VAO
    glBindVertexArray(0);

    unbindShader();
  }
}