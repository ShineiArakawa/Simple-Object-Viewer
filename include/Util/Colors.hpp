#pragma once

#include <Util/Math.hpp>
#include <algorithm>

namespace simview {
namespace util {

vec3f_t rgbToHsv(vec3f_t rgb);
vec3f_t hsvToRgb(vec3f_t hsv);

}  // namespace util
}  // namespace simview
