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