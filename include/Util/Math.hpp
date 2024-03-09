#pragma once

#include <array>
#include <memory>
#include <vector>

namespace simview {

template <class dtype>
using vec4_t = std::array<dtype, 4>;
using vec4f_t = vec4_t<float>;
using vec4d_t = vec4_t<double>;
using vec4i_t = vec4_t<int>;
using vec4ull_t = vec4_t<size_t>;

template <class dtype>
using vec3_t = std::array<dtype, 3>;
using vec3f_t = vec3_t<float>;
using vec3d_t = vec3_t<double>;
using vec3i_t = vec3_t<int>;
using vec3ull_t = vec3_t<size_t>;

template <class dtype>
using vec2_t = std::array<dtype, 2>;
using vec2f_t = vec2_t<float>;
using vec2d_t = vec2_t<double>;
using vec2i_t = vec2_t<int>;
using vec2ull_t = vec2_t<size_t>;

template <class dtype>
using vec_pt = std::shared_ptr<std::vector<dtype>>;
using vecf_pt = vec_pt<float>;
using vecd_pt = vec_pt<double>;
using veci_pt = vec_pt<int>;
using vecull_pt = vec_pt<size_t>;

namespace math {

inline vec3f_t outerProduct(const vec3f_t& vec0,
                            const vec3f_t& vec1) {
  return {
      vec0[1] * vec1[2] - vec0[2] * vec1[1],  // vec0.y * vec1.z - vec0.z * vec1.y
      vec0[2] * vec1[0] - vec0[0] * vec1[2],  // vec0.z * vec1.x - vec0.x * vec1.z
      vec0[0] * vec1[1] - vec0[1] * vec1[0]   // vec0.x * vec1.y - vec0.y * vec1.x
  };
}

inline void outerProduct(const float& x0, const float& y0, const float& z0,
                         const float& x1, const float& y1, const float& z1,
                         float& x, float& y, float& z) {
  x = y0 * z1 - z0 * y1;
  y = z0 * x1 - x0 * z1;
  z = x0 * y1 - y0 * x1;
}

inline void normalize(vec3f_t& vec) {
  const float length = std::sqrt(vec[0] * vec[0] +
                                 vec[1] * vec[1] +
                                 vec[2] * vec[3]);
  vec[0] = vec[0] / length;
  vec[1] = vec[1] / length;
  vec[2] = vec[2] / length;
}

inline void normalize(float& x, float& y, float& z) {
  const float length = std::sqrt(x * x +
                                 y * y +
                                 z * z);
  x /= length;
  y /= length;
  z /= length;
}

}  // namespace math

template <class DType, class InternalIndexing>
class ReservedArray {
 private:
  inline static const InternalIndexing DEFAULT_LENGTH = 10000;
  inline static const InternalIndexing INCLEMENT_LENGTH = 10000;
  DType* _array;
  InternalIndexing _reservedLength;
  InternalIndexing _lastIndex;

 private:
  void extend(const InternalIndexing& newLength) {
    DType* newArray = (DType*)calloc(newLength, sizeof(DType));

    if (_array != nullptr) {
      // Copy
      for (InternalIndexing index = 0; index < _reservedLength; ++index) {
        newArray[index] = _array[index];
      }
    }

    _reservedLength = newLength;
    _array = newArray;
  }

 public:
  ReservedArray()
      : _array(nullptr),
        _reservedLength(DEFAULT_LENGTH),
        _lastIndex(-1) {
    extend(_reservedLength);
  }
  ReservedArray(const InternalIndexing reserveLength)
      : _array(nullptr),
        _reservedLength(reserveLength),
        _lastIndex(-1) {
    extend(_reservedLength);
  }

  ~ReservedArray() { free(_array); }

  InternalIndexing getLength() const { return _lastIndex; }

  InternalIndexing getReservedLength() const { return _reservedLength; }

  DType getValue(const InternalIndexing& index) const { return _array[index]; }

  void setValue(const InternalIndexing& index, const DType& value) {
    if (index >= _reservedLength) {
      const InternalIndexing nInclements = (index - _reservedLength) / INCLEMENT_LENGTH + 1;
      const InternalIndexing newLength = _reservedLength + nInclements * INCLEMENT_LENGTH;
      extend(newLength);
    }

    if (index > _lastIndex) {
      _lastIndex = index;
    }

    _array[index] = value;
  }

  void append(const DType& value) {
    if (_lastIndex == _reservedLength - 1) {
      const InternalIndexing newLength = _reservedLength + INCLEMENT_LENGTH;
      extend(newLength);
    }

    _array[++_lastIndex] = value;
  }

  std::string toString() const {
    std::string buffer = "";

    for (InternalIndexing index = 0; index < _lastIndex + 1; ++index) {
      buffer += " " + std::to_string(_array[index]);
    }

    return buffer;
  }
};

using ReservedArrayf = ReservedArray<float, int_fast32_t>;
using ReservedArrayi = ReservedArray<int, int_fast32_t>;
using ReservedArrayd = ReservedArray<double, int_fast32_t>;

}  // namespace simview