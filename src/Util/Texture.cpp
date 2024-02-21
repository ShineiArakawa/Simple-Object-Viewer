#include <Util/Texture.hpp>

void Texture::loadTexture(const std::string& filePath, GLuint& texID) {
  int texWidth, texHeight, channels;
  // Texture ============================================================================================
  unsigned char* bytesTexture = stb::api_stbi_load(filePath.c_str(), &texWidth, &texHeight, &channels, stb::api_STBI_rgb_alpha);
  if (!bytesTexture) {
    fprintf(stderr, "Failed to load image file: %s\n", filePath.c_str());
    exit(1);
  }

  glGenTextures(1, &texID);
  glBindTexture(GL_TEXTURE_2D, texID);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytesTexture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glGenerateMipmap(GL_TEXTURE_2D);

  glBindTexture(GL_TEXTURE_2D, 0);

  stb::api_stbi_image_free(bytesTexture);
}

void Texture::readTexture(const std::string& filePath, Texture::TextureArray texture) {
  texture->clear();
  int texWidth, texHeight, nChannels;

  // Texture ============================================================================================
  unsigned char* bytesTexture = stb::api_stbi_load(filePath.c_str(), &texWidth, &texHeight, &nChannels, stb::api_STBI_rgb_alpha);
  if (!bytesTexture) {
    fprintf(stderr, "Failed to load image file: %s\n", filePath.c_str());
    exit(1);
  }

  for (int i = 0; i < texHeight; i++) {
    texture->push_back(std::make_shared<std::vector<std::shared_ptr<std::vector<int>>>>());

    for (int j = 0; j < texWidth; j++) {
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
