#include <SimView/Model/Object.hpp>

namespace simview {
namespace model {

using namespace util;
using namespace shader;

Object::Object(const std::string &filePath,  // filePath
               const float offsetX,          // offsetX
               const float offsetY,          // offsetY
               const float offsetZ,          // offsetZ
               const float scale,            // scale
               const bool autoScale          // autoScale
               )
    : Primitive(),
      _filePath(filePath),
      _offsetX(offsetX),
      _offsetY(offsetY),
      _offsetZ(offsetZ),
      _scale(scale),
      _autoScale(autoScale) {}

Object::~Object() {}

void Object::initVAO() {
  VertexArray_t vertices = std::make_shared<std::vector<Vertex>>();
  IndexArray_t indices = std::make_shared<std::vector<uint32_t>>();

  ObjectLoader::readFromFile(_filePath,
                             vertices,
                             indices,
                             _offsetX,
                             _offsetY,
                             _offsetZ,
                             _autoScale);

  ObjectLoader::scaleObject(vertices, _scale);

  initVAO(vertices, indices);
}

void Object::initVAO(const std::shared_ptr<std::vector<vec3f_t>> positions,  // positions
                     const std::shared_ptr<std::vector<vec3f_t>> colors,     // colors
                     const std::shared_ptr<std::vector<vec3f_t>> normals,    // normals
                     const std::shared_ptr<std::vector<vec2f_t>> uvCoords,   // uvCoords
                     const std::shared_ptr<std::vector<int>> ids             // ids
) {
  VertexArray_t vertices = std::make_shared<std::vector<Vertex>>();
  IndexArray_t indices = std::make_shared<std::vector<uint32_t>>();

  const int nVetiices = positions->size();

  for (int iVertex = 0; iVertex < nVetiices; ++iVertex) {
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
    if (normals != nullptr) {
      normal = glm::vec3((*normals)[iVertex][0],
                         (*normals)[iVertex][1],
                         (*normals)[iVertex][2]);
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

void Object::initVAO(const VertexArray_t &vertices,
                     const IndexArray_t &indices) {
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
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * indices->size(), indices->data(), GL_STATIC_DRAW);

  _indexBufferSize = (int)indices->size();

  // Temporarily disable VAO
  glBindVertexArray(0);

  glm::vec3 minCoords, maxCoords;
  std::tie(minCoords, maxCoords) = ObjectLoader::getCorners(vertices);
  _bbox = std::make_shared<AxisAlignedBoundingBox>(minCoords, maxCoords);
}

void Object::paintGL(
    const TransformationContext &transCtx,  // transCtx
    const LightingContext &lightingCtx,     // lightingCtx
    const RenderingContext &renderingCtx    // renderingCtx
) {
  if (_isVisible) {
    const glm::mat4 &mvtMat = transCtx.mvMat * glm::translate(_position);
    const glm::mat4 &mvptMat = transCtx.mvpMat * glm::translate(_position);
    const glm::mat4 &normMat = glm::transpose(glm::inverse(mvtMat));
    const glm::mat4 &lightMvptMat = transCtx.lightMvpMat * glm::translate(_position);

    paintBBOX(mvtMat, mvptMat, normMat);

    bindShader(mvtMat,                        // mvMat
               mvptMat,                       // mvpMat
               normMat,                       // normMat
               transCtx.lightMat,             // lightMat
               lightingCtx.lightPos,          // lightPos
               lightingCtx.shininess,         // shininess
               lightingCtx.ambientIntensity,  // ambientIntensity
               glm::vec3(0.0f),               // ambientColor
               glm::vec3(0.0f),               // diffuseColor
               glm::vec3(0.0f),               // specularColor
               getRenderType(),               // renderType
               getWireFrameMode(),            // wireFrameMode
               renderingCtx.wireFrameColor,   // wireFrameColor
               renderingCtx.wireFrameWidth,   // wireFrameWidth
               renderingCtx.depthTextureId,   // depthTextureId
               lightMvptMat,                  // lightMvpMat
               _isEnabledShadowMapping,       // isEnabledShadowMapping
               false,                         // disableDepthTest
               _isEnabledNormalMap            // isEnabledNormalMap
    );

    {
      // Activate texture image
      _shader->setUniformTexture(DefaultModelShader::UNIFORM_NAME_AMBIENT_TEXTURE, _textureId);
      _shader->setUniformVariable(DefaultModelShader::UNIFORM_NAME_AMBIENT_TEXTURE_FLAG, true);

      _shader->setUniformTexture(DefaultModelShader::UNIFORM_NAME_DIFFUSE_TEXTURE, _textureId);
      _shader->setUniformVariable(DefaultModelShader::UNIFORM_NAME_DIFFUSE_TEXTURE_FLAG, true);
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
  if (_isVisible) {
    const glm::mat4 &lightMvptMat = lightMvpMat * glm::translate(_position);
    _depthShader->setUniformVariable(DefaultDepthShader::UNIFORM_NAME_LIGHT_MVP_MAT, lightMvptMat);

    drawGL();
  }
}

void Object::loadTexture(const std::string &filePath) {
  Texture::loadTexture(filePath, _textureId);
}

void Object::loadNormalMap(const std::string &filePath) {
  Texture::loadTexture(filePath, _normalMapId);
}

}  // namespace model
}  // namespace simview