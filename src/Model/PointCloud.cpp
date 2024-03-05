#include <Model/PointCloud.hpp>

namespace oglt {
namespace model {

using namespace util;
using namespace shader;

PointCloud::PointCloud(const std::string &filePath,
                       const float offsetX,
                       const float offsetY,
                       const float offsetZ,
                       const float scale,
                       const float pointSize)
    : _filePath(filePath),
      _offsetX(offsetX),
      _offsetY(offsetY),
      _offsetZ(offsetZ),
      _scale(scale),
      _pointSize(pointSize) {
}

void PointCloud::initVAO() {
  std::shared_ptr<std::vector<Vertex>> points = std::make_shared<std::vector<Vertex>>();
  std::shared_ptr<std::vector<uint32_t>> indices = std::make_shared<std::vector<uint32_t>>();

  ObjectLoader::readFromFile(_filePath, points, indices, _offsetX, _offsetY, _offsetZ);
  ObjectLoader::moveToOrigin(points);
  ObjectLoader::scaleObject(points, _scale);
  ObjectLoader::translateObject(points, _offsetX, _offsetY, _offsetZ);

  LOG_INFO("Loaded point cloud data with " + std::to_string(points->size()) + " points.");

  initVAO(points, indices);
}

void PointCloud::initVAO(const std::shared_ptr<std::vector<vec3f_t>> positions,
                         const std::shared_ptr<std::vector<vec3f_t>> colors = nullptr,
                         const std::shared_ptr<std::vector<int>> ids = nullptr) {
  std::shared_ptr<std::vector<Vertex>> points = std::make_shared<std::vector<Vertex>>();
  std::shared_ptr<std::vector<uint32_t>> indices = std::make_shared<std::vector<uint32_t>>();

  const int nPoints = positions->size();

  for (int iVertex = 0; iVertex < nPoints; ++iVertex) {
    const glm::vec3 position = glm::vec3((*positions)[iVertex][0],
                                         (*positions)[iVertex][1],
                                         (*positions)[iVertex][2]);

    glm::vec3 color(0.0f);
    if (colors != nullptr) {
      color = glm::vec3((*colors)[iVertex][0],
                        (*colors)[iVertex][1],
                        (*colors)[iVertex][2]);
    }

    glm::vec3 normal(0.0f);
    glm::vec3 bary(0.0f);

    glm::vec2 uvCoord(0.0f);

    int id = -1;
    if (ids != nullptr) {
      id = (*ids)[iVertex];
    }

    const Vertex vertex(position, color, normal, bary, uvCoord, id);

    points->push_back(vertex);
    indices->push_back(iVertex);
  }

  ObjectLoader::moveToOrigin(points);
  ObjectLoader::scaleObject(points, _scale);
  ObjectLoader::translateObject(points, _offsetX, _offsetY, _offsetZ);

  LOG_INFO("Num of points : " + std::to_string(points->size()));

  initVAO(points, indices);
}

void PointCloud::initVAO(const std::shared_ptr<std::vector<Vertex>> &points,
                         const std::shared_ptr<std::vector<uint32_t>> &indices) {
  // Create VAO
  glGenVertexArrays(1, &_vaoId);
  glBindVertexArray(_vaoId);

  // Create vertex buffer object
  glGenBuffers(1, &_vertexBufferId);
  glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferId);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * points->size(), points->data(), GL_STATIC_DRAW);

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
  std::tie(minCoords, maxCoords) = ObjectLoader::getCorners(points);
  _bbox = std::make_shared<AxisAlignedBoundingBox>(minCoords, maxCoords);
}

void PointCloud::paintGL(const glm::mat4 &mvMat,
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
        lightMvptMat);

    paintBBOX(mvtMat, mvptMat, normMat);

    drawGL();

    unbindShader();
  }
}

void PointCloud::drawGL(const int &index) {
  // Enable VAO
  glBindVertexArray(_vaoId);

  glPointSize(_pointSize);

  // Draw triangles
  // (GLenum mode, GLsizei count, GLenum type, const void *indices)
  glDrawElements(GL_POINTS, _indexBufferSize, GL_UNSIGNED_INT, 0);

  // Disable VAO
  glBindVertexArray(0);
}

void PointCloud::drawAllGL(const glm::mat4 &lightMvpMat) {
  const glm::mat4 &lightMvptMat = lightMvpMat * glm::translate(_position);
  _depthShader->setUniformVariable(DefaultDepthShader::UNIFORM_NAME_LIGHT_MVP_MAT, lightMvptMat);

  drawGL();
}
}  // namespace model
}  // namespace oglt