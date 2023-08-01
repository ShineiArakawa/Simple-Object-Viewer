#include <Model/Box.hpp>
#include <iostream>

Box::Box(const float offsetX, const float offsetY, const float offsetZ) {
  _offsetX = offsetX;
  _offsetY = offsetY;
  _offsetZ = offsetZ;
}

Box::~Box() {}

void Box::initVAO() {
  // Create vertex array
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  int idx = 0;
  glm::vec3 offset(_offsetX, _offsetY, _offsetZ);

  for (int i = 0; i < 6; i++) {
    for (int j = 0; j < 3; j++) {
      glm::vec3 pos = positions[faces[i * 2 + 0][j]];

      Vertex v(pos + offset, colors[i], colors[i], textureCoords[faces[i * 2 + 0][j]], 0.0f);
      vertices.push_back(v);
      indices.push_back(idx++);
    }

    for (int j = 0; j < 3; j++) {
      glm::vec3 pos = positions[faces[i * 2 + 1][j]];

      Vertex v(pos + offset, colors[i], colors[i], textureCoords[faces[i * 2 + 0][j]], 0.0f);
      vertices.push_back(v);
      indices.push_back(idx++);
    }
  }

  // Create VAO
  glGenVertexArrays(1, &_vaoId);
  glBindVertexArray(_vaoId);

  // Create vertex buffer object
  glGenBuffers(1, &_vertexBufferId);
  glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferId);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

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
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);

  // Temporarily disable VAO
  glBindVertexArray(0);
}

void Box::paintGL(const glm::mat4 &mvpMat) {
  GLuint uid;

  // Enable shader program
  glUseProgram(_shaderID);

  // Transfer uniform variables
  GLuint mvpMatLocId = glGetUniformLocation(_shaderID, "u_mvpMat");
  glUniformMatrix4fv(mvpMatLocId, 1, GL_FALSE, glm::value_ptr(mvpMat));

  uid = glGetUniformLocation(_shaderID, "u_toUseTexture");
  glUniform1f(uid, getRenderType());

  // Enable VAO
  glBindVertexArray(_vaoId);

  // Draw triangles
  glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

  // Disable VAO
  glBindVertexArray(0);

  // Disable shader program
  glUseProgram(0);
}