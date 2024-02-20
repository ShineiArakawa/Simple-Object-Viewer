#include <Model/Box.hpp>
#include <iostream>

Box::Box(const float offsetX, const float offsetY, const float offsetZ, const float scaleX, const float scaleY, const float scaleZ) {
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
  std::shared_ptr<std::vector<Vertex>> vertices = std::make_shared<std::vector<Vertex>>();
  std::vector<unsigned int> indices;
  int idx = 0;

  for (int i = 0; i < 6; i++) {
    for (int j = 0; j < 3; j++) {
      glm::vec3 pos = positions[faces[i * 2 + 0][j]];

      Vertex v(pos, colors[i], normals[i], uvKeypointCoords[faceToUVKeypointIndex[i * 2 + 0][j]], 0.0f);
      vertices->push_back(v);
      indices.push_back(idx++);
    }

    for (int j = 0; j < 3; j++) {
      glm::vec3 pos = positions[faces[i * 2 + 1][j]];

      Vertex v(pos, colors[i], normals[i], uvKeypointCoords[faceToUVKeypointIndex[i * 2 + 1][j]], 0.0f);
      vertices->push_back(v);
      indices.push_back(idx++);
    }
  }

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
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);

  // Temporarily disable VAO
  glBindVertexArray(0);
}

void Box::paintGL(const glm::mat4& mvMat,
                  const glm::mat4& mvpMat,
                  const glm::mat4& normMat,
                  const glm::mat4& lightMat,
                  const glm::vec3& lightPos,
                  const float& shininess,
                  const float& ambientIntensity) {
  if (_isVisible) {
    const glm::mat4& mvtMat = mvMat * glm::translate(_position);
    const glm::mat4& mvptMat = mvpMat * glm::translate(_position);

    bindShader(mvtMat, mvptMat, normMat, lightMat, lightPos, shininess, ambientIntensity, getRenderType());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _textureId);
    const GLuint uid = glGetUniformLocation(_shaderID, "u_texture");
    glUniform1i(uid, 0);

    // Enable VAO
    glBindVertexArray(_vaoId);

    // Draw triangles
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Disable VAO
    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_2D, 0);

    unbindShader();
  }
}

void Box::loadTexture(const std::string& filePath) { Texture::loadTexture(filePath, _textureId); }
