#include <Model/Sphere.hpp>

Sphere::Sphere(const int nDivs, const float offsetX, const float offsetY, const float offsetZ, const float scaleX, const float scaleY, const float scaleZ, const glm::vec3 color)
    : _nDivs(nDivs),
      _offsetX(offsetX),
      _offsetY(offsetY),
      _offsetZ(offsetZ),
      _scaleX(scaleX),
      _scaleY(scaleY),
      _scaleZ(scaleZ),
      _color(color) {
}

Sphere::~Sphere() = default;

void Sphere::initVAO() {
  std::shared_ptr<std::vector<Vertex>> vertices = std::make_shared<std::vector<Vertex>>();
  std::shared_ptr<std::vector<unsigned int>> indices = std::make_shared<std::vector<unsigned int>>();

  createSphere(_nDivs, _color, vertices, indices);

  ObjectLoader::moveToOrigin(vertices);
  ObjectLoader::scaleObject(vertices, _scaleX / 2.0f, _scaleY / 2.0f, _scaleZ / 2.0f);
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

void Sphere::paintGL(const glm::mat4 &mvMat,
                     const glm::mat4 &mvpMat,
                     const glm::mat4 &normMat,
                     const glm::mat4 &lightMat,
                     const glm::vec3 &lightPos,
                     const float &shininess,
                     const float &ambientIntensity) {
  if (_isVisible) {
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

void Sphere::createSphere(const int nDivs, std::shared_ptr<std::vector<Vertex>> vertices, std::shared_ptr<std::vector<unsigned int>> indices, const bool isDoubled) {
  createSphere(nDivs, glm::vec3(0.0f), vertices, indices, isDoubled);
}

void Sphere::createSphere(const int nDivs, const glm::vec3 color, std::shared_ptr<std::vector<Vertex>> vertices, std::shared_ptr<std::vector<unsigned int>> indices, const bool isDoubled) {
  const int nDivsTheta = nDivs;
  const int nDivsPhi = isDoubled ? 2 * nDivs : nDivs;
  const float deltaTheta = M_PI / (float)(nDivsTheta - 1);
  const float deltaPhi = 2 * M_PI / (float)(nDivsPhi - 1);

  vertices->clear();
  indices->clear();

  vertices->resize((nDivsTheta - 1) * (nDivsPhi - 1) * 6);
  indices->resize((nDivsTheta - 1) * (nDivsPhi - 1) * 6);

  for (int iTheta = 0; iTheta < nDivsTheta - 1; ++iTheta) {
    for (int iPhi = 0; iPhi < nDivsPhi - 1; ++iPhi) {
      const int index = 6 * (iTheta * (nDivsPhi - 1) + iPhi);

      const float theta = (float)iTheta * deltaTheta;
      const float thetaNext = theta + deltaTheta;
      const float phi = (float)iPhi * deltaPhi;
      const float phiNext = phi + deltaPhi;

      const glm::vec3 position0(std::sin(theta) * std::cos(phi), std::sin(theta) * std::sin(phi), std::cos(theta));
      const glm::vec3 position1(std::sin(thetaNext) * std::cos(phi), std::sin(thetaNext) * std::sin(phi), std::cos(thetaNext));
      const glm::vec3 position2(std::sin(thetaNext) * std::cos(phiNext), std::sin(thetaNext) * std::sin(phiNext), std::cos(thetaNext));
      const glm::vec3 position3(std::sin(theta) * std::cos(phiNext), std::sin(theta) * std::sin(phiNext), std::cos(theta));

      Vertex v0(position0, color, position0, glm::vec2(0), -1.0);
      Vertex v1(position1, color, position1, glm::vec2(0), -1.0);
      Vertex v2(position2, color, position2, glm::vec2(0), -1.0);
      Vertex v3(position3, color, position3, glm::vec2(0), -1.0);

      (*vertices)[index + 0] = v0;
      (*indices)[index + 0] = index + 0;
      (*vertices)[index + 1] = v1;
      (*indices)[index + 1] = index + 1;
      (*vertices)[index + 2] = v2;
      (*indices)[index + 2] = index + 2;

      (*vertices)[index + 3] = v2;
      (*indices)[index + 3] = index + 3;
      (*vertices)[index + 4] = v3;
      (*indices)[index + 4] = index + 4;
      (*vertices)[index + 5] = v0;
      (*indices)[index + 5] = index + 5;
    }
  }
}