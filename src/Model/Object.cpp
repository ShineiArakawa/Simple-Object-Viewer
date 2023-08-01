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

  ObjectLoader::readObjFile(_filePath, vertices, indices, _offsetX, _offsetY, _offsetZ);
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

void Object::paintGL(const glm::mat4 &mvpMat) {
  GLuint uid;

  glUseProgram(_shaderID);

  {
    GLuint mvpMatLocId = glGetUniformLocation(_shaderID, "u_mvpMat");
    glUniformMatrix4fv(mvpMatLocId, 1, GL_FALSE, glm::value_ptr(mvpMat));

    uid = glGetUniformLocation(_shaderID, "u_toUseTexture");
    glUniform1f(uid, getRenderType());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _textureId);
    uid = glGetUniformLocation(_shaderID, "u_texture");
    glUniform1i(uid, 0);

    glBindVertexArray(_vaoId);
    glDrawElements(GL_TRIANGLES, _indexBufferSize, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
  }

  glUseProgram(0);
}

void Object::loadTexture(const std::string &filePath) { Texture::loadTexture(filePath, _textureId); }
