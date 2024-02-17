#include <Util/StbAdapter.hpp>

namespace stb {
unsigned char *api_stbi_load(char const *filename, int *x, int *y, int *comp, int req_comp) {
  return stbi_load(filename, x, y, comp, req_comp);
}
void api_stbi_image_free(void *retval_from_stbi_load) {
  stbi_image_free(retval_from_stbi_load);
}
}  // namespace stb