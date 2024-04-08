#include <SimView/Model/LightBall.hpp>

namespace simview {
namespace model {

using namespace util;
using namespace shader;

LightBall::LightBall() {}

LightBall::~LightBall() {}

void LightBall::initVAO() {
  VertexArray_t vertices = std::make_shared<std::vector<Vertex>>();
  IndexArray_t indices = std::make_shared<std::vector<uint32_t>>();

  Sphere::createSphere(NUM_DIVISIONS, COLOR, vertices, indices);

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

void LightBall::paintGL(
    const TransformationContext &transCtx,  // transCtx
    const LightingContext &lightingCtx,     // lightingCtx
    const RenderingContext &renderingCtx    // renderingCtx
) {
  if (_isVisible) {
    _position = lightingCtx.lightPos;
    const glm::mat4 &mvtMat = transCtx.lightMat * glm::translate(_position);
    const glm::mat4 &mvptMat = transCtx.projMat * mvtMat;
    const glm::mat4 &normMat = glm::transpose(glm::inverse(mvtMat));

    bindShader(
        mvtMat,                                    // mvMat
        mvptMat,                                   // mvpMat
        normMat,                                   // normMat
        transCtx.lightMat,                         // lightMat
        _position,                                 // lightPos
        lightingCtx.shininess,                     // shininess
        lightingCtx.ambientIntensity,              // ambientIntensity
        glm::vec3(0.0f),                           // ambientColor
        glm::vec3(0.0f),                           // diffuseColor
        glm::vec3(0.0f),                           // specularColor
        getRenderType(false, RenderType::NORMAL),  // renderType
        getWireFrameMode(WireFrameMode::OFF),      // wireFrameMode
        renderingCtx.wireFrameColor,               // wireFrameColor
        renderingCtx.wireFrameWidth,               // wireFrameWidth
        renderingCtx.depthTextureId,               // depthTextureId
        glm::mat4(0.0f),                           // lightMvpMat
        false,                                     // isEnabledShadowMapping
        false,                                     // disableDepthTest
        false                                      // isEnabledNormalMap
    );

    drawGL();

    unbindShader();
  }
}

void LightBall::drawGL(const int &index) {
  // Enable VAO
  glBindVertexArray(_vaoId);

  // Draw triangles
  glDrawElements(GL_TRIANGLES, _indexBufferSize, GL_UNSIGNED_INT, 0);

  // Disable VAO
  glBindVertexArray(0);
}

void LightBall::drawAllGL(const glm::mat4 &lightMvpMat) {}

}  // namespace model
}  // namespace simview
