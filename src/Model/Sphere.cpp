#include <Model/Sphere.hpp>

namespace simview {
namespace model {

using namespace util;
using namespace shader;

Sphere::Sphere(const int nDivs,
               const float offsetX,
               const float offsetY,
               const float offsetZ,
               const float scaleX,
               const float scaleY,
               const float scaleZ,
               const glm::vec3 color)
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
  VertexArray_t vertices = std::make_shared<std::vector<Vertex>>();
  IndexArray_t indices = std::make_shared<std::vector<uint32_t>>();

  createSphere(_nDivs, _color, vertices, indices);

  ObjectLoader::moveToOrigin(vertices);
  ObjectLoader::scaleObject(vertices, _scaleX / 2.0f, _scaleY / 2.0f, _scaleZ / 2.0f);
  ObjectLoader::translateObject(vertices, _offsetX, _offsetY, _offsetZ);

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
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, bary));

  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, uv));

  glEnableVertexAttribArray(5);
  glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, id));

  // Create index buffer object
  glGenBuffers(1, &_indexBufferId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufferId);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * indices->size(), indices->data(), GL_STATIC_DRAW);

  _indexBufferSize = (int)indices->size();

  // Temporarily disable VAO
  glBindVertexArray(0);

  glm::vec3 minCoords, maxCoords;
  std::tie(minCoords, maxCoords) = ObjectLoader::getCorners(vertices);
  _bbox = std::make_shared<AxisAlignedBoundingBox>(minCoords, maxCoords);
}

void Sphere::paintGL(const glm::mat4 &mvMat,
                     const glm::mat4 &mvpMat,
                     const glm::mat4 &lightMat,
                     const glm::vec3 &lightPos,
                     const float &shininess,
                     const float &ambientIntensity,
                     const glm::vec3 &wireFrameColor,
                     const float &wireFrameWidth,
                     const GLuint &depthTextureId,
                     const glm::mat4 &lightMvpMat) {
  if (_isVisible) {
    const glm::mat4 &mvtMat = mvMat * glm::translate(_position);
    const glm::mat4 &mvptMat = mvpMat * glm::translate(_position);
    const glm::mat4 &normMat = glm::transpose(glm::inverse(mvtMat));
    const glm::mat4 &lightMvptMat = lightMvpMat * glm::translate(_position);

    paintBBOX(mvtMat, mvptMat, normMat);

    bindShader(
        mvtMat,
        mvptMat,
        normMat,
        lightMat,
        lightPos,
        shininess,
        ambientIntensity,
        glm::vec3(0.0f),
        glm::vec3(0.0f),
        glm::vec3(0.0f),
        getRenderType(),
        getWireFrameMode(),
        wireFrameColor,
        wireFrameWidth,
        depthTextureId,
        lightMvptMat);

    drawGL();

    unbindShader();
  }
}

void Sphere::drawGL(const int &index) {
  // Enable VAO
  glBindVertexArray(_vaoId);

  // Draw triangles
  glDrawElements(GL_TRIANGLES, _indexBufferSize, GL_UNSIGNED_INT, 0);

  // Disable VAO
  glBindVertexArray(0);
}

void Sphere::drawAllGL(const glm::mat4 &lightMvpMat) {
  if (_isVisible) {
    const glm::mat4 &lightMvptMat = lightMvpMat * glm::translate(_position);
    _depthShader->setUniformVariable(DefaultDepthShader::UNIFORM_NAME_LIGHT_MVP_MAT, lightMvptMat);

    drawGL();
  }
}

void Sphere::createSphere(const int nDivs,
                          VertexArray_t vertices,
                          IndexArray_t indices,
                          const bool isDoubled) {
  createSphere(nDivs, glm::vec3(0.0f), vertices, indices, isDoubled);
}

void Sphere::createSphere(const int nDivs,
                          const glm::vec3 color,
                          VertexArray_t vertices,
                          IndexArray_t indices,
                          const bool isDoubled) {
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

      Vertex v00(position0, color, position0, BARY_CENTER[0], glm::vec2(0), -1.0);
      Vertex v01(position1, color, position1, BARY_CENTER[1], glm::vec2(0), -1.0);
      Vertex v02(position2, color, position2, BARY_CENTER[2], glm::vec2(0), -1.0);

      Vertex v12(position2, color, position2, BARY_CENTER[0], glm::vec2(0), -1.0);
      Vertex v13(position3, color, position3, BARY_CENTER[1], glm::vec2(0), -1.0);
      Vertex v10(position0, color, position0, BARY_CENTER[2], glm::vec2(0), -1.0);

      (*vertices)[index + 0] = v00;
      (*indices)[index + 0] = index + 0;
      (*vertices)[index + 1] = v01;
      (*indices)[index + 1] = index + 1;
      (*vertices)[index + 2] = v02;
      (*indices)[index + 2] = index + 2;

      (*vertices)[index + 3] = v12;
      (*indices)[index + 3] = index + 3;
      (*vertices)[index + 4] = v13;
      (*indices)[index + 4] = index + 4;
      (*vertices)[index + 5] = v10;
      (*indices)[index + 5] = index + 5;
    }
  }
}

}  // namespace model
}  // namespace simview