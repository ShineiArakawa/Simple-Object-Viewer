#include <Model/Object.hpp>
#include <Util/Texture.hpp>

Object::Object(const std::string &filePath, const float offsetX, const float offsetY, const float offsetZ, const float scale) {
  _filePath = filePath;
  _offsetX = offsetX;
  _offsetY = offsetY;
  _offsetZ = offsetZ;
  _scale = scale;
}

Object::~Object() {}

void Object::initVAO() {
  std::shared_ptr<std::vector<Vertex>> vertices = std::make_shared<std::vector<Vertex>>();
  std::shared_ptr<std::vector<uint32_t>> indices = std::make_shared<std::vector<uint32_t>>();

  ObjectLoader::readFromFile(_filePath,
                             vertices,
                             indices,
                             _offsetX,
                             _offsetY,
                             _offsetZ);
  ObjectLoader::scaleObject(vertices, _scale);

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
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices->size(), indices->data(), GL_STATIC_DRAW);

  _indexBufferSize = (int)indices->size();

  // Temporarily disable VAO
  glBindVertexArray(0);
}

void Object::paintGL(const glm::mat4 &mvMat,
                     const glm::mat4 &mvpMat,
                     const glm::mat4 &normMat,
                     const glm::mat4 &lightMat,
                     const glm::vec3 &lightPos,
                     const float &shininess,
                     const float &ambientIntensity,
                     const glm::vec3 &wireFrameColor,
                     const float &wireFrameWidth) {
  if (_isVisible) {
    const glm::mat4 &mvtMat = mvMat * glm::translate(_position);
    const glm::mat4 &mvptMat = mvpMat * glm::translate(_position);

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
        false,
        _isEnabledNormalMap);

    {
      // Activate texture image
      GLuint uid;
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, _textureId);
      uid = glGetUniformLocation(_shaderID, UNIFORM_NAME_DIFFUSE_TEXTURE);
      glUniform1i(uid, 0);

      uid = glGetUniformLocation(_shaderID, "u_hasDiffuseTexture");
      glUniform1f(uid, 1.0f);
    }

    {
      // Activate normal map
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, _normalMapId);
      const GLuint uid = glGetUniformLocation(_shaderID, UNIFORM_NAME_NORMAL_MAP);
      glUniform1i(uid, 1);
    }

    glBindVertexArray(_vaoId);
    glDrawElements(GL_TRIANGLES, _indexBufferSize, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_2D, 0);
    unbindShader();
  }
}

void Object::loadTexture(const std::string &filePath) {
  Texture::loadTexture(filePath, _textureId);
  // LOG_INFO("_textureId=" + std::to_string(_textureId));
}

void Object::loadNormalMap(const std::string &filePath) {
  Texture::loadTexture(filePath, _normalMapId);
  // LOG_INFO("_normalMapId=" + std::to_string(_normalMapId));
}
