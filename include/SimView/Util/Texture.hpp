#pragma once

#include <SimView/OpenGL.hpp>
#include <SimView/Util/Logging.hpp>
#include <SimView/Util/StbAdapter.hpp>
#include <memory>
#include <string>
#include <vector>

namespace simview {
namespace util {

class Texture {
 private:
  // Nothing

 public:
  using InnerTextureArray = std::vector<std::shared_ptr<std::vector<std::shared_ptr<std::vector<int>>>>>;
  using TextureArray = std::shared_ptr<InnerTextureArray>;

  static void loadTexture(const std::string& filePath, GLuint& texID);
  static void loadTexture(const unsigned char* bytes,
                          const int& width,
                          const int& height,
                          const int& channels,
                          GLuint& texID);
  static void readTexture(const std::string& filePath, Texture::TextureArray texture);
};

}  // namespace util
}  // namespace simview