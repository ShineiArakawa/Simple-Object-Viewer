#include <Util/Colors.hpp>

namespace simview {
namespace util {

vec3f_t rgbToHsv(vec3f_t rgb) {
  vec3f_t hsv;
  double min, max, delta;

  min = std::min({rgb[0], rgb[1], rgb[2]});
  max = std::max({rgb[0], rgb[1], rgb[2]});

  hsv[2] = max;  // Value

  delta = max - min;

  if (-1e-10 < max && max < 1e-10) {
    // r = g = b = 0
    hsv[1] = 0.0f;
    hsv[0] = -1.0f;
    return hsv;
  } else {
    hsv[1] = delta / max;  // Saturation
  }

  if (rgb[0] == max) {
    hsv[0] = (rgb[1] - rgb[2]) / delta;  // between yellow & magenta
  } else if (rgb[1] == max) {
    hsv[0] = 2.0f + (rgb[2] - rgb[0]) / delta;  // between cyan & yellow
  } else {
    hsv[0] = 4.0f + (rgb[0] - rgb[1]) / delta;  // between magenta & cyan
  }

  hsv[0] *= 60.0f;  // degrees

  if (hsv[0] < 0.0f) {
    hsv[0] += 360.0f;
  }

  return hsv;
}

vec3f_t hsvToRgb(vec3f_t hsv) {
  vec3f_t rgb;
  float hh, p, q, t, ff;
  int i;

  if (hsv[1] <= 0.0f) {  // 彩度が0の場合はグレースケール
    rgb[0] = hsv[2];
    rgb[1] = hsv[2];
    rgb[2] = hsv[2];
    return rgb;
  }

  hh = hsv[0];
  if (hh >= 360.0f) hh = 0.0f;
  hh /= 60.0f;
  i = static_cast<int>(hh);
  ff = hh - i;
  p = hsv[2] * (1.0f - hsv[1]);
  q = hsv[2] * (1.0f - (hsv[1] * ff));
  t = hsv[2] * (1.0f - (hsv[1] * (1.0f - ff)));

  switch (i) {
    case 0:
      rgb[0] = hsv[2];
      rgb[1] = t;
      rgb[2] = p;
      break;
    case 1:
      rgb[0] = q;
      rgb[1] = hsv[2];
      rgb[2] = p;
      break;
    case 2:
      rgb[0] = p;
      rgb[1] = hsv[2];
      rgb[2] = t;
      break;
    case 3:
      rgb[0] = p;
      rgb[1] = q;
      rgb[2] = hsv[2];
      break;
    case 4:
      rgb[0] = t;
      rgb[1] = p;
      rgb[2] = hsv[2];
      break;
    default:
      rgb[0] = hsv[2];
      rgb[1] = p;
      rgb[2] = q;
      break;
  }

  return rgb;
}

}  // namespace util
}  // namespace simview
