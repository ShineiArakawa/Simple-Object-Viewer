#pragma once

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include <stb_image.h>
#endif

#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_STATIC
#include <stb_image_write.h>
#endif

#include <SimView/Util/FileUtil.hpp>
#include <SimView/Util/Logging.hpp>
#include <iostream>
#include <string>

namespace simview {
namespace util {
namespace stb {

const int api_STBI_rgb_alpha = STBI_rgb_alpha;
unsigned char *api_stbi_load(char const *filename, int *x, int *y, int *comp, int req_comp);
void api_stbi_image_free(void *);

void saveImage(const int width, const int height, const int channels, unsigned char *bytes, const std::string filePath);

}  // namespace stb
}  // namespace util
}  // namespace simview