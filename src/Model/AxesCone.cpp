#include <Model/AxesCone.hpp>

AxesCone::AxesCone(const int nDivs, const float offsetX, const float offsetY, const float offsetZ, const float scale)
    : _nDivs(nDivs),
      _offsetX(offsetX),
      _offsetY(offsetY),
      _offsetZ(offsetZ),
      _scale(_scale) {
}

AxesCone::~AxesCone() = default;

void AxesCone::createXorientedCone(
    std::shared_ptr<std::vector<Vertex>>& vertices,
    std::shared_ptr<std::vector<unsigned int>>& indices,
    const glm::vec3& color) const {
  const float deltaTheta = 2 * M_PI / (float)(_nDivs - 1);
  const glm::vec3 origin(0.0f, 0.0f, 0.0f);
  const glm::vec3 apex(1.0f, 0.0f, 0.0f);
  const glm::vec3 coneBottom(1.0f - CONE_HIGHT, 0.0f, 0.0f);
  int index = 0;

  for (int iTheta = 0; iTheta < _nDivs - 1; ++iTheta) {
    const float theta = (float)iTheta * deltaTheta;

    const glm::vec3 coneBottomPosition0(1.0f - CONE_HIGHT, CONE_RADIUS * std::cos(theta), CONE_RADIUS * std::sin(theta));
    const glm::vec3 coneBottomPosition1(1.0f - CONE_HIGHT, CONE_RADIUS * std::cos(theta + deltaTheta), CONE_RADIUS * std::sin(theta + deltaTheta));

    const glm::vec3 coneNormal = glm::normalize(glm::cross(coneBottomPosition1 - coneBottomPosition0, apex - coneBottomPosition1));

    // Cone
    vertices->push_back(Vertex(coneBottomPosition0, color, glm::vec3(0.0f, std::cos(theta), std::sin(theta)), glm::vec2(0.0f), 0.0f));
    indices->push_back(index++);
    vertices->push_back(Vertex(coneBottomPosition1, color, glm::vec3(0.0f, std::cos(theta + deltaTheta), std::sin(theta + deltaTheta)), glm::vec2(0.0f), 0.0f));
    indices->push_back(index++);
    vertices->push_back(Vertex(apex, color, apex, glm::vec2(0.0f), 0.0f));
    indices->push_back(index++);

    // Cone lit
    vertices->push_back(Vertex(coneBottomPosition0, color, -apex, glm::vec2(0.0f), 0.0f));
    indices->push_back(index++);
    vertices->push_back(Vertex(coneBottom, color, -apex, glm::vec2(0.0f), 0.0f));
    indices->push_back(index++);
    vertices->push_back(Vertex(coneBottomPosition1, color, -apex, glm::vec2(0.0f), 0.0f));
    indices->push_back(index++);

    const glm::vec3 cylinderTopPosition0(1.0f - CONE_HIGHT, CYLINDER_RADIUS * std::cos(theta), CYLINDER_RADIUS * std::sin(theta));
    const glm::vec3 cylinderTopPosition1(1.0f - CONE_HIGHT, CYLINDER_RADIUS * std::cos(theta + deltaTheta), CYLINDER_RADIUS * std::sin(theta + deltaTheta));
    const glm::vec3 cylinderBottomPosition0(0.0f, CYLINDER_RADIUS * std::cos(theta), CYLINDER_RADIUS * std::sin(theta));
    const glm::vec3 cylinderBottomPosition1(0.0f, CYLINDER_RADIUS * std::cos(theta + deltaTheta), CYLINDER_RADIUS * std::sin(theta + deltaTheta));

    const glm::vec3 cylinderNormal0 = glm::vec3(0.0f, std::cos(theta), std::sin(theta));
    const glm::vec3 cylinderNormal1 = glm::vec3(0.0f, std::cos(theta + deltaTheta), std::sin(theta + deltaTheta));

    // Cylinder side
    vertices->push_back(Vertex(cylinderTopPosition1, color, cylinderNormal1, glm::vec2(0.0f), 0.0f));
    indices->push_back(index++);
    vertices->push_back(Vertex(cylinderTopPosition0, color, cylinderNormal0, glm::vec2(0.0f), 0.0f));
    indices->push_back(index++);
    vertices->push_back(Vertex(cylinderBottomPosition0, color, cylinderNormal0, glm::vec2(0.0f), 0.0f));
    indices->push_back(index++);
    vertices->push_back(Vertex(cylinderBottomPosition0, color, cylinderNormal0, glm::vec2(0.0f), 0.0f));
    indices->push_back(index++);
    vertices->push_back(Vertex(cylinderBottomPosition1, color, cylinderNormal1, glm::vec2(0.0f), 0.0f));
    indices->push_back(index++);
    vertices->push_back(Vertex(cylinderTopPosition1, color, cylinderNormal1, glm::vec2(0.0f), 0.0f));
    indices->push_back(index++);

    // Cylinder lit
    vertices->push_back(Vertex(cylinderBottomPosition0, color, -apex, glm::vec2(0.0f), 0.0f));
    indices->push_back(index++);
    vertices->push_back(Vertex(origin, color, -apex, glm::vec2(0.0f), 0.0f));
    indices->push_back(index++);
    vertices->push_back(Vertex(cylinderBottomPosition1, color, -apex, glm::vec2(0.0f), 0.0f));
    indices->push_back(index++);
  }
}

void AxesCone::initVAO() {
  // X-axis cone
  std::shared_ptr<std::vector<Vertex>> xConeVertices = std::make_shared<std::vector<Vertex>>();
  std::shared_ptr<std::vector<unsigned int>> xConeIndices = std::make_shared<std::vector<unsigned int>>();
  createXorientedCone(xConeVertices, xConeIndices, glm::vec3(1.0f, 0.0f, 0.0f));

  // Y-axis cone
  std::shared_ptr<std::vector<Vertex>> yConeVertices = std::make_shared<std::vector<Vertex>>();
  std::shared_ptr<std::vector<unsigned int>> yConeIndices = std::make_shared<std::vector<unsigned int>>();
  createXorientedCone(yConeVertices, yConeIndices, glm::vec3(0.0f, 1.0f, 0.0f));
  rotateObject(yConeVertices, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

  // Z-axis cone
  std::shared_ptr<std::vector<Vertex>> zConeVertices = std::make_shared<std::vector<Vertex>>();
  std::shared_ptr<std::vector<unsigned int>> zConeIndices = std::make_shared<std::vector<unsigned int>>();
  createXorientedCone(zConeVertices, zConeIndices, glm::vec3(0.0f, 0.0f, 1.0f));
  rotateObject(zConeVertices, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

  std::shared_ptr<std::vector<Vertex>> vertices = std::make_shared<std::vector<Vertex>>();
  std::shared_ptr<std::vector<unsigned int>> indices = std::make_shared<std::vector<unsigned int>>();

  mergeVertices(xConeVertices, xConeIndices, vertices, indices);
  mergeVertices(yConeVertices, yConeIndices, vertices, indices);
  mergeVertices(zConeVertices, zConeIndices, vertices, indices);

  // ObjectLoader::scaleObject(vertices, _scale);
  ObjectLoader::move(vertices, _offsetX, _offsetY, _offsetZ);

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
  glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, id));

  // Create index buffer object
  glGenBuffers(1, &_indexBufferId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufferId);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices->size(), indices->data(), GL_STATIC_DRAW);

  _indexBufferSize = (int)indices->size();

  // Temporarily disable VAO
  glBindVertexArray(0);
}

void AxesCone::paintGL(const glm::mat4& mvMat,
                       const glm::mat4& mvpMat,
                       const glm::mat4& normMat,
                       const glm::mat4& lightMat,
                       const glm::vec3& lightPos,
                       const float& shininess,
                       const float& ambientIntensity) {
  if (_isVisible) {
    const glm::mat4& mvtMat = mvMat * glm::translate(_position);
    const glm::mat4& mvptMat = mvpMat * glm::translate(_position);

    bindShader(
        mvtMat,
        mvptMat,
        normMat,
        lightMat,
        lightPos,
        shininess,
        ambientIntensity,
        getRenderType(false, Primitives::RenderType::SHADE));

    // Enable VAO
    glBindVertexArray(_vaoId);

    // Draw triangles
    glDrawElements(GL_TRIANGLES, _indexBufferSize, GL_UNSIGNED_INT, 0);

    // Disable VAO
    glBindVertexArray(0);

    unbindShader();
  }
}
