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
  const auto startTime = std::chrono::system_clock::now();

  Texture::TextureArray heightMap = std::make_shared<Texture::InnerTextureArray>();
  Texture::readTexture(_filePath, heightMap);

  const int height = heightMap->size();
  const int width = (*heightMap)[0]->size();

  const float diffH = 1.0f / (float)height;
  const float diffW = 1.0f / (float)width;

  // Create vertex array
  std::shared_ptr<std::vector<Vertex>> vertices = std::make_shared<std::vector<Vertex>>();
  std::vector<unsigned int> indices;
  vertices->resize((height - 1) * (width - 1) * 6);
  indices.resize((height - 1) * (width - 1) * 6);

#pragma omp parallel for
  for (int h_texture = 0; h_texture < height - 1; h_texture++) {
    for (int w_texture = 0; w_texture < width - 1; w_texture++) {
      const int index = 6 * (h_texture * (width - 1) + w_texture);

      std::array<glm::vec3, 4> points;
      for (int i_point = 0; i_point < 4; i_point++) {
        const int h_texture_offset = (i_point == 2 || i_point == 3) ? 1 : 0;
        const int w_texture_offset = (i_point == 1 || i_point == 2) ? 1 : 0;

        points[i_point] = glm::vec3(
            positions[i_point].x * diffH + (float)h_texture * diffH,
            (float)(*(*(*heightMap)[h_texture + h_texture_offset])[w_texture + w_texture_offset])[0] / 255.0f,
            positions[i_point].z * diffW + (float)w_texture * diffW);
      }

      std::array<glm::vec3, 2> normals;
      for (int i_normal = 0; i_normal < 2; i_normal++) {
        const auto &relvec0 = points[faces[i_normal][1]] - points[faces[i_normal][0]];
        const auto &relvec1 = points[faces[i_normal][2]] - points[faces[i_normal][1]];
        const auto normal = glm::normalize(glm::cross(relvec0, relvec1));
        normals[i_normal] = normal;
      }

      for (int i_vert = 0; i_vert < 3; i_vert++) {
        Vertex v(points[faces[0][i_vert]], colors[i_vert], normals[0], BARY_CENTER[i_vert], textureCoords[i_vert], 0.0f);

        (*vertices)[index + i_vert] = v;
        indices[index + i_vert] = index + i_vert;
      }

      for (int i_vert = 0; i_vert < 3; i_vert++) {
        Vertex v(points[faces[1][i_vert]], colors[i_vert], normals[1], BARY_CENTER[i_vert], textureCoords[i_vert], 0.0f);

        (*vertices)[index + i_vert + 3] = v;
        indices[index + i_vert + 3] = index + i_vert + 3;
      }
    }
  }

  std::cout << "### Initialized terrain with " << vertices->size() / 3 << " polygons" << std::endl;

  ObjectLoader::moveToOrigin(vertices);
  ObjectLoader::scaleObject(vertices, _scaleX, _scaleH, _scaleY);
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
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);

  _indexBufferSize = (int)indices.size();

  // Temporarily disable VAO
  glBindVertexArray(0);

  const auto endTime = std::chrono::system_clock::now();
  const double elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
  printf("Elapsed time: %.3lf [sec]\n", elapsedTime / 1000.0);
}

void Terrain::paintGL(const glm::mat4 &mvMat,
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
        wireFrameWidth);

    // Enable VAO
    glBindVertexArray(_vaoId);

    // Draw triangles
    glDrawElements(GL_TRIANGLES, _indexBufferSize, GL_UNSIGNED_INT, 0);

    // Disable VAO
    glBindVertexArray(0);

    unbindShader();
  }
}