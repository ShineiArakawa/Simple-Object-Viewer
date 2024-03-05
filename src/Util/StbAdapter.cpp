#include <Util/StbAdapter.hpp>

namespace oglt {
namespace util {
namespace stb {

unsigned char *api_stbi_load(char const *filename, int *x, int *y, int *comp, int req_comp) {
  return stbi_load(filename, x, y, comp, req_comp);
}

void api_stbi_image_free(void *retval_from_stbi_load) {
  stbi_image_free(retval_from_stbi_load);
}

void saveImage(const int width, const int height, const int channels, unsigned char *bytes, const std::string filePath) {
  try {
    const std::string dirPath = FileUtil::dirPath(filePath);
    if (!FileUtil::exists(dirPath)) {
      FileUtil::mkdirs(dirPath);
    }

    // if (FileUtil::exists(filePath)) {
    //   std::remove(filePath.c_str());
    // }

    const std::string extension = FileUtil::extension(filePath);
    if (extension == ".png") {
      stbi_write_png(filePath.c_str(), width, height, channels, bytes, 0);
    } else if (extension == ".jpg") {
      stbi_write_jpg(filePath.c_str(), width, height, channels, bytes, 100);
    } else {
      fprintf(stderr, "Unsupported save image format: %s\n", extension.c_str());
    }
  } catch (std::exception &error) {
    std::cerr << "[Error] See below messages." << std::endl;
    std::cerr << error.what() << std::endl;
  }
};

}  // namespace stb
}  // namespace util
}  // namespace oglt