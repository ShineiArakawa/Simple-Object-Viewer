#pragma once

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include <stb_image.h>
#endif

namespace stb {
const int api_STBI_rgb_alpha = STBI_rgb_alpha;
unsigned char *api_stbi_load(char const *filename, int *x, int *y, int *comp, int req_comp);
void api_stbi_image_free(void *);
}  // namespace stb