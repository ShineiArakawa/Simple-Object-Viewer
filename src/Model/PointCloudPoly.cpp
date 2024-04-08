#include <SimView/Model/PointCloudPoly.hpp>

namespace simview {
namespace model {

using namespace util;
using namespace shader;

PointCloudPoly::PointCloudPoly(const std::string &filePath,  // filePath
                               const float offsetX,          // offsetX
                               const float offsetY,          // offsetY
                               const float offsetZ,          // offsetZ
                               const float scale,            // scale
                               const float pointSize,        // pointSize
                               const bool isDoubled          // isDoubled
                               )
    : _filePath(filePath),
      _offsetX(offsetX),
      _offsetY(offsetY),
      _offsetZ(offsetZ),
      _scale(scale),
      _pointSize(pointSize),
      _isDoubled(isDoubled) {
}

void PointCloudPoly::initVAO() {
  VertexArray_t points = std::make_shared<std::vector<Vertex>>();
  IndexArray_t pointsIndices = std::make_shared<std::vector<uint32_t>>();

  ObjectLoader::readFromFile(_filePath, points, pointsIndices, _offsetX, _offsetY, _offsetZ);
  ObjectLoader::moveToOrigin(points);
  ObjectLoader::scaleObject(points, _scale);
  ObjectLoader::translateObject(points, _offsetX, _offsetY, _offsetZ);

  VertexArray_t vertices = std::make_shared<std::vector<Vertex>>();
  IndexArray_t indices = std::make_shared<std::vector<uint32_t>>();

  int index = 0;

  vertices->resize((NUM_DIVISIONS - 1) * (2 * NUM_DIVISIONS - 1) * 6 * (int)points->size());
  indices->resize((NUM_DIVISIONS - 1) * (2 * NUM_DIVISIONS - 1) * 6 * (int)points->size());

  VertexArray_t primitiveVertices = std::make_shared<std::vector<Vertex>>();
  IndexArray_t primitiveIndices = std::make_shared<std::vector<uint32_t>>();
  {
    Sphere::createSphere(NUM_DIVISIONS, primitiveVertices, primitiveIndices, _isDoubled);
    ObjectLoader::moveToOrigin(primitiveVertices);
    ObjectLoader::scaleObject(primitiveVertices, _pointSize);
  }

  for (int iPoint = 0; iPoint < (int)points->size(); ++iPoint) {
    const Vertex point = (*points)[iPoint];

    VertexArray_t iVertices = std::make_shared<std::vector<Vertex>>(*primitiveVertices);
    IndexArray_t iIndices = std::make_shared<std::vector<uint32_t>>(*primitiveIndices);

    ObjectLoader::translateObject(iVertices, point.position[0], point.position[1], point.position[2]);

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

void PointCloudPoly::paintGL(
    const TransformationContext &transCtx,  // transCtx
    const LightingContext &lightingCtx,     // lightingCtx
    const RenderingContext &renderingCtx    // renderingCtx
) {
  if (_isVisible) {
    const glm::mat4 &mvtMat = transCtx.mvMat * glm::translate(_position);
    const glm::mat4 &mvptMat = transCtx.mvpMat * glm::translate(_position);
    const glm::mat4 &normMat = glm::transpose(glm::inverse(mvtMat));
    const glm::mat4 &lightMvptMat = transCtx.lightMvpMat * glm::translate(_position);

    paintBBOX(mvtMat, mvptMat, normMat);

    bindShader(
        mvtMat,                        // mvMat
        mvptMat,                       // mvpMat
        normMat,                       // normMat
        transCtx.lightMat,             // lightMat
        lightingCtx.lightPos,          // lightPos
        lightingCtx.shininess,         // shininess
        lightingCtx.ambientIntensity,  // ambientIntensity
        glm::vec3(0.0f),               // ambientColor
        glm::vec3(0.0f),               // diffuseColor
        glm::vec3(0.0f),               // specularColor
        getRenderType(),               // renderType
        getWireFrameMode(),            // wireFrameMode
        renderingCtx.wireFrameColor,   // wireFrameColor
        renderingCtx.wireFrameWidth,   // wireFrameWidth
        renderingCtx.depthTextureId,   // depthTextureId
        lightMvptMat,                  // lightMvpMat
        _isEnabledShadowMapping,       // isEnabledShadowMapping
        false,                         // disableDepthTest
        false                          // isEnabledNormalMap
    );

    drawGL();

    unbindShader();
  }
}

void PointCloudPoly::drawGL(const int &index) {
  // Enable VAO
  glBindVertexArray(_vaoId);

  // Draw triangles
  glDrawElements(GL_TRIANGLES, _indexBufferSize, GL_UNSIGNED_INT, 0);

  // Disable VAO
  glBindVertexArray(0);
}

void PointCloudPoly::drawAllGL(const glm::mat4 &lightMvpMat) {
  if (_isVisible) {
    const glm::mat4 &lightMvptMat = lightMvpMat * glm::translate(_position);
    _depthShader->setUniformVariable(DefaultDepthShader::UNIFORM_NAME_LIGHT_MVP_MAT, lightMvptMat);

    drawGL();
  }
}

}  // namespace model
}  // namespace simview