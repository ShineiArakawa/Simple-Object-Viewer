#include <SimView/Model/Background.hpp>

namespace simview {
namespace model {

using namespace util;
using namespace shader;

Background::Background(const GLuint& textureId)
    : Primitive(),
      _isLoadedTexture(true),
      _textureId(textureId),
      _textureFilePath("") {
  setDefaultRenderType(RenderType::TEXTURE);
}

Background::Background(const std::string& filePath)
    : Primitive(),
      _isLoadedTexture(false),
      _textureFilePath(filePath) {
  setDefaultRenderType(RenderType::TEXTURE);
}

Background::~Background() {}

void Background::initVAO() {
  VertexArray_t vertices = std::make_shared<std::vector<Vertex>>();
  IndexArray_t indices = std::make_shared<std::vector<uint32_t>>();
  int idx = 0;

  for (int j = 0; j < 3; j++) {
    Vertex v(positions[j], glm::vec3(0), glm::vec3(0), BARY_CENTER[j], uvCoords[j], -1.0);
    vertices->push_back(v);
    indices->push_back(idx++);
  }

  for (int j = 0; j < 3; j++) {
    Vertex v(positions[j + 1], glm::vec3(0), glm::vec3(0), BARY_CENTER[j], uvCoords[j + 1], -1.0);
    vertices->push_back(v);
    indices->push_back(idx++);
  }

  glGenVertexArrays(1, &_vaoId);
  glBindVertexArray(_vaoId);

  glGenBuffers(1, &_vertexBufferId);
  glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferId);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices->size(), vertices->data(), GL_STATIC_DRAW);

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

  glGenBuffers(1, &_indexBufferId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufferId);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * indices->size(), indices->data(), GL_STATIC_DRAW);

  glBindVertexArray(0);

  // Load Texture
  if (!_isLoadedTexture) {
    Texture::loadTexture(_textureFilePath, _textureId);
  }
}

void Background::paintGL(
    const TransformationContext& transCtx,  // transCtx
    const LightingContext& lightingCtx,     // lightingCtx
    const RenderingContext& renderingCtx    // renderingCtx
) {
  if (_isVisible) {
    bindShader(
        glm::mat4(1.0f),                                       // mvMat
        glm::mat4(1.0f),                                       // mvpMat
        glm::mat4(1.0f),                                       // normMat
        transCtx.lightMat,                                     // lightMat
        lightingCtx.lightPos,                                  // lightPos
        lightingCtx.shininess,                                 // shininess
        lightingCtx.ambientIntensity,                          // ambientIntensity
        glm::vec3(0.0f),                                       // ambientColor
        glm::vec3(0.0f),                                       // diffuseColor
        glm::vec3(0.0f),                                       // specularColor
        getRenderType(false, Primitive::RenderType::TEXTURE),  // renderType
        renderingCtx.wireFrameColor,                           // wireFrameColor
        renderingCtx.wireFrameWidth,                           // wireFrameWidth
        renderingCtx.depthTextureId,                           // depthTextureId
        transCtx.lightMvpMat,                                  // lightMvpMat
        false,                                                 // isEnabledShadowMapping
        true,                                                  // disableDepthTest
        false                                                  // isEnabledNormalMap
    );

    _shader->setUniformTexture(DefaultModelShader::UNIFORM_NAME_DIFFUSE_TEXTURE, _textureId);
    _shader->setUniformVariable(DefaultModelShader::UNIFORM_NAME_DIFFUSE_TEXTURE_FLAG, 1.0f);

    drawGL();

    unbindShader();
  }
}

void Background::drawGL(const int& index) {
  glBindVertexArray(_vaoId);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

void Background::drawAllGL(const glm::mat4& lightMvpMat) {
  if (_isVisible) {
    drawGL();
  }
}

}  // namespace model
}  // namespace simview