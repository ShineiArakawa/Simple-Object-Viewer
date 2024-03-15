#include <Model/TextBox.hpp>

namespace simview {
namespace model {

using namespace util;
using namespace shader;

TextBox::TextBox(const char* text, const int fontPixelsize, const int padding)
    : _text(text), _padding(padding) {
  setDefaultRenderType(RenderType::TEXTURE);

  _fontRegistry = std::make_shared<fonts::TTFFontRegistry>(fonts::RobotoMedium_data,
                                                           fonts::RobotoMedium_size,
                                                           fontPixelsize);
}

TextBox::~TextBox() {}

void TextBox::initVAO() {
  // ============================================================================================================
  // Generate font texture
  // ============================================================================================================
  // Get bitmaps
  std::vector<util::fonts::Bitmap_t> vecBitmaps;
  std::vector<size_t> vecWidths;

  int height;
  const size_t nChars = strlen(_text);
  for (size_t iChar = 0; iChar < nChars; ++iChar) {
    int width;
    util::fonts::Bitmap_t bitmap = _fontRegistry->getHeightAdjustedBitmap(_text[iChar], width, height);
    vecBitmaps.push_back(bitmap);
    vecWidths.push_back(width);
  }

  // Calculate width
  size_t totalWidth = 0;
  std::vector<size_t> vecCumsumWidths;

  for (size_t iChar = 0; iChar < nChars; ++iChar) {
    vecCumsumWidths.push_back(totalWidth);
    totalWidth += vecWidths[iChar];
  }

  totalWidth += _padding * (nChars - 1);

  // Allocate
  unsigned char* bytes = (unsigned char*)malloc(sizeof(unsigned char) * totalWidth * (size_t)height * (size_t)4);
  const size_t nPixels = totalWidth * (size_t)height;
  for (size_t iPixel = 0; iPixel < nPixels; ++iPixel) {
    const size_t offset = 4 * iPixel;
    bytes[offset + 0] = 0;
    bytes[offset + 1] = 0;
    bytes[offset + 2] = 0;
    bytes[offset + 3] = 255;
  }

  // Copy
  for (size_t iChar = 0; iChar < nChars; ++iChar) {
    size_t offsetX = vecCumsumWidths[iChar];
    if (iChar > 0) {
      offsetX += _padding * iChar;
    }
    const size_t offsetY = 0;

    const size_t width = vecWidths[iChar];

    unsigned char* iBitmap = util::fonts::vectorToPointer(vecBitmaps[iChar]);

    for (size_t y = 0; y < (size_t)height; ++y) {
      const size_t indexOffsetY = 4 * (y + offsetY) * totalWidth;

      for (size_t x = 0; x < width; ++x) {
        const size_t offset = indexOffsetY + 4 * (x + offsetX);

        bytes[offset + 0] = iBitmap[y * width + x];
        bytes[offset + 1] = iBitmap[y * width + x];
        bytes[offset + 2] = iBitmap[y * width + x];
        bytes[offset + 3] = 255;
      }
    }
  }

  const float aspectRatio = (float)totalWidth / height;

  Texture::loadTexture(bytes, totalWidth, height, 4, _textureId);
  free(bytes);

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

  ObjectLoader::scaleObject(vertices, aspectRatio, 1.0f, 1.0f);

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
    const glm::mat4& mvtMat = transCtx.mvMat * glm::translate(_position);
    const glm::mat4& mvptMat = transCtx.mvpMat * glm::translate(_position);
    const glm::mat4& normMat = glm::transpose(glm::inverse(mvtMat));
    const glm::mat4& lightMvptMat = transCtx.lightMvpMat * glm::translate(_position);

    bindShader(
        mvtMat,                                                // mvMat
        mvptMat,                                               // mvpMat
        normMat,                                               // normMat
        transCtx.lightMat,                                     // lightMat
        lightingCtx.lightPos,                                  // lightPos
        lightingCtx.shininess,                                 // shininess
        lightingCtx.ambientIntensity,                          // ambientIntensity
        glm::vec3(0.0f),                                       // ambientColor
        glm::vec3(0.0f),                                       // diffuseColor
        glm::vec3(0.0f),                                       // specularColor
        getRenderType(false, Primitive::RenderType::TEXTURE),  // renderType
        getWireFrameMode(),                                    // wireFrameMode
        renderingCtx.wireFrameColor,                           // wireFrameColor
        renderingCtx.wireFrameWidth,                           // wireFrameWidth
        renderingCtx.depthTextureId,                           // depthTextureId
        lightMvptMat,                                          // lightMvpMat
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
  if (_isVisible) {
    drawGL();
  }
}

}  // namespace model
}  // namespace simview