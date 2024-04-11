#include <SimView/Model/TextBox.hpp>

namespace simview {
namespace model {

using namespace util;
using namespace shader;

TextBox::TextBox(const char* text,
                 const glm::vec2 position,
                 const float sizeMagnification,
                 const int fontPixelsize,
                 const int padding)
    : Primitive(),
      _text(text),
      _postionInScreenSpace(std::min(1.0f, std::max(-1.0f, position.x)),  // X
                            std::min(1.0f, std::max(-1.0f, position.y)),  // Y
                            0.99f,                                        // Far
                            1.0f),
      _padding(padding),
      _fontRegistry(std::make_shared<fonts::TrueTypeFontRegistry>(fonts::RobotoMedium_data,
                                                                  fonts::RobotoMedium_size,
                                                                  fontPixelsize,
                                                                  padding)),
      _sizeMagnification(sizeMagnification) {
  // Set defailt render mode to texture
  setDefaultRenderType(RenderType::TEXTURE);
}

TextBox::~TextBox() {}

void TextBox::initVAO() {
  // ============================================================================================================
  // Generate font texture
  // ============================================================================================================
  // Get bitmaps
  int width, height;
  const fonts::Bitmap_t bitmap = _fontRegistry->getBitmap(_text, width, height);

  // Copy to RGBA format data
  const size_t nBytes = sizeof(unsigned char) * width * height * (size_t)4;

  if ((double)nBytes / 1024.0 / 1024.0 / 1024.0 > 1.0) {
    LOG_WARN("The size of texture is larger than 1 GB. Be careful not to overflow the memory.");
  }

  unsigned char* bytes = (unsigned char*)malloc(nBytes);
  const size_t nPixels = (size_t)width * height;

  for (size_t iPixel = 0; iPixel < nPixels; ++iPixel) {
    const size_t offset = 4 * iPixel;
    bytes[offset + 0] = bitmap->data[iPixel];
    bytes[offset + 1] = bitmap->data[iPixel];
    bytes[offset + 2] = bitmap->data[iPixel];
    bytes[offset + 3] = 255;
  }

  // Transfer to VRAM
  Texture::loadTexture(bytes, width, height, 4, _textureId);
  free(bytes);

  // Calc aspect ratio to resize quad
  const float aspectRatio = (float)width / height;

  // ============================================================================================================
  // Init quad
  // ============================================================================================================
  VertexArray_t vertices = std::make_shared<std::vector<Vertex>>();
  IndexArray_t indices = std::make_shared<std::vector<uint32_t>>();
  int idx = 0;

  for (int j = 0; j < 3; j++) {
    Vertex v(positions[j], glm::vec3(0), glm::vec3(0), BARY_CENTER[j], uvCoords[j], -1.0);
    vertices->push_back(v);
    indices->push_back(idx++);
  }

  for (int j = 0; j < 3; j++) {
    Vertex v(positions[j + 1], glm::vec3(0), glm::vec3(0), BARY_CENTER[j], uvCoords[j + 1], -1.0);
    vertices->push_back(v);
    indices->push_back(idx++);
  }

  ObjectLoader::scaleObject(vertices, aspectRatio * _sizeMagnification, _sizeMagnification, 1.0f);
  ObjectLoader::rotateObject(vertices, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));  // Rotate on X-axis
  ObjectLoader::rotateObject(vertices, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));  // Rotate on Z-axis
  // NOTE: Then, the quad is faced with screen.

  glGenVertexArrays(1, &_vaoId);
  glBindVertexArray(_vaoId);

  glGenBuffers(1, &_vertexBufferId);
  glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferId);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices->size(), vertices->data(), GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));

  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bary));

  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

  glEnableVertexAttribArray(5);
  glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, id));

  glGenBuffers(1, &_indexBufferId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufferId);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * indices->size(), indices->data(), GL_STATIC_DRAW);

  glBindVertexArray(0);
}

void TextBox::paintGL(
    const TransformationContext& transCtx,  // transCtx
    const LightingContext& lightingCtx,     // lightingCtx
    const RenderingContext& renderingCtx    // renderingCtx
) {
  if (_isVisible) {
    // Re-compute model-view-projection matrices
    const glm::mat4& mvMat = transCtx.viewMat;  // Indentity model matrix
    const glm::mat4& mvpMat = transCtx.projMat * mvMat;
    const glm::mat4& lightMat = mvMat * transCtx.lightTransMat;

    // Calc model space position from screen space 2D coords
    glm::vec4 screenSpaceCoords = glm::inverse(transCtx.projMat) * _postionInScreenSpace;
    screenSpaceCoords /= screenSpaceCoords.w;
    const glm::vec3& positionInModelSpace = (glm::inverse(mvMat) * screenSpaceCoords).xyz();

    // Tranlation in model space
    const glm::mat4& mvtMat = mvMat * glm::translate(positionInModelSpace);
    const glm::mat4& mvptMat = mvpMat * glm::translate(positionInModelSpace);
    const glm::mat4& normMat = glm::transpose(glm::inverse(mvtMat));

    bindShader(
        mvtMat,                                                // mvMat
        mvptMat,                                               // mvpMat
        normMat,                                               // normMat
        lightMat,                                              // lightMat
        lightingCtx.lightPos,                                  // lightPos
        lightingCtx.shininess,                                 // shininess
        lightingCtx.ambientIntensity,                          // ambientIntensity
        glm::vec3(0.0f),                                       // ambientColor
        glm::vec3(0.0f),                                       // diffuseColor
        glm::vec3(0.0f),                                       // specularColor
        getRenderType(false, Primitive::RenderType::TEXTURE),  // renderType: always TEXTURE mode
        0.0f,                                                  // wireFrameMode: always off
        renderingCtx.wireFrameColor,                           // wireFrameColor
        renderingCtx.wireFrameWidth,                           // wireFrameWidth
        renderingCtx.depthTextureId,                           // depthTextureId
        glm::mat4(0.0f),                                       // lightMvpMat
        false,                                                 // isEnabledShadowMapping
        false,                                                 // disableDepthTest
        false                                                  // isEnabledNormalMap
    );

    _shader->setUniformTexture(DefaultModelShader::UNIFORM_NAME_DIFFUSE_TEXTURE, _textureId);
    _shader->setUniformVariable(DefaultModelShader::UNIFORM_NAME_DIFFUSE_TEXTURE_FLAG, true);

    drawGL();

    unbindShader();
  }
}

void TextBox::drawGL(const int& index) {
  glBindVertexArray(_vaoId);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

void TextBox::drawAllGL(const glm::mat4& lightMvpMat) {
  // NOTE: Alwayes disabled for shadow mapping
  // if (_isVisible) {
  //   drawGL();
  // }
}

}  // namespace model
}  // namespace simview