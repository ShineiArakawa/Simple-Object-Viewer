#include <SimView/Util/StbAdapter.hpp>

namespace simview {
namespace util {
namespace stb {

unsigned char *api_stbi_load(char const *filename, int *x, int *y, int *comp, int req_comp) {
  return stbi_load(filename, x, y, comp, req_comp);
}

void api_stbi_image_free(void *retval_from_stbi_load) {
  stbi_image_free(retval_from_stbi_load);
}

void saveImage(const int width, const int height, const int channels, unsigned char *bytes, const std::string filePath) {
  if (bytes == nullptr) {
    LOG_ERROR("Bytes is nullptr!");
    return;
  }

  try {
    const std::string dirPath = FileUtil::dirPath(filePath);
    if (!FileUtil::exists(dirPath)) {
      FileUtil::mkdirs(dirPath);
    }

    const std::string extension = FileUtil::extension(filePath);
    if (extension == ".png") {
      stbi_write_png(filePath.c_str(), width, height, channels, bytes, 0);
    } else if (extension == ".jpg") {
      stbi_write_jpg(filePath.c_str(), width, height, channels, bytes, 100);
    } else {
      LOG_ERROR("Unsupported save image format: " + extension);
    }
  } catch (std::exception &error) {
    LOG_ERROR("[Error] See below messages.");
    LOG_ERROR("error.what()");
  }
};

}  // namespace stb
}  // namespace util
}  // namespace simview