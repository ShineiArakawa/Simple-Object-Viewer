#include <Model/Box.hpp>
#include <iostream>

namespace simview {
namespace model {

using namespace util;
using namespace shader;

Box::Box(const float offsetX,
         const float offsetY,
         const float offsetZ,
         const float scaleX,
         const float scaleY,
         const float scaleZ) {
  _offsetX = offsetX;
  _offsetY = offsetY;
  _offsetZ = offsetZ;
  _scaleX = scaleX;
  _scaleY = scaleY;
  _scaleZ = scaleZ;
}

Box::~Box() = default;

void Box::initVAO() {
  // Create vertex array
  VertexArray_t vertices = std::make_shared<std::vector<Vertex>>();
  IndexArray_t indices = std::make_shared<std::vector<uint32_t>>();
  int idx = 0;

  for (int i = 0; i < 6; i++) {
    for (int j = 0; j < 3; j++) {
      glm::vec3 pos = positions[faces[i * 2 + 0][j]];

      Vertex v(pos, colors[i], normals[i], BARY_CENTER[j], uvKeypointCoords[faceToUVKeypointIndex[i * 2 + 0][j]], 0.0f);
      vertices->push_back(v);
      indices->push_back(idx++);
    }

    for (int j = 0; j < 3; j++) {
      glm::vec3 pos = positions[faces[i * 2 + 1][j]];

      Vertex v(pos, colors[i], normals[i], BARY_CENTER[j], uvKeypointCoords[faceToUVKeypointIndex[i * 2 + 1][j]], 0.0f);
      vertices->push_back(v);
      indices->push_back(idx++);
    }
  }

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

void Box::paintGL(const glm::mat4& mvMat,
                  const glm::mat4& mvpMat,
                  const glm::mat4& lightMat,
                  const glm::vec3& lightPos,
                  const float& shininess,
                  const float& ambientIntensity,
                  const glm::vec3& wireFrameColor,
                  const float& wireFrameWidth,
                  const GLuint& depthTextureId,
                  const glm::mat4& lightMvpMat) {
  if (_isVisible) {
    const glm::mat4& mvtMat = mvMat * glm::translate(_position);
    const glm::mat4& mvptMat = mvpMat * glm::translate(_position);
    const glm::mat4& normMat = glm::transpose(glm::inverse(mvtMat));
    const glm::mat4& lightMvptMat = lightMvpMat * glm::translate(_position);

    bindShader(
        mvtMat,                   // mvMat
        mvptMat,                  // mvpMat
        normMat,                  // normMat
        lightMat,                 // lightMat
        lightPos,                 // lightPos
        shininess,                // shininess
        ambientIntensity,         // ambientIntensity
        glm::vec3(0.0f),          // ambientColor
        glm::vec3(0.0f),          // diffuseColor
        glm::vec3(0.0f),          // specularColor
        getRenderType(),          // renderType
        getWireFrameMode(),       // wireFrameMode
        wireFrameColor,           // wireFrameColor
        wireFrameWidth,           // wireFrameWidth
        depthTextureId,           // depthTextureId
        lightMvptMat,             // lightMvpMat
        _isEnabledShadowMapping,  // isEnabledShadowMapping
        false,                    // disableDepthTest
        false                     // isEnabledNormalMap
    );

    _shader->setUniformTexture(DefaultModelShader::UNIFORM_NAME_AMBIENT_TEXTURE, _textureId);
    _shader->setUniformVariable(DefaultModelShader::UNIFORM_NAME_AMBIENT_TEXTURE_FLAG, true);

    _shader->setUniformTexture(DefaultModelShader::UNIFORM_NAME_DIFFUSE_TEXTURE, _textureId);
    _shader->setUniformVariable(DefaultModelShader::UNIFORM_NAME_DIFFUSE_TEXTURE_FLAG, true);

    drawGL();

    unbindShader();
  }
}

void Box::drawGL(const int& index) {
  // Enable VAO
  glBindVertexArray(_vaoId);

  // Draw triangles
  glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

  // Disable VAO
  glBindVertexArray(0);
}

void Box::drawAllGL(const glm::mat4& lightMvpMat) {
  if (_isVisible) {
    const glm::mat4& lightMvptMat = lightMvpMat * glm::translate(_position);
    _depthShader->setUniformVariable(DefaultDepthShader::UNIFORM_NAME_LIGHT_MVP_MAT, lightMvptMat);

    drawGL();
  }
}

void Box::loadTexture(const std::string& filePath) {
  Texture::loadTexture(filePath, _textureId);
}

}  // namespace model
}  // namespace simview