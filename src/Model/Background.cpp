#include <Model/Background.hpp>

Background::Background(const std::string& filePath) {
  Texture::loadTexture(filePath, _textureId);
  setDefaultRenderType(RenderType::TEXTURE);
}

Background::~Background() {}

void Background::initVAO() {
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  int idx = 0;

  for (int j = 0; j < 3; j++) {
    Vertex v(positions[j], glm::vec3(0), glm::vec3(0), uvCoords[j], -1.0);
    vertices.push_back(v);
    indices.push_back(idx++);
  }

  for (int j = 0; j < 3; j++) {
    Vertex v(positions[j + 1], glm::vec3(0), glm::vec3(0), uvCoords[j + 1], -1.0);
    vertices.push_back(v);
    indices.push_back(idx++);
  }

  glGenVertexArrays(1, &_vaoId);
  glBindVertexArray(_vaoId);

  glGenBuffers(1, &_vertexBufferId);
  glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferId);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

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

  glGenBuffers(1, &_indexBufferId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufferId);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);

  glBindVertexArray(0);
}

void Background::paintGL(const glm::mat4& mvpMat) {
  GLuint uid;

  glUseProgram(_shaderID);
  glDisable(GL_DEPTH_TEST);

  uid = glGetUniformLocation(_shaderID, "u_mvpMat");
  glUniformMatrix4fv(uid, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));

  uid = glGetUniformLocation(_shaderID, "u_toUseTexture");
  glUniform1f(uid, getRenderType());

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _textureId);
  uid = glGetUniformLocation(_shaderID, "u_texture");
  glUniform1i(uid, 0);

  glBindVertexArray(_vaoId);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);

  glEnable(GL_DEPTH_TEST);
  glUseProgram(0);
}