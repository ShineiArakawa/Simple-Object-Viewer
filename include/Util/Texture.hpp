#pragma once
#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include <OpenGL.hpp>
#include <Util/StbAdapter.hpp>
#include <memory>
#include <string>
#include <vector>

class Texture {
 private:
  /* data */
 public:
  using InnerTextureArray = std::vector<std::shared_ptr<std::vector<std::shared_ptr<std::vector<int>>>>>;
  using TextureArray = std::shared_ptr<InnerTextureArray>;
  static void loadTexture(const std::string& filePath, GLuint& texID);
  static void readTexture(const std::string& filePath, Texture::TextureArray texture);
};

#endif