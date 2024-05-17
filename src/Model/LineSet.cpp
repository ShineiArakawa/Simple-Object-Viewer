#include <SimView/Model/LineSet.hpp>

namespace simview {
namespace model {

using namespace util;
using namespace shader;

LineSet::LineSet(const std::shared_ptr<std::vector<vec3f_t>> positions,  // positions
                 const glm::vec3 color,                                  // color
                 const float lineWidth                                   // lineWidth
                 )
    : Primitive(),
      _positions(positions),
      _lineWidth(lineWidth),
      _color(color) {
}

LineSet::~LineSet() = default;

void LineSet::initVAO() {
  VertexArray_t vertices = std::make_shared<std::vector<Vertex>>();
  IndexArray_t indices = std::make_shared<std::vector<uint32_t>>();

  const size_t nPositions = _positions->size();
  vertices->resize(nPositions);
  indices->resize(nPositions);

  for (size_t i = 0; i < nPositions; ++i) {
    const Vertex vertex(glm::vec3((*_positions)[i][0], (*_positions)[i][1], (*_positions)[i][2]),
                        _color,
                        glm::vec3(0.0f),
                        glm::vec3(0.0f),
                        glm::vec2(0.0f),
                        0.0f);
    (*vertices)[i] = vertex;
    (*indices)[i] = i;
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
}

void LineSet::paintGL(
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
        getWireFrameMode(Primitive::WireFrameMode::OFF),     // wireFrameMode
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

void LineSet::drawGL(const int &index) {
  glBindVertexArray(_vaoId);

  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_BLEND);

  glDrawElements(GL_LINES, _indexBufferSize, GL_UNSIGNED_INT, 0);

  glDisable(GL_LINE_SMOOTH);
  glDisable(GL_BLEND);

  glBindVertexArray(0);
}

void LineSet::drawAllGL(const glm::mat4 &lightMvpMat) {
  // Do nothing
}

}  // namespace model
}  // namespace simview