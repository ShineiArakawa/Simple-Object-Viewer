#include <SimView/Model/GridPlane.hpp>

namespace simview {
namespace model {

GridPlane::GridPlane(const int& nDivs,
                     const glm::vec2& minCoords,
                     const glm::vec2& maxCoords)
    : Primitive(),
      _nDivs(nDivs),
      _minCoords(minCoords),
      _maxCoords(maxCoords) {}

GridPlane::~GridPlane() = default;

void GridPlane::initVAO() {
  VertexArray_t vertices = std::make_shared<std::vector<Vertex>>();
  IndexArray_t indices = std::make_shared<std::vector<uint32_t>>();
  int idx = 0;

  const glm::vec3 color = glm::vec3(1.0f);
  const glm::vec3 normal = glm::vec3(0.0f);
  const glm::vec3 bary = glm::vec3(0.0f);
  const glm::vec2 uv = glm::vec2(0.0f);
  const glm::vec2 stride = {(_maxCoords.x - _minCoords.x) / (float)_nDivs,
                            (_maxCoords.y - _minCoords.y) / (float)_nDivs};

  // For x
  for (int indexX = 0; indexX < _nDivs + 1; ++indexX) {
    const float xCoord = _minCoords.x + stride.x * (float)indexX;

    Vertex v0(glm::vec3(xCoord, _minCoords.y, 0.0f),
              color,
              normal,
              bary,
              uv,
              0.0f);
    vertices->push_back(v0);
    indices->push_back(idx++);

    Vertex v1(glm::vec3(xCoord, _maxCoords.y, 0.0f),
              color,
              normal,
              bary,
              uv,
              0.0f);
    vertices->push_back(v1);
    indices->push_back(idx++);
  }

  // For y
  for (int indexY = 0; indexY < _nDivs + 1; ++indexY) {
    const float yCoord = _minCoords.y + stride.y * (float)indexY;

    Vertex v0(glm::vec3(_minCoords.x, yCoord, 0.0f),
              color,
              normal,
              bary,
              uv,
              0.0f);
    vertices->push_back(v0);
    indices->push_back(idx++);

    Vertex v1(glm::vec3(_maxCoords.x, yCoord, 0.0f),
              color,
              normal,
              bary,
              uv,
              0.0f);
    vertices->push_back(v1);
    indices->push_back(idx++);
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

  _indexBufferSize = (int)indices->size();

  // Temporarily disable VAO
  glBindVertexArray(0);
}

void GridPlane::paintGL(
    const TransformationContext& transCtx,  // transCtx
    const LightingContext& lightingCtx,     // lightingCtx
    const RenderingContext& renderingCtx    // renderingCtx
) {
  if (_isVisible) {
    const glm::mat4& mvtMat = transCtx.mvMat * glm::translate(_position);
    const glm::mat4& mvptMat = transCtx.mvpMat * glm::translate(_position);
    const glm::mat4& normMat = glm::transpose(glm::inverse(mvtMat));
    const glm::mat4& lightMvptMat = transCtx.lightMvpMat * glm::translate(_position);

    bindShader(
        mvtMat,                                              // mvMat
        mvptMat,                                             // mvpMat
        normMat,                                             // normMat
        transCtx.lightMat,                                   // lightMat
        lightingCtx.lightPos,                                // lightPos
        lightingCtx.shininess,                               // shininess
        lightingCtx.ambientIntensity,                        // ambientIntensity
        glm::vec3(0.0f),                                     // ambientColor
        glm::vec3(0.0f),                                     // diffuseColor
        glm::vec3(0.0f),                                     // specularColor
        getRenderType(false, Primitive::RenderType::COLOR),  // renderType
        renderingCtx.wireFrameColor,                         // wireFrameColor
        renderingCtx.wireFrameWidth,                         // wireFrameWidth
        renderingCtx.depthTextureId,                         // depthTextureId
        lightMvptMat,                                        // lightMvpMat
        false,                                               // isEnabledShadowMapping
        false,                                               // disableDepthTest
        false                                                // isEnabledNormalMap
    );

    drawGL();

    unbindShader();
  }
}

void GridPlane::drawGL(const int& index) {
  glBindVertexArray(_vaoId);

  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_BLEND);

  glDrawElements(GL_LINES, _indexBufferSize, GL_UNSIGNED_INT, 0);

  glDisable(GL_LINE_SMOOTH);
  glDisable(GL_BLEND);

  glBindVertexArray(0);
}

void GridPlane::drawAllGL(const glm::mat4& lightMvpMat) {
  // Disabled
  // if (_isVisible) {
  //   drawGL();
  // }
}

}  // namespace model
}  // namespace simview