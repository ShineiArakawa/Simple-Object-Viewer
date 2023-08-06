#include <Model/Terrain.hpp>
#include <Util/Texture.hpp>

Terrain::Terrain(const std::string &filePath, const float offsetX, const float offsetY, const float offsetZ, const float scaleX, const float scaleY,
                 const float scaleH) {
  _filePath = filePath;
  _offsetX = offsetX;
  _offsetY = offsetY;
  _offsetZ = offsetZ;
  _scaleX = scaleX;
  _scaleY = scaleY;
  _scaleH = scaleH;
}

Terrain::~Terrain() {}

void Terrain::initVAO() {
  Texture::TextureArray heightMap = std::make_shared<Texture::InnerTextureArray>();
  Texture::readTexture(_filePath, heightMap);

  int height = heightMap->size();
  int width = (*heightMap)[0]->size();
  int nChannels = (*(*heightMap)[0])[0]->size();

  float diffH = 1.0f / (float)height;
  float diffW = 1.0f / (float)width;

  // Create vertex array
  std::shared_ptr<std::vector<Vertex>> vertices = std::make_shared<std::vector<Vertex>>();
  std::vector<unsigned int> indices;
  int idx = 0;

  for (int h_texture = 0; h_texture < height - 1; h_texture++) {
    for (int w_texture = 0; w_texture < width - 1; w_texture++) {
      for (int i_vert = 0; i_vert < 3; i_vert++) {
        int h_texture_offset = 0;
        int w_texture_offset = 0;
        if (i_vert == 1) {
          h_texture_offset = 1;
        } else if (i_vert == 2) {
          w_texture_offset = 1;
        }

        glm::vec3 pos(0.0f);
        pos.x = positions[i_vert].x * diffH + (float)h_texture * diffH;
        pos.y = (float)(*(*(*heightMap)[h_texture + h_texture_offset])[w_texture + w_texture_offset])[0] / 255.0f;
        pos.z = positions[i_vert].z * diffW + (float)w_texture * diffW;

        Vertex v(pos, colors[i_vert], colors[i_vert], textureCoords[i_vert], 0.0f);
        vertices->push_back(v);
        indices.push_back(idx++);
      }

      for (int i_vert = 1; i_vert < 4; i_vert++) {
        int h_texture_offset = 0;
        int w_texture_offset = 0;
        if (i_vert == 1) {
          h_texture_offset = 1;
        } else if (i_vert == 2) {
          w_texture_offset = 1;
        } else {
          h_texture_offset = 1;
          w_texture_offset = 1;
        }

        glm::vec3 pos(0.0f);
        pos.x = positions[i_vert].x * diffH + (float)h_texture * diffH;
        pos.y = (float)(*(*(*heightMap)[h_texture + h_texture_offset])[w_texture + w_texture_offset])[0] / 255.0f;
        pos.z = positions[i_vert].z * diffW + (float)w_texture * diffW;

        Vertex v(pos, colors[i_vert], colors[i_vert], textureCoords[i_vert], 0.0f);
        vertices->push_back(v);
        indices.push_back(idx++);
      }
    }
  }

  std::cout << "### Initialized terrain with " << vertices->size() / 3 << " polygons" << std::endl;

  ObjectLoader::moveToOrigin(vertices);
  ObjectLoader::scaleObject(vertices, _scaleX, _scaleH, _scaleY);
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

  _indexBufferSize = (int)indices.size();

  // Temporarily disable VAO
  glBindVertexArray(0);
}

void Terrain::paintGL(const glm::mat4 &mvpMat) {
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
  glDrawElements(GL_TRIANGLES, _indexBufferSize, GL_UNSIGNED_INT, 0);

  // Disable VAO
  glBindVertexArray(0);

  // Disable shader program
  glUseProgram(0);
}