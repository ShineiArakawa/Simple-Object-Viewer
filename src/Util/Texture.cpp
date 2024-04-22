#include <SimView/Util/Texture.hpp>

namespace simview {
namespace util {

void Texture::loadTexture(const std::string& filePath, GLuint& texID) {
  // Texture ============================================================================================
  int texWidth, texHeight, channels;
  unsigned char* bytesTexture = stb::api_stbi_load(filePath.c_str(), &texWidth, &texHeight, &channels, stb::api_STBI_rgb_alpha);

  if (!bytesTexture) {
    LOG_ERROR("Failed to load image file from " + filePath);
    return;
  }

  loadTexture(bytesTexture, texWidth, texHeight, channels, texID);

  stb::api_stbi_image_free(bytesTexture);

  LOG_INFO("Loaded texture from " + filePath);
}

void Texture::loadTexture(const unsigned char* bytes,
                          const int& width,
                          const int& height,
                          const int& channels,
                          GLuint& texID) {
  GLint internalFormat = -1;
  GLenum format = -1;

  if (channels == 1) {
    internalFormat = GL_R8;
    format = GL_RED;
  } else {
    internalFormat = GL_RGBA8;
    format = GL_RGBA;
  }

  glGenTextures(1, &texID);
  glBindTexture(GL_TEXTURE_2D, texID);
  glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, bytes);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glGenerateMipmap(GL_TEXTURE_2D);

  glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::readTexture(const std::string& filePath, Texture::TextureArray texture) {
  texture->clear();

  int texWidth, texHeight, nChannels;

  // Texture ============================================================================================
  unsigned char* bytesTexture = stb::api_stbi_load(filePath.c_str(), &texWidth, &texHeight, &nChannels, stb::api_STBI_rgb_alpha);
  if (!bytesTexture) {
    LOG_ERROR("Failed to load image file from " + filePath);
    return;
  }

  for (int i = 0; i < texHeight; ++i) {
    texture->push_back(std::make_shared<std::vector<std::shared_ptr<std::vector<int>>>>());

    for (int j = 0; j < texWidth; ++j) {
      (*texture)[i]->push_back(std::make_shared<std::vector<int>>());
      unsigned char* texel = bytesTexture + (j + texWidth * i) * nChannels;

      for (int channel = 0; channel < nChannels; channel++) {
        unsigned char charValue = texel[channel];
        int value = charValue;
        (*(*texture)[i])[j]->push_back(value);
      }
    }
  }

  stb::api_stbi_image_free(bytesTexture);
}

}  // namespace util
}  // namespace simview