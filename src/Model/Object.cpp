#include <Model/Object.hpp>
#include <Util/Texture.hpp>

namespace simview {
namespace model {

using namespace util;
using namespace shader;

Object::Object(const std::string &filePath,
               const float offsetX,
               const float offsetY,
               const float offsetZ,
               const float scale)
    : _filePath(filePath),
      _offsetX(offsetX),
      _offsetY(offsetY),
      _offsetZ(offsetZ),
      _scale(scale) {
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

  initVAO(vertices, indices);
}

void Object::initVAO(const std::shared_ptr<std::vector<vec3f_t>> positions,
                     const std::shared_ptr<std::vector<vec3f_t>> colors,
                     const std::shared_ptr<std::vector<vec3f_t>> normals,
                     const std::shared_ptr<std::vector<vec2f_t>> uvCoords,
                     const std::shared_ptr<std::vector<int>> ids) {
  std::shared_ptr<std::vector<Vertex>> vertices = std::make_shared<std::vector<Vertex>>();
  std::shared_ptr<std::vector<uint32_t>> indices = std::make_shared<std::vector<uint32_t>>();

  const int nVetiices = positions->size();

  for (int iVertex = 0; iVertex < nVetiices; ++iVertex) {
    const glm::vec3 position = glm::vec3((*positions)[iVertex][0], (*positions)[iVertex][1], (*positions)[iVertex][2]);

    glm::vec3 color(0.0f);
    if (colors != nullptr) {
      color = glm::vec3((*colors)[iVertex][0], (*colors)[iVertex][1], (*colors)[iVertex][2]);
    }

    glm::vec3 normal(0.0f);
    if (normals != nullptr) {
      normal = glm::vec3((*normals)[iVertex][0], (*normals)[iVertex][1], (*normals)[iVertex][2]);
    }

    const int iBary = iVertex % 3;
    const glm::vec3 bary = BARY_CENTER[iBary];

    glm::vec2 uvCoord;
    if (uvCoords != nullptr) {
      uvCoord = glm::vec2((*uvCoords)[iVertex][0], (*uvCoords)[iVertex][1]);
    }

    int id = -1;
    if (ids != nullptr) {
      id = (*ids)[iVertex];
    }

    const Vertex vertex(position, color, normal, bary, uvCoord, id);

    vertices->push_back(vertex);
    indices->push_back(iVertex);
  }

  ObjectLoader::moveToOrigin(vertices);
  ObjectLoader::scaleObject(vertices, _scale);
  ObjectLoader::translateObject(vertices, _offsetX, _offsetY, _offsetZ);

  LOG_INFO("Num of vertices : " + std::to_string(vertices->size()));
  LOG_INFO("Num of triangles: " + std::to_string(vertices->size() / 3));

  initVAO(vertices, indices);
}

void Object::initVAO(const std::shared_ptr<std::vector<Vertex>> &vertices,
                     const std::shared_ptr<std::vector<uint32_t>> &indices) {
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

  glm::vec3 minCoords, maxCoords;
  std::tie(minCoords, maxCoords) = ObjectLoader::getCorners(vertices);
  _bbox = std::make_shared<AxisAlignedBoundingBox>(minCoords, maxCoords);
}

void Object::paintGL(const glm::mat4 &mvMat,
                     const glm::mat4 &mvpMat,
                     const glm::mat4 &lightMat,
                     const glm::vec3 &lightPos,
                     const float &shininess,
                     const float &ambientIntensity,
                     const glm::vec3 &wireFrameColor,
                     const float &wireFrameWidth,
                     const GLuint &depthTextureId,
                     const glm::mat4 &lightMvpMat) {
  if (_isVisible) {
    const glm::mat4 &mvtMat = mvMat * glm::translate(_position);
    const glm::mat4 &mvptMat = mvpMat * glm::translate(_position);
    const glm::mat4 &normMat = glm::transpose(glm::inverse(mvtMat));
    const glm::mat4 &lightMvptMat = lightMvpMat * glm::translate(_position);

    paintBBOX(mvtMat, mvptMat, normMat);

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
        depthTextureId,
        lightMvptMat,
        false,
        _isEnabledNormalMap);

    {
      // Activate texture image
      _shader->setUniformTexture(DefaultModelShader::UNIFORM_NAME_AMBIENT_TEXTURE, _textureId);
      _shader->setUniformVariable(DefaultModelShader::UNIFORM_NAME_AMBIENT_TEXTURE_FLAG, 1.0f);

      _shader->setUniformTexture(DefaultModelShader::UNIFORM_NAME_DIFFUSE_TEXTURE, _textureId);
      _shader->setUniformVariable(DefaultModelShader::UNIFORM_NAME_DIFFUSE_TEXTURE_FLAG, 1.0f);
    }

    {
      // Activate normal map
      _shader->setUniformTexture(DefaultModelShader::UNIFORM_NAME_NORMAL_MAP, _normalMapId);
    }

    drawGL();

    unbindShader();
  }
}

void Object::drawGL(const int &index) {
  // Draw
  glBindVertexArray(_vaoId);
  glDrawElements(GL_TRIANGLES, _indexBufferSize, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

void Object::drawAllGL(const glm::mat4 &lightMvpMat) {
  const glm::mat4 &lightMvptMat = lightMvpMat * glm::translate(_position);
  _depthShader->setUniformVariable(DefaultDepthShader::UNIFORM_NAME_LIGHT_MVP_MAT, lightMvptMat);

  drawGL();
}

void Object::loadTexture(const std::string &filePath) {
  Texture::loadTexture(filePath, _textureId);
}

void Object::loadNormalMap(const std::string &filePath) {
  Texture::loadTexture(filePath, _normalMapId);
}
}  // namespace model
}  // namespace simview