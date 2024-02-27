#include <Model/MaterialObject.hpp>

MaterialObject::MaterialObject(const std::string& filePath, const glm::vec3 offset, const glm::vec3 scale)
    : _filePath(filePath),
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
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * materialGroup->indices->size(), materialGroup->indices->data(), GL_STATIC_DRAW);

    buffer->indexBufferSize = (int)materialGroup->indices->size();

    // Temporarily disable VAO
    glBindVertexArray(0);

    // Load texture
    if (FileUtil::exists(materialGroup->diffuseTexturePath)) {
      Texture::loadTexture(materialGroup->diffuseTexturePath, buffer->textureId);
    }

    _materialObjectBuffers->push_back(buffer);
  }
}

void MaterialObject::paintGL(const glm::mat4& mvMat,
                             const glm::mat4& mvpMat,
                             const glm::mat4& normMat,
                             const glm::mat4& lightMat,
                             const glm::vec3& lightPos,
                             const float& shininess,
                             const float& ambientIntensity,
                             const glm::vec3& wireFrameColor,
                             const float& wireFrameWidth) {
  if (_isVisible) {
    const glm::mat4& mvtMat = mvMat * glm::translate(_position);
    const glm::mat4& mvptMat = mvpMat * glm::translate(_position);

    for (const auto& object : *_materialObjectBuffers) {
      bindShader(
          mvtMat,
          mvptMat,
          normMat,
          lightMat,
          lightPos,
          object->materialGroup->shininess,
          ambientIntensity,
          object->materialGroup->ambientColor,
          object->materialGroup->diffuseColor,
          object->materialGroup->specularColor,
          getRenderType(),
          getWireFrameMode(),
          wireFrameColor,
          wireFrameWidth);

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, object->textureId);
      const GLuint uid = glGetUniformLocation(_shaderID, "u_texture");
      glUniform1i(uid, 0);

      glBindVertexArray(object->vaoId);

      glDrawElements(GL_TRIANGLES, object->indexBufferSize, GL_UNSIGNED_INT, 0);

      glBindVertexArray(0);

      glBindTexture(GL_TEXTURE_2D, 0);

      unbindShader();
    }
  }
}