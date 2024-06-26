#include <SimView/Model/MaterialObject.hpp>

namespace simview {
namespace model {

using namespace util;
using namespace shader;

MaterialObject::MaterialObject(const std::string& filePath,
                               const glm::vec3 offset,
                               const glm::vec3 scale)
    : Primitive(),
      _filePath(filePath),
      _offset(offset),
      _scale(scale),
      _materialObjectBuffers(std::make_shared<std::vector<MaterialObjectBuffer_t>>()) {
}

MaterialObject::~MaterialObject() = default;

void MaterialObject::initVAO() {
  MaterialGroups_t materialGroups = std::make_shared<std::vector<MaterialGroup_t>>();
  ObjectLoader::readObjFileWithMaterialGroup(_filePath,
                                             materialGroups,
                                             _offset,
                                             _scale);

  for (int iObject = 0; iObject < (int)materialGroups->size(); ++iObject) {
    MaterialObjectBuffer_t buffer = std::make_shared<MaterialObjectBuffer>();
    const auto& materialGroup = (*materialGroups)[iObject];

    buffer->materialGroup = materialGroup;

    // Create VAO
    glGenVertexArrays(1, &buffer->vaoId);
    glBindVertexArray(buffer->vaoId);

    // Create vertex buffer object
    glGenBuffers(1, &buffer->vertexBufferId);
    glBindBuffer(GL_ARRAY_BUFFER, buffer->vertexBufferId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * materialGroup->vertices->size(), materialGroup->vertices->data(), GL_STATIC_DRAW);

    // Setup attributes for vertex buffer object
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

    // Create index buffer object
    glGenBuffers(1, &buffer->indexBufferId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->indexBufferId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * materialGroup->indices->size(), materialGroup->indices->data(), GL_STATIC_DRAW);

    buffer->indexBufferSize = (int)materialGroup->indices->size();

    // Temporarily disable VAO
    glBindVertexArray(0);

    // Load texture
    if (FileUtil::exists(materialGroup->ambientTexturePath) && FileUtil::isFile(materialGroup->ambientTexturePath)) {
      Texture::loadTexture(materialGroup->ambientTexturePath, buffer->ambientTextureId);
      buffer->enabledAmbientTexture = true;
    }

    if (FileUtil::exists(materialGroup->diffuseTexturePath) && FileUtil::isFile(materialGroup->diffuseTexturePath)) {
      Texture::loadTexture(materialGroup->diffuseTexturePath, buffer->diffuseTextureId);
      buffer->enabledDiffuseTexture = true;
    }

    if (FileUtil::exists(materialGroup->specularTexturePath) && FileUtil::isFile(materialGroup->specularTexturePath)) {
      Texture::loadTexture(materialGroup->specularTexturePath, buffer->specularTextureId);
      buffer->enabledSpecularTexture = true;
    }

    if (FileUtil::exists(materialGroup->bumpTexturePath) && FileUtil::isFile(materialGroup->bumpTexturePath)) {
      Texture::loadTexture(materialGroup->bumpTexturePath, buffer->bumpTextureId);
      buffer->enabledBumpTexture = true;
    }

    buffer->wireFrame = std::make_shared<WireFrame>(materialGroup->vertices, materialGroup->indices);

    _materialObjectBuffers->push_back(buffer);
  }
}

void MaterialObject::paintGL(
    const TransformationContext& transCtx,  // transCtx
    const LightingContext& lightingCtx,     // lightingCtx
    const RenderingContext& renderingCtx    // renderingCtx
) {
  if (_isVisible) {
    const glm::mat4& mvtMat = transCtx.mvMat * glm::translate(_position);
    const glm::mat4& mvptMat = transCtx.mvpMat * glm::translate(_position);
    const glm::mat4& normMat = glm::transpose(glm::inverse(mvtMat));
    const glm::mat4& lightMvptMat = transCtx.lightMvpMat * glm::translate(_position);

    for (int iObject = 0; iObject < (int)_materialObjectBuffers->size(); ++iObject) {
      const MaterialObjectBuffer_t& object = (*_materialObjectBuffers)[iObject];

      paintWireFrame(object, mvptMat, renderingCtx.wireFrameColor, renderingCtx.wireFrameWidth);

      if (_wireFrameMode != WireFrameMode::ONLY) {
        {
          // Prepare
          bindShader(
              mvtMat,                                            // mvMat
              mvptMat,                                           // mvpMat
              normMat,                                           // normMat
              transCtx.lightMat,                                 // lightMat
              lightingCtx.lightPos,                              // lightPos
              object->materialGroup->shininess,                  // shininess
              lightingCtx.ambientIntensity,                      // ambientIntensity
              object->materialGroup->ambientColor,               // ambientColor
              object->materialGroup->diffuseColor,               // diffuseColor
              object->materialGroup->specularColor,              // specularColor
              getRenderType(),                                   // renderType
              renderingCtx.wireFrameColor,                       // wireFrameColor
              renderingCtx.wireFrameWidth,                       // wireFrameWidth
              renderingCtx.depthTextureId,                       // depthTextureId
              lightMvptMat,                                      // lightMvpMat
              _isEnabledShadowMapping,                           // isEnabledShadowMapping
              false,                                             // disableDepthTest
              _isEnabledNormalMap && object->enabledBumpTexture  // isEnabledNormalMap
          );

          _shader->setUniformTexture(DefaultModelShader::UNIFORM_NAME_AMBIENT_TEXTURE, object->ambientTextureId);
          _shader->setUniformVariable(DefaultModelShader::UNIFORM_NAME_AMBIENT_TEXTURE_FLAG, object->enabledAmbientTexture);

          _shader->setUniformTexture(DefaultModelShader::UNIFORM_NAME_DIFFUSE_TEXTURE, object->diffuseTextureId);
          _shader->setUniformVariable(DefaultModelShader::UNIFORM_NAME_DIFFUSE_TEXTURE_FLAG, object->enabledDiffuseTexture);

          _shader->setUniformTexture(DefaultModelShader::UNIFORM_NAME_SPECULAR_TEXTURE, object->specularTextureId);
          _shader->setUniformVariable(DefaultModelShader::UNIFORM_NAME_SPECULAR_TEXTURE_FLAG, object->enabledSpecularTexture);

          _shader->setUniformTexture(DefaultModelShader::UNIFORM_NAME_NORMAL_MAP, object->bumpTextureId);
        }

        drawGL(iObject);

        {
          // Post process
          unbindShader();
        }
      }
    }
  }
}

void MaterialObject::drawGL(const int& index) {
  // Draw
  glBindVertexArray((*_materialObjectBuffers)[index]->vaoId);
  glDrawElements(GL_TRIANGLES, (*_materialObjectBuffers)[index]->indexBufferSize, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

void MaterialObject::drawAllGL(const glm::mat4& lightMvpMat) {
  if (_isVisible) {
    const glm::mat4& lightMvptMat = lightMvpMat * glm::translate(_position);
    _depthShader->setUniformVariable(DefaultDepthShader::UNIFORM_NAME_LIGHT_MVP_MAT, lightMvptMat);

    for (int iObject = 0; iObject < (int)_materialObjectBuffers->size(); ++iObject) {
      drawGL(iObject);
    }
  }
}

void MaterialObject::paintWireFrame(const MaterialObjectBuffer_t& object,
                                    const glm::mat4& mvpMat,
                                    const glm::vec3& color,
                                    const float& width) const {
  if (object->wireFrame != nullptr && (_wireFrameMode == WireFrameMode::ON || _wireFrameMode == WireFrameMode::ONLY)) {
    _shader->getLineShader()->bind();
    _shader->getLineShader()->setUniformVariable(shader::DefaultLineShader::UNIFORM_NAME_MVP_MAT, mvpMat);
    _shader->getLineShader()->setUniformVariable(shader::DefaultLineShader::UNIFORM_NAME_LINE_COLOER, color);

    object->wireFrame->draw(width);

    _shader->unbind();
  }
};

}  // namespace model
}  // namespace simview