#pragma once

/**
 * @file llas.hpp
 * @author Shinei Arakawa (sarakawalab@gmail.com)
 * @brief A header-only las import library.
 * @version 0.1
 * @date 2024-04-23
 *
 * @copyright Copyright (c) 2024
 *
 * [LICENCE]
 *   MIT License
 *
 *   Copyright (c) 2024 Shinei Arakawa
 *
 *   Permission is hereby granted, free of charge, to any person obtaining a copy
 *   of this software and associated documentation files (the "Software"), to deal
 *   in the Software without restriction, including without limitation the rights
 *   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *   copies of the Software, and to permit persons to whom the Software is
 *   furnished to do so, subject to the following conditions:
 *
 *   The above copyright notice and this permission notice shall be included in all
 *   copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *   SOFTWARE.
 *
 * [Readme]
 *   This implementation is based on 'ASPRS LAS 1.4 Format Specification R15 July 9 2019'
 *   Link is "https://www.asprs.org/wp-content/uploads/2019/07/LAS_1_4_r15.pdf"
 */

#ifndef __LLAS_HPP__
#define __LLAS_HPP__

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#if defined(LLAS_STATIC)
#define LLAS_FUNC_DECL_PREFIX static
#else
#define LLAS_FUNC_DECL_PREFIX inline
#endif

// ==========================================================================
// Type defines
// ==========================================================================
// clang-format off
#define LLAS_CHAR             char         // 1
#define LLAS_SCHAR            int8_t       // 1
#define LLAS_UCHAR            uint8_t      // 1
#define LLAS_SHORT            int16_t      // 2
#define LLAS_USHORT           uint16_t     // 2
#define LLAS_LONG             int32_t      // 4
#define LLAS_ULONG            uint32_t     // 4
#define LLAS_LLONG            int64_t      // 8
#define LLAS_ULLONG           uint64_t     // 8
#define LLAS_FLOAT            float        // 4
#define LLAS_DOUBLE           double       // 8
#define LLAS_STRING           char*        // -
// clang-format on

#define LLAS_BUFFER_SIZE 1024

// ==========================================================================
// Simple logging macro
// ==========================================================================

#if defined(LLAS_LOG_DEBUG)
#define _LLAS_logDebug(message) (std::cout << "[DEBUG] " << __FILE__ << ":" << __LINE__ << " " << message << std::endl)
#else
#define _LLAS_logDebug(message)
#endif

#if defined(LLAS_LOG_DEBUG) || defined(LLAS_LOG_INFO)
#define _LLAS_logInfo(message) (std::cout << "[INFO] " << __FILE__ << ":" << __LINE__ << " " << message << std::endl)
#else
#define _LLAS_logInfo(message)
#endif

#define _LLAS_logError(message) (std::cerr << "[ERROR] " << __FILE__ << ":" << __LINE__ << " " << message << std::endl)

namespace llas {
// ==========================================================================
// Utility Functions
// ==========================================================================

LLAS_FUNC_DECL_PREFIX void _readBytes(std::ifstream& ifs,
                                      std::vector<char>& bytes,
                                      const std::streamsize& nBytes) {
  ifs.read(bytes.data(), nBytes);
}

// ==========================================================================
// Math utility
// ==========================================================================
namespace math {
template <class dtype>
using vec4_t = std::array<dtype, 4>;
using vec4f_t = vec4_t<float>;
using vec4d_t = vec4_t<double>;
using vec4i_t = vec4_t<int>;

template <class dtype>
using vec3_t = std::array<dtype, 3>;
using vec3f_t = vec3_t<float>;
using vec3d_t = vec3_t<double>;
using vec3i_t = vec3_t<int>;

template <class dtype>
using vec2_t = std::array<dtype, 2>;
using vec2f_t = vec2_t<float>;
using vec2d_t = vec2_t<double>;
using vec2i_t = vec2_t<int>;

template <class dtype>
using vec_t = std::vector<dtype>;
using vecf_t = vec_t<float>;
using vecd_t = vec_t<double>;
using veci_t = vec_t<int>;

template <class dtype>
using vec_pt = std::shared_ptr<vec_t<dtype>>;
using vecf_pt = vec_pt<float>;
using vecd_pt = vec_pt<double>;
using veci_pt = vec_pt<int>;

LLAS_FUNC_DECL_PREFIX float innerProduct(const vec3f_t& vec0,
                                         const vec3f_t& vec1) {
  return vec0[0] * vec1[0] + vec0[1] * vec1[1] + vec0[2] * vec1[2];
}

LLAS_FUNC_DECL_PREFIX vec3f_t outerProduct(const vec3f_t& vec0,
                                           const vec3f_t& vec1) {
  return {
      vec0[1] * vec1[2] - vec0[2] * vec1[1],  // vec0.y * vec1.z - vec0.z * vec1.y
      vec0[2] * vec1[0] - vec0[0] * vec1[2],  // vec0.z * vec1.x - vec0.x * vec1.z
      vec0[0] * vec1[1] - vec0[1] * vec1[0]   // vec0.x * vec1.y - vec0.y * vec1.x
  };
}

LLAS_FUNC_DECL_PREFIX void outerProduct(const float& x0, const float& y0, const float& z0,
                                        const float& x1, const float& y1, const float& z1,
                                        float& x, float& y, float& z) {
  x = y0 * z1 - z0 * y1;
  y = z0 * x1 - x0 * z1;
  z = x0 * y1 - y0 * x1;
}

LLAS_FUNC_DECL_PREFIX float length(vec3f_t& vec) {
  return std::sqrt(vec[0] * vec[0] +
                   vec[1] * vec[1] +
                   vec[2] * vec[2]);
}

LLAS_FUNC_DECL_PREFIX void normalize(vec3f_t& vec) {
  const float len = length(vec);
  vec[0] = vec[0] / len;
  vec[1] = vec[1] / len;
  vec[2] = vec[2] / len;
}

LLAS_FUNC_DECL_PREFIX void normalize(float& x, float& y, float& z) {
  const float length = std::sqrt(x * x +
                                 y * y +
                                 z * z);
  x /= length;
  y /= length;
  z /= length;
}

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// vec4 operators
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// #######################################
// Plus
// #######################################
// vector + vector
template <class DType>
vec4_t<DType> operator+(const vec4_t<DType>& left, const vec4_t<DType>& right) {
  vec4_t<DType> result;
  result[0] = left[0] + right[0];
  result[1] = left[1] + right[1];
  result[2] = left[2] + right[2];
  result[3] = left[3] + right[3];
  return result;
}

// vector + scalar
template <class DType>
vec4_t<DType> operator+(const vec4_t<DType>& left, const DType& right) {
  vec4_t<DType> result;
  result[0] = left[0] + right;
  result[1] = left[1] + right;
  result[2] = left[2] + right;
  result[3] = left[3] + right;
  return result;
}

// scalar + vector
template <class DType>
vec4_t<DType> operator+(const DType& left, const vec4_t<DType>& right) {
  return right + left;
}

// #######################################
// Minus
// #######################################
// vector - vector
template <class DType>
vec4_t<DType> operator-(const vec4_t<DType>& left, const vec4_t<DType>& right) {
  vec4_t<DType> result;
  result[0] = left[0] - right[0];
  result[1] = left[1] - right[1];
  result[2] = left[2] - right[2];
  result[3] = left[3] - right[3];
  return result;
}

// vector - scalar
template <class DType>
vec4_t<DType> operator-(const vec4_t<DType>& left, const DType& right) {
  vec4_t<DType> result;
  result[0] = left[0] - right;
  result[1] = left[1] - right;
  result[2] = left[2] - right;
  result[3] = left[3] - right;
  return result;
}

// scalar - vector
template <class DType>
vec4_t<DType> operator-(const DType& left, const vec4_t<DType>& right) {
  vec4_t<DType> result;
  result[0] = left - right[0];
  result[1] = left - right[1];
  result[2] = left - right[2];
  result[3] = left - right[3];
  return result;
}

// #######################################
// Mult
// #######################################
// vector * vector
template <class DType>
vec4_t<DType> operator*(const vec4_t<DType>& left, const vec4_t<DType>& right) {
  vec4_t<DType> result;
  result[0] = left[0] * right[0];
  result[1] = left[1] * right[1];
  result[2] = left[2] * right[2];
  result[3] = left[3] * right[3];
  return result;
}

// vector * scalar
template <class DType>
vec4_t<DType> operator*(const vec4_t<DType>& left, const DType& right) {
  vec4_t<DType> result;
  result[0] = left[0] * right;
  result[1] = left[1] * right;
  result[2] = left[2] * right;
  result[3] = left[3] * right;
  return result;
}

// scalar * vector
template <class DType>
vec4_t<DType> operator*(const DType& left, const vec4_t<DType>& right) {
  return right * left;
}

// #######################################
// Div
// #######################################
// vector / vector
template <class DType>
vec4_t<DType> operator/(const vec4_t<DType>& left, const vec4_t<DType>& right) {
  vec4_t<DType> result;
  result[0] = left[0] / right[0];
  result[1] = left[1] / right[1];
  result[2] = left[2] / right[2];
  result[3] = left[3] / right[3];
  return result;
}

// vector / scalar
template <class DType>
vec4_t<DType> operator/(const vec4_t<DType>& left, const DType& right) {
  vec4_t<DType> result;
  result[0] = left[0] / right;
  result[1] = left[1] / right;
  result[2] = left[2] / right;
  result[3] = left[3] / right;
  return result;
}

// scalar / vector
template <class DType>
vec4_t<DType> operator/(const DType& left, const vec4_t<DType>& right) {
  vec4_t<DType> result;
  result[0] = left / right[0];
  result[1] = left / right[1];
  result[2] = left / right[2];
  result[3] = left / right[3];
  return result;
}

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// vec3 operators
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// #######################################
// Plus
// #######################################
// vector + vector
template <class DType>
vec3_t<DType> operator+(const vec3_t<DType>& left, const vec3_t<DType>& right) {
  vec3_t<DType> result;
  result[0] = left[0] + right[0];
  result[1] = left[1] + right[1];
  result[2] = left[2] + right[2];
  return result;
}

// vector + scalar
template <class DType>
vec3_t<DType> operator+(const vec3_t<DType>& left, const DType& right) {
  vec3_t<DType> result;
  result[0] = left[0] + right;
  result[1] = left[1] + right;
  result[2] = left[2] + right;
  return result;
}

// scalar + vector
template <class DType>
vec3_t<DType> operator+(const DType& left, const vec3_t<DType>& right) {
  return right + left;
}

// #######################################
// Minus
// #######################################
// vector - vector
template <class DType>
vec3_t<DType> operator-(const vec3_t<DType>& left, const vec3_t<DType>& right) {
  vec3_t<DType> result;
  result[0] = left[0] - right[0];
  result[1] = left[1] - right[1];
  result[2] = left[2] - right[2];
  return result;
}

// vector - scalar
template <class DType>
vec3_t<DType> operator-(const vec3_t<DType>& left, const DType& right) {
  vec3_t<DType> result;
  result[0] = left[0] - right;
  result[1] = left[1] - right;
  result[2] = left[2] - right;
  return result;
}

// scalar - vector
template <class DType>
vec3_t<DType> operator-(const DType& left, const vec3_t<DType>& right) {
  vec3_t<DType> result;
  result[0] = left - right[0];
  result[1] = left - right[1];
  result[2] = left - right[2];
  return result;
}

// #######################################
// Mult
// #######################################
// vector * vector
template <class DType>
vec3_t<DType> operator*(const vec3_t<DType>& left, const vec3_t<DType>& right) {
  vec3_t<DType> result;
  result[0] = left[0] * right[0];
  result[1] = left[1] * right[1];
  result[2] = left[2] * right[2];
  return result;
}

// vector * scalar
template <class DType>
vec3_t<DType> operator*(const vec3_t<DType>& left, const DType& right) {
  vec3_t<DType> result;
  result[0] = left[0] * right;
  result[1] = left[1] * right;
  result[2] = left[2] * right;
  return result;
}

// scalar * vector
template <class DType>
vec3_t<DType> operator*(const DType& left, const vec3_t<DType>& right) {
  return right * left;
}

// #######################################
// Div
// #######################################
// vector / vector
template <class DType>
vec3_t<DType> operator/(const vec3_t<DType>& left, const vec3_t<DType>& right) {
  vec3_t<DType> result;
  result[0] = left[0] / right[0];
  result[1] = left[1] / right[1];
  result[2] = left[2] / right[2];
  return result;
}

// vector / scalar
template <class DType>
vec3_t<DType> operator/(const vec3_t<DType>& left, const DType& right) {
  vec3_t<DType> result;
  result[0] = left[0] / right;
  result[1] = left[1] / right;
  result[2] = left[2] / right;
  return result;
}

// scalar / vector
template <class DType>
vec3_t<DType> operator/(const DType& left, const vec3_t<DType>& right) {
  vec3_t<DType> result;
  result[0] = left / right[0];
  result[1] = left / right[1];
  result[2] = left / right[2];
  return result;
}

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// vec2 operators
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// #######################################
// Plus
// #######################################
// vector + vector
template <class DType>
vec2_t<DType> operator+(const vec2_t<DType>& left, const vec2_t<DType>& right) {
  vec2_t<DType> result;
  result[0] = left[0] + right[0];
  result[1] = left[1] + right[1];
  return result;
}

// vector + scalar
template <class DType>
vec2_t<DType> operator+(const vec2_t<DType>& left, const DType& right) {
  vec2_t<DType> result;
  result[0] = left[0] + right;
  result[1] = left[1] + right;
  return result;
}

// scalar + vector
template <class DType>
vec2_t<DType> operator+(const DType& left, const vec2_t<DType>& right) {
  return right + left;
}

// #######################################
// Minus
// #######################################
// vector - vector
template <class DType>
vec2_t<DType> operator-(const vec2_t<DType>& left, const vec2_t<DType>& right) {
  vec2_t<DType> result;
  result[0] = left[0] - right[0];
  result[1] = left[1] - right[1];
  return result;
}

// vector - scalar
template <class DType>
vec2_t<DType> operator-(const vec2_t<DType>& left, const DType& right) {
  vec2_t<DType> result;
  result[0] = left[0] - right;
  result[1] = left[1] - right;
  return result;
}

// scalar - vector
template <class DType>
vec2_t<DType> operator-(const DType& left, const vec2_t<DType>& right) {
  vec2_t<DType> result;
  result[0] = left - right[0];
  result[1] = left - right[1];
  return result;
}

// #######################################
// Mult
// #######################################
// vector * vector
template <class DType>
vec2_t<DType> operator*(const vec2_t<DType>& left, const vec2_t<DType>& right) {
  vec2_t<DType> result;
  result[0] = left[0] * right[0];
  result[1] = left[1] * right[1];
  return result;
}

// vector * scalar
template <class DType>
vec2_t<DType> operator*(const vec2_t<DType>& left, const DType& right) {
  vec2_t<DType> result;
  result[0] = left[0] * right;
  result[1] = left[1] * right;
  return result;
}

// scalar * vector
template <class DType>
vec2_t<DType> operator*(const DType& left, const vec2_t<DType>& right) {
  return right * left;
}

// #######################################
// Div
// #######################################
// vector / vector
template <class DType>
vec2_t<DType> operator/(const vec2_t<DType>& left, const vec2_t<DType>& right) {
  vec2_t<DType> result;
  result[0] = left[0] / right[0];
  result[1] = left[1] / right[1];
  return result;
}

// vector / scalar
template <class DType>
vec2_t<DType> operator/(const vec2_t<DType>& left, const DType& right) {
  vec2_t<DType> result;
  result[0] = left[0] / right;
  result[1] = left[1] / right;
  return result;
}

// scalar / vector
template <class DType>
vec2_t<DType> operator/(const DType& left, const vec2_t<DType>& right) {
  vec2_t<DType> result;
  result[0] = left / right[0];
  result[1] = left / right[1];
  return result;
}
}  // namespace math

// ==========================================================================
// Data structure
// ==========================================================================
struct PublicHeader {
  // clang-format off
  inline static const std::streamsize NUM_BYTES_FILE_SIGNATURE                                      = 4;
  inline static const std::streamsize NUM_BYTES_FILE_SOURCE_ID                                      = 2;
  inline static const std::streamsize NUM_BYTES_GLOBAL_ENCODING                                     = 2;
  inline static const std::streamsize NUM_BYTES_PROJECT_ID_1                                        = 4;
  inline static const std::streamsize NUM_BYTES_PROJECT_ID_2                                        = 2;
  inline static const std::streamsize NUM_BYTES_PROJECT_ID_3                                        = 2;
  inline static const std::streamsize NUM_BYTES_PROJECT_ID_4                                        = 8;
  inline static const std::streamsize NUM_BYTES_VERSION_MAJOR                                       = 1;
  inline static const std::streamsize NUM_BYTES_VERSION_MINOR                                       = 1;
  inline static const std::streamsize NUM_BYTES_SYSTEM_IDENTIFIER                                   = 32;
  inline static const std::streamsize NUM_BYTES_GENERATING_SOFTWARE                                 = 32;
  inline static const std::streamsize NUM_BYTES_FILE_CREATION_DAY_OF_YEAR                           = 2;
  inline static const std::streamsize NUM_BYTES_FILE_CREATION_YEAR                                  = 2;
  inline static const std::streamsize NUM_BYTES_HEADER_SIZE                                         = 2;
  inline static const std::streamsize NUM_BYTES_OFFSET_TO_POINT_DATA                                = 4;
  inline static const std::streamsize NUM_BYTES_NUM_OF_VARIABLE_LENGTH_RECORDS                      = 4;
  inline static const std::streamsize NUM_BYTES_POINT_DATA_RECORD_FORMAT                            = 1;
  inline static const std::streamsize NUM_BYTES_POINT_DATA_RECORD_LENGTH                            = 2;
  inline static const std::streamsize NUM_BYTES_LEGACY_NUM_OF_POINT_RECORDS                         = 4;
  inline static const std::streamsize NUM_BYTES_LEGACY_NUM_OF_POINT_BY_RETURN                       = 20;
  inline static const std::streamsize NUM_BYTES_X_SCALE_FACTOR                                      = 8;
  inline static const std::streamsize NUM_BYTES_Y_SCALE_FACTOR                                      = 8;
  inline static const std::streamsize NUM_BYTES_Z_SCALE_FACTOR                                      = 8;
  inline static const std::streamsize NUM_BYTES_X_OFFSET                                            = 8;
  inline static const std::streamsize NUM_BYTES_Y_OFFSET                                            = 8;
  inline static const std::streamsize NUM_BYTES_Z_OFFSET                                            = 8;
  inline static const std::streamsize NUM_BYTES_MAX_X                                               = 8;
  inline static const std::streamsize NUM_BYTES_MAX_Y                                               = 8;
  inline static const std::streamsize NUM_BYTES_MAX_Z                                               = 8;
  inline static const std::streamsize NUM_BYTES_MIN_X                                               = 8;
  inline static const std::streamsize NUM_BYTES_MIN_Y                                               = 8;
  inline static const std::streamsize NUM_BYTES_MIN_Z                                               = 8;
  inline static const std::streamsize NUM_BYTES_START_OF_WAVEFORM_DATA_PACKET_RECORD                = 8;
  inline static const std::streamsize NUM_BYTES_START_OF_FIRST_EXTENDED_VARIABLE_LENGTH_RECORD      = 8;
  inline static const std::streamsize NUM_BYTES_NUM_OF_EXTENDED_VARIABLE_LENGTH_RECORDS             = 4;
  inline static const std::streamsize NUM_BYTES_NUM_OF_POINT_RECORDS                                = 8;
  inline static const std::streamsize NUM_BYTES_NUM_OF_POINTS_BY_RETURN                             = 120;
  // clang-format on

  PublicHeader()
      : fileSignature(),
        fileSourceID(),
        globalEncoding(),
        projectID1(),
        projectID2(),
        projectID3(),
        projectID4(),
        versionMajor(),
        versionMinor(),
        systemIdentifier(),
        generatingSoftware(),
        fileCreationDayOfYear(),
        fileCreationYear(),
        headerSize(),
        offsetToPointData(),
        numOfVariableLengthRecords(),
        pointDataRecordFormat(),
        pointDataRecordLength(),
        legacyNumOfPointRecords(),
        legacyNumOfPointByReturn(),
        xScaleFactor(),
        yScaleFactor(),
        zScaleFactor(),
        xOffset(),
        yOffset(),
        zOffset(),
        maxX(),
        maxY(),
        maxZ(),
        minX(),
        minY(),
        minZ(),
        startOfWaveformDataPacketRecord(),
        startOfFirstExtendedVariableLengthRecord(),
        numOfExtendedVariableLengthRecords(),
        numOfPointRecords(),
        numOfPointsByReturn(),
        hasStartOfWaveformDataPacketRecord(),
        hasStartOfFirstExtendedVariableLengthRecord(),
        hasNumOfExtendedVariableLengthRecords(),
        hasNumOfPointRecords(),
        hasNumOfPointsByReturn() {
  }

  // clang-format off
  LLAS_CHAR      fileSignature[NUM_BYTES_FILE_SIGNATURE + 1];           // plus null-termination
  LLAS_USHORT    fileSourceID;
  LLAS_USHORT    globalEncoding;
  LLAS_ULONG     projectID1;
  LLAS_USHORT    projectID2;
  LLAS_USHORT    projectID3;
  LLAS_CHAR      projectID4[NUM_BYTES_PROJECT_ID_4 + 1];                // plus null-termination
  LLAS_UCHAR     versionMajor;
  LLAS_UCHAR     versionMinor;
  LLAS_CHAR      systemIdentifier[NUM_BYTES_SYSTEM_IDENTIFIER + 1];     // plus null-termination
  LLAS_CHAR      generatingSoftware[NUM_BYTES_GENERATING_SOFTWARE + 1]; // plus null-termination
  LLAS_USHORT    fileCreationDayOfYear;
  LLAS_USHORT    fileCreationYear;
  LLAS_USHORT    headerSize;
  LLAS_ULONG     offsetToPointData;
  LLAS_ULONG     numOfVariableLengthRecords;
  LLAS_UCHAR     pointDataRecordFormat;
  LLAS_USHORT    pointDataRecordLength;
  LLAS_ULONG     legacyNumOfPointRecords;
  LLAS_ULONG     legacyNumOfPointByReturn[NUM_BYTES_LEGACY_NUM_OF_POINT_BY_RETURN / sizeof(LLAS_ULONG)];
  LLAS_DOUBLE    xScaleFactor;
  LLAS_DOUBLE    yScaleFactor;
  LLAS_DOUBLE    zScaleFactor;
  LLAS_DOUBLE    xOffset;
  LLAS_DOUBLE    yOffset;
  LLAS_DOUBLE    zOffset;
  LLAS_DOUBLE    maxX;
  LLAS_DOUBLE    maxY;
  LLAS_DOUBLE    maxZ;
  LLAS_DOUBLE    minX;
  LLAS_DOUBLE    minY;
  LLAS_DOUBLE    minZ;
  LLAS_ULLONG    startOfWaveformDataPacketRecord;
  LLAS_ULLONG    startOfFirstExtendedVariableLengthRecord;
  LLAS_ULONG     numOfExtendedVariableLengthRecords;
  LLAS_ULLONG    numOfPointRecords;
  LLAS_ULLONG    numOfPointsByReturn[NUM_BYTES_NUM_OF_POINTS_BY_RETURN / sizeof(LLAS_ULLONG)];

  bool hasStartOfWaveformDataPacketRecord;
  bool hasStartOfFirstExtendedVariableLengthRecord;
  bool hasNumOfExtendedVariableLengthRecords;
  bool hasNumOfPointRecords;
  bool hasNumOfPointsByReturn;
  // clang-format on

  static PublicHeader readPublicHeader(const std::vector<char>& fileBytes) {
    PublicHeader publicHeader;
    size_t offset = 0;

    {
      // File signature
      const std::streamsize nBytes = PublicHeader::NUM_BYTES_FILE_SIGNATURE;
      std::memcpy(&publicHeader.fileSignature, fileBytes.data() + offset, nBytes);
      offset += nBytes;
    }

    {
      // File source ID
      const std::streamsize nBytes = PublicHeader::NUM_BYTES_FILE_SOURCE_ID;
      std::memcpy(&publicHeader.fileSourceID, fileBytes.data() + offset, nBytes);
      offset += nBytes;
    }

    {
      // Global encoding
      const std::streamsize nBytes = PublicHeader::NUM_BYTES_GLOBAL_ENCODING;
      std::memcpy(&publicHeader.globalEncoding, fileBytes.data() + offset, nBytes);
      offset += nBytes;
    }

    {
      // Project ID-GUID Data 1
      const std::streamsize nBytes = PublicHeader::NUM_BYTES_PROJECT_ID_1;
      std::memcpy(&publicHeader.projectID1, fileBytes.data() + offset, nBytes);
      offset += nBytes;
    }

    {
      // Project ID-GUID Data 2
      const std::streamsize nBytes = PublicHeader::NUM_BYTES_PROJECT_ID_2;
      std::memcpy(&publicHeader.projectID2, fileBytes.data() + offset, nBytes);
      offset += nBytes;
    }

    {
      // Project ID-GUID Data 3
      const std::streamsize nBytes = PublicHeader::NUM_BYTES_PROJECT_ID_3;
      std::memcpy(&publicHeader.projectID3, fileBytes.data() + offset, nBytes);
      offset += nBytes;
    }

    {
      // Project ID-GUID Data 4
      const std::streamsize nBytes = PublicHeader::NUM_BYTES_PROJECT_ID_4;
      std::memcpy(&publicHeader.projectID4, fileBytes.data() + offset, nBytes);
      offset += nBytes;
    }

    {
      // Version major
      const std::streamsize nBytes = PublicHeader::NUM_BYTES_VERSION_MAJOR;
      std::memcpy(&publicHeader.versionMajor, fileBytes.data() + offset, nBytes);
      offset += nBytes;
    }

    {
      // Version minor
      const std::streamsize nBytes = PublicHeader::NUM_BYTES_VERSION_MINOR;
      std::memcpy(&publicHeader.versionMinor, fileBytes.data() + offset, nBytes);
      offset += nBytes;

      if (publicHeader.versionMinor >= 3) {
        publicHeader.hasStartOfWaveformDataPacketRecord = true;
      }

      if (publicHeader.versionMinor >= 4) {
        publicHeader.hasStartOfFirstExtendedVariableLengthRecord = true;
        publicHeader.hasNumOfExtendedVariableLengthRecords = true;
        publicHeader.hasNumOfPointRecords = true;
        publicHeader.hasNumOfPointsByReturn = true;
      }
    }

    {
      // System Identifier
      const std::streamsize nBytes = PublicHeader::NUM_BYTES_SYSTEM_IDENTIFIER;
      std::memcpy(&publicHeader.systemIdentifier, fileBytes.data() + offset, nBytes);
      offset += nBytes;
    }

    {
      // Generating Software
      const std::streamsize nBytes = PublicHeader::NUM_BYTES_GENERATING_SOFTWARE;
      std::memcpy(&publicHeader.generatingSoftware, fileBytes.data() + offset, nBytes);
      offset += nBytes;
    }

    {
      // File Creation Day of Year
      const std::streamsize nBytes = PublicHeader::NUM_BYTES_FILE_CREATION_DAY_OF_YEAR;
      std::memcpy(&publicHeader.fileCreationDayOfYear, fileBytes.data() + offset, nBytes);
      offset += nBytes;
    }

    {
      // File Creation Year
      const std::streamsize nBytes = PublicHeader::NUM_BYTES_FILE_CREATION_YEAR;
      std::memcpy(&publicHeader.fileCreationYear, fileBytes.data() + offset, nBytes);
      offset += nBytes;
    }

    {
      // Header size
      const std::streamsize nBytes = PublicHeader::NUM_BYTES_HEADER_SIZE;
      std::memcpy(&publicHeader.headerSize, fileBytes.data() + offset, nBytes);
      offset += nBytes;
    }

    {
      // Offset to point data
      const std::streamsize nBytes = PublicHeader::NUM_BYTES_OFFSET_TO_POINT_DATA;
      std::memcpy(&publicHeader.offsetToPointData, fileBytes.data() + offset, nBytes);
      offset += nBytes;
    }

    {
      // Number of Variable Length Records
      const std::streamsize nBytes = PublicHeader::NUM_BYTES_NUM_OF_VARIABLE_LENGTH_RECORDS;
      std::memcpy(&publicHeader.numOfVariableLengthRecords, fileBytes.data() + offset, nBytes);
      offset += nBytes;
    }

    {
      // Point Data Record Format
      const std::streamsize nBytes = PublicHeader::NUM_BYTES_POINT_DATA_RECORD_FORMAT;
      std::memcpy(&publicHeader.pointDataRecordFormat, fileBytes.data() + offset, nBytes);
      offset += nBytes;
    }

    {
      // Point Data Record Length
      const std::streamsize nBytes = PublicHeader::NUM_BYTES_POINT_DATA_RECORD_LENGTH;
      std::memcpy(&publicHeader.pointDataRecordLength, fileBytes.data() + offset, nBytes);
      offset += nBytes;
    }

    {
      // Legacy Number of Point Records
      const std::streamsize nBytes = PublicHeader::NUM_BYTES_LEGACY_NUM_OF_POINT_RECORDS;
      std::memcpy(&publicHeader.legacyNumOfPointRecords, fileBytes.data() + offset, nBytes);
      offset += nBytes;
    }

    {
      // Legacy Number of Point by Return
      const std::streamsize nBytes = PublicHeader::NUM_BYTES_LEGACY_NUM_OF_POINT_BY_RETURN;
      std::memcpy(&publicHeader.legacyNumOfPointByReturn, fileBytes.data() + offset, nBytes);
      offset += nBytes;
    }

    {
      // X Scale Factor
      const std::streamsize nBytes = PublicHeader::NUM_BYTES_X_SCALE_FACTOR;
      std::memcpy(&publicHeader.xScaleFactor, fileBytes.data() + offset, nBytes);
      offset += nBytes;
    }

    {
      // Y Scale Factor
      const std::streamsize nBytes = PublicHeader::NUM_BYTES_Y_SCALE_FACTOR;
      std::memcpy(&publicHeader.yScaleFactor, fileBytes.data() + offset, nBytes);
      offset += nBytes;
    }

    {
      // Z Scale Factor
      const std::streamsize nBytes = PublicHeader::NUM_BYTES_Z_SCALE_FACTOR;
      std::memcpy(&publicHeader.zScaleFactor, fileBytes.data() + offset, nBytes);
      offset += nBytes;
    }

    {
      // X Offset
      const std::streamsize nBytes = PublicHeader::NUM_BYTES_X_OFFSET;
      std::memcpy(&publicHeader.xOffset, fileBytes.data() + offset, nBytes);
      offset += nBytes;
    }

    {
      // Y Offset
      const std::streamsize nBytes = PublicHeader::NUM_BYTES_Y_OFFSET;
      std::memcpy(&publicHeader.yOffset, fileBytes.data() + offset, nBytes);
      offset += nBytes;
    }

    {
      // Z Offset
      const std::streamsize nBytes = PublicHeader::NUM_BYTES_Z_OFFSET;
      std::memcpy(&publicHeader.zOffset, fileBytes.data() + offset, nBytes);
      offset += nBytes;
    }

    {
      // Max X
      const std::streamsize nBytes = PublicHeader::NUM_BYTES_MAX_X;
      std::memcpy(&publicHeader.maxX, fileBytes.data() + offset, nBytes);
      offset += nBytes;
    }

    {
      // Min X
      const std::streamsize nBytes = PublicHeader::NUM_BYTES_MIN_X;
      std::memcpy(&publicHeader.minX, fileBytes.data() + offset, nBytes);
      offset += nBytes;
    }

    {
      // Max Y
      const std::streamsize nBytes = PublicHeader::NUM_BYTES_MAX_Y;
      std::memcpy(&publicHeader.maxY, fileBytes.data() + offset, nBytes);
      offset += nBytes;
    }

    {
      // Min Y
      const std::streamsize nBytes = PublicHeader::NUM_BYTES_MIN_Y;
      std::memcpy(&publicHeader.minY, fileBytes.data() + offset, nBytes);
      offset += nBytes;
    }

    {
      // Max Z
      const std::streamsize nBytes = PublicHeader::NUM_BYTES_MAX_Z;
      std::memcpy(&publicHeader.maxZ, fileBytes.data() + offset, nBytes);
      offset += nBytes;
    }

    {
      // Min Z
      const std::streamsize nBytes = PublicHeader::NUM_BYTES_MIN_Z;
      std::memcpy(&publicHeader.minZ, fileBytes.data() + offset, nBytes);
      offset += nBytes;
    }

    if (publicHeader.hasStartOfWaveformDataPacketRecord) {
      // Start of Waveform Data Packet Record
      const std::streamsize nBytes = PublicHeader::NUM_BYTES_START_OF_WAVEFORM_DATA_PACKET_RECORD;
      std::memcpy(&publicHeader.startOfWaveformDataPacketRecord, fileBytes.data() + offset, nBytes);
      offset += nBytes;
    }

    if (publicHeader.hasStartOfFirstExtendedVariableLengthRecord) {
      // Start of First Extended Variable Length Record
      const std::streamsize nBytes = PublicHeader::NUM_BYTES_START_OF_FIRST_EXTENDED_VARIABLE_LENGTH_RECORD;
      std::memcpy(&publicHeader.startOfFirstExtendedVariableLengthRecord, fileBytes.data() + offset, nBytes);
      offset += nBytes;
    }

    if (publicHeader.hasNumOfExtendedVariableLengthRecords) {
      // Number of Extended Variable Length Records
      const std::streamsize nBytes = PublicHeader::NUM_BYTES_NUM_OF_EXTENDED_VARIABLE_LENGTH_RECORDS;
      std::memcpy(&publicHeader.numOfExtendedVariableLengthRecords, fileBytes.data() + offset, nBytes);
      offset += nBytes;
    }

    if (publicHeader.hasNumOfPointRecords) {
      // Number of Point Records
      const std::streamsize nBytes = PublicHeader::NUM_BYTES_NUM_OF_POINT_RECORDS;
      std::memcpy(&publicHeader.numOfPointRecords, fileBytes.data() + offset, nBytes);
      offset += nBytes;
    }

    if (publicHeader.hasNumOfPointsByReturn) {
      // Number of Points by Return
      const std::streamsize nBytes = PublicHeader::NUM_BYTES_NUM_OF_POINTS_BY_RETURN;
      std::memcpy(&publicHeader.numOfPointsByReturn, fileBytes.data() + offset, nBytes);
      offset += nBytes;
    }

    return publicHeader;
  };
};

struct VariableLengthRecord {
  // clang-format off
  inline static const std::streamsize NUM_BYTES_RESERVED                                            = 2;
  inline static const std::streamsize NUM_BYTES_USER_ID                                             = 16;
  inline static const std::streamsize NUM_BYTES_RECORD_ID                                           = 2;
  inline static const std::streamsize NUM_BYTES_RECORD_LENGTH_AFTER_HEADER                          = 2;
  inline static const std::streamsize NUM_BYTES_DESCPIPTION                                         = 32;
  // clang-format on

  VariableLengthRecord()
      : reserved(),
        userID(),
        recordID(),
        recordLengthAfterHeader(),
        description(),
        record() {}

  // clang-format off
  LLAS_UCHAR        reserved;
  LLAS_CHAR         userID[NUM_BYTES_USER_ID + 1];          // plus null-termination
  LLAS_USHORT       recordID;
  LLAS_USHORT       recordLengthAfterHeader;
  LLAS_CHAR         description[NUM_BYTES_DESCPIPTION + 1]; // plus null-termination
  std::vector<char> record;
  // clang-format on

  static VariableLengthRecord readVariableLengthRecord(const std::vector<char>& fileBytes,
                                                       std::streamsize& offset) {
    VariableLengthRecord vlr;

    {
      // Reserved
      const std::streamsize nBytes = VariableLengthRecord::NUM_BYTES_RESERVED;
      std::memcpy(&vlr.reserved, fileBytes.data() + offset, nBytes);
      offset += nBytes;
    }

    {
      // User ID
      const std::streamsize nBytes = VariableLengthRecord::NUM_BYTES_USER_ID;
      std::memcpy(&vlr.userID, fileBytes.data() + offset, nBytes);
      offset += nBytes;
    }

    {
      // Record ID
      const std::streamsize nBytes = VariableLengthRecord::NUM_BYTES_RECORD_ID;
      std::memcpy(&vlr.recordID, fileBytes.data() + offset, nBytes);
      offset += nBytes;
    }

    {
      // Record Length After Header
      const std::streamsize nBytes = VariableLengthRecord::NUM_BYTES_RECORD_LENGTH_AFTER_HEADER;
      std::memcpy(&vlr.recordLengthAfterHeader, fileBytes.data() + offset, nBytes);
      offset += nBytes;
    }

    {
      // Description
      const std::streamsize nBytes = VariableLengthRecord::NUM_BYTES_DESCPIPTION;
      std::memcpy(&vlr.description, fileBytes.data() + offset, nBytes);
      offset += nBytes;
    }

    if (vlr.recordLengthAfterHeader <= 65535) {
      // Record
      const std::streamsize nBytes = (std::streamsize)vlr.recordLengthAfterHeader;
      vlr.record.resize(nBytes);
      vlr.record.assign(fileBytes.begin() + offset, fileBytes.begin() + offset + nBytes);
      offset += nBytes;
    } else {
      _LLAS_logError("Exceed the payload limit of variable length record: " << vlr.recordLengthAfterHeader);
    }

    return vlr;
  }
};

struct PointDataRecord {
  // clang-format off
  inline static const std::streamsize NUM_BYTES_X                                                   = 4;
  inline static const std::streamsize NUM_BYTES_Y                                                   = 4;
  inline static const std::streamsize NUM_BYTES_Z                                                   = 4;
  inline static const std::streamsize NUM_BYTES_INTENSITY                                           = 2;
  inline static const std::streamsize NUM_BYTES_SENSOR_DATA                                         = 1;
  inline static const std::streamsize NUM_BYTES_CLASSIFICATION                                      = 1;
  inline static const std::streamsize NUM_BYTES_SCAN_ANGLE_RANK                                     = 1;
  inline static const std::streamsize NUM_BYTES_USER_DATA                                           = 1;
  inline static const std::streamsize NUM_BYTES_POINT_SOURCE_ID                                     = 2;
  inline static const std::streamsize NUM_BYTES_GPS_TIME                                            = 8;
  inline static const std::streamsize NUM_BYTES_RED                                                 = 2;
  inline static const std::streamsize NUM_BYTES_GREEN                                               = 2;
  inline static const std::streamsize NUM_BYTES_BLUE                                                = 2;
  // clang-format on

  PointDataRecord()
      : x(),
        y(),
        z(),
        intensity(),
        classification(),
        scanAngleRank(),
        userData(),
        pointSourceID(),
        GPSTime(),
        red(),
        green(),
        blue() {
  }

  // clang-format off
  LLAS_LONG      x;
  LLAS_LONG      y;
  LLAS_LONG      z;
  LLAS_USHORT    intensity;
  LLAS_UCHAR     classification;
  LLAS_SCHAR     scanAngleRank;
  LLAS_UCHAR     userData;
  LLAS_USHORT    pointSourceID;
  LLAS_DOUBLE    GPSTime;
  LLAS_USHORT    red;
  LLAS_USHORT    green;
  LLAS_USHORT    blue;
  // clang-format on

  static PointDataRecord _readPointDataRecordFotmat0to4(const std::vector<char>& fileBytes,
                                                        std::streamsize& offset,
                                                        const LLAS_UCHAR& format) {
    PointDataRecord pointDataRecord;

    {
      // X
      const std::streamsize nBytes = PointDataRecord::NUM_BYTES_X;
      std::memcpy(&pointDataRecord.x, fileBytes.data() + offset, nBytes);
      offset += nBytes;
    }

    {
      // Y
      const std::streamsize nBytes = PointDataRecord::NUM_BYTES_Y;
      std::memcpy(&pointDataRecord.y, fileBytes.data() + offset, nBytes);
      offset += nBytes;
    }

    {
      // Z
      const std::streamsize nBytes = PointDataRecord::NUM_BYTES_Z;
      std::memcpy(&pointDataRecord.z, fileBytes.data() + offset, nBytes);
      offset += nBytes;
    }

    {
      // Intensity
      const std::streamsize nBytes = PointDataRecord::NUM_BYTES_INTENSITY;
      std::memcpy(&pointDataRecord.intensity, fileBytes.data() + offset, nBytes);
      offset += nBytes;
    }

    {
      // Sensor Data
      const std::streamsize nBytes = PointDataRecord::NUM_BYTES_SENSOR_DATA;
      // TODO: set variables
      offset += nBytes;
    }

    {
      // Classification
      const std::streamsize nBytes = PointDataRecord::NUM_BYTES_CLASSIFICATION;
      std::memcpy(&pointDataRecord.classification, fileBytes.data() + offset, nBytes);
      offset += nBytes;
    }

    {
      // Scan Angle Rank
      const std::streamsize nBytes = PointDataRecord::NUM_BYTES_SCAN_ANGLE_RANK;
      std::memcpy(&pointDataRecord.scanAngleRank, fileBytes.data() + offset, nBytes);
      offset += nBytes;
    }

    {
      // User Data
      const std::streamsize nBytes = PointDataRecord::NUM_BYTES_USER_DATA;
      std::memcpy(&pointDataRecord.userData, fileBytes.data() + offset, nBytes);
      offset += nBytes;
    }

    {
      // Point Soruce ID
      const std::streamsize nBytes = PointDataRecord::NUM_BYTES_POINT_SOURCE_ID;
      std::memcpy(&pointDataRecord.pointSourceID, fileBytes.data() + offset, nBytes);
      offset += nBytes;
    }

    if (format == 1 || format == 3 || format == 4) {
      // GPS Time
      const std::streamsize nBytes = PointDataRecord::NUM_BYTES_GPS_TIME;
      std::memcpy(&pointDataRecord.GPSTime, fileBytes.data() + offset, nBytes);
      offset += nBytes;
    }

    if (format == 2 || format == 3) {
      {
        // Red
        const std::streamsize nBytes = PointDataRecord::NUM_BYTES_RED;
        std::memcpy(&pointDataRecord.red, fileBytes.data() + offset, nBytes);
        offset += nBytes;
      }

      {
        // Green
        const std::streamsize nBytes = PointDataRecord::NUM_BYTES_GREEN;
        std::memcpy(&pointDataRecord.green, fileBytes.data() + offset, nBytes);
        offset += nBytes;
      }

      {
        // Blue
        const std::streamsize nBytes = PointDataRecord::NUM_BYTES_BLUE;
        std::memcpy(&pointDataRecord.blue, fileBytes.data() + offset, nBytes);
        offset += nBytes;
      }
    }

    return pointDataRecord;
  }

  static PointDataRecord _readPointDataRecordFotmat5to15(const std::vector<char>& fileBytes,
                                                         std::streamsize& offset,
                                                         const LLAS_UCHAR& format) {
    _LLAS_logError("Unsupported point data record format: " + std::to_string((int)format));
    PointDataRecord pointDataRecord;
    return pointDataRecord;
  }

  static PointDataRecord readPointDataRecord(const std::vector<char>& fileBytes,
                                             std::streamsize& offset,
                                             const LLAS_UCHAR& format) {
    PointDataRecord pointDataRecord;

    if (0 <= (int)format && (int)format < 5) {
      pointDataRecord = _readPointDataRecordFotmat0to4(fileBytes, offset, format);
    } else if (5 <= (int)format && (int)format < 16) {
      pointDataRecord = _readPointDataRecordFotmat5to15(fileBytes, offset, format);
    } else {
      _LLAS_logError("Unsupported point data record format: " + std::to_string((int)format));
    }

    return pointDataRecord;
  }
};

struct ExtendedVariableLengthRecord {
  // clang-format off
  inline static const std::streamsize NUM_BYTES_RESERVED                                            = 2;
  inline static const std::streamsize NUM_BYTES_USER_ID                                             = 16;
  inline static const std::streamsize NUM_BYTES_RECORD_ID                                           = 2;
  inline static const std::streamsize NUM_BYTES_RECORD_LENGTH_AFTER_HEADER                          = 16;
  inline static const std::streamsize NUM_BYTES_DESCPIPTION                                         = 32;
  // clang-format on

  ExtendedVariableLengthRecord()
      : reserved(),
        userID(),
        recordID(),
        recordLengthAfterHeader(),
        description(),
        record() {}

  // clang-format off
  LLAS_UCHAR        reserved;
  LLAS_CHAR         userID[NUM_BYTES_USER_ID + 1];          // plus null-termination
  LLAS_USHORT       recordID;
  LLAS_ULLONG       recordLengthAfterHeader;
  LLAS_CHAR         description[NUM_BYTES_DESCPIPTION + 1]; // plus null-termination
  std::vector<char> record;
  // clang-format on

  static ExtendedVariableLengthRecord readExtendedVariableLengthRecord(const std::vector<char>& fileBytes,
                                                                       std::streamsize& offset) {
    ExtendedVariableLengthRecord evlr;

    {
      // Reserved
      const std::streamsize nBytes = ExtendedVariableLengthRecord::NUM_BYTES_RESERVED;
      std::memcpy(&evlr.reserved, fileBytes.data() + offset, nBytes);
      offset += nBytes;
    }

    {
      // User ID
      const std::streamsize nBytes = ExtendedVariableLengthRecord::NUM_BYTES_USER_ID;
      std::memcpy(&evlr.userID, fileBytes.data() + offset, nBytes);
      offset += nBytes;
    }

    {
      // Record ID
      const std::streamsize nBytes = ExtendedVariableLengthRecord::NUM_BYTES_RECORD_ID;
      std::memcpy(&evlr.recordID, fileBytes.data() + offset, nBytes);
      offset += nBytes;
    }

    {
      // Record Length After Header
      const std::streamsize nBytes = ExtendedVariableLengthRecord::NUM_BYTES_RECORD_LENGTH_AFTER_HEADER;
      std::memcpy(&evlr.recordLengthAfterHeader, fileBytes.data() + offset, nBytes);
      offset += nBytes;
    }

    {
      // Description
      const std::streamsize nBytes = ExtendedVariableLengthRecord::NUM_BYTES_DESCPIPTION;
      std::memcpy(&evlr.description, fileBytes.data() + offset, nBytes);
      offset += nBytes;
    }

    {
      // Record
      const std::streamsize nBytes = (std::streamsize)evlr.recordLengthAfterHeader;
      evlr.record.assign(fileBytes.begin() + offset, fileBytes.begin() + offset + nBytes);
      offset += nBytes;
    }

    return evlr;
  }
};

struct LasData {
  LasData()
      : header(),
        variableLengthRecords(),
        pointDataRecords(),
        extendedVariableLengthRecord() {}

  PublicHeader header;
  std::vector<VariableLengthRecord> variableLengthRecords;
  std::vector<PointDataRecord> pointDataRecords;
  std::vector<ExtendedVariableLengthRecord> extendedVariableLengthRecord;

  /// @brief Get the number of points
  /// @return `nPoints` (`size_t`)
  inline size_t getNumPoints() const {
    return pointDataRecords.size();
  };

  /// @brief Get point coords
  /// @param rescale Scale and add offsets based on values in public header
  /// @return `Point coordinates` (`vecd_t`): arranged like `[x0, y0, z0, x1, y1, z1, ...]`.
  inline math::vecd_t getPointCoords(const bool rescale = true) const {
    math::vecd_t coords;

    const size_t nPoints = getNumPoints();
    coords.resize(3 * nPoints);

    for (size_t i = 0; i < nPoints; ++i) {
      double x = (double)pointDataRecords[i].x;
      double y = (double)pointDataRecords[i].y;
      double z = (double)pointDataRecords[i].z;

      if (rescale) {
        x = x * header.xScaleFactor + header.xOffset;
        y = y * header.yScaleFactor + header.yOffset;
        z = z * header.zScaleFactor + header.zOffset;
      }

      const size_t offset = 3 * i;

      coords[offset + 0] = x;
      coords[offset + 1] = y;
      coords[offset + 2] = z;
    }

    return coords;
  };

  /// @brief Get point colors
  /// @return `Point colors` (`std::vector<unsigned char>`): arranged like `[r0, g0, b0, r1, g1, b1, ...]`.
  inline math::vec_t<unsigned char> getPointColors() const {
    math::vec_t<unsigned char> colors;

    const size_t nPoints = getNumPoints();
    colors.resize(3 * nPoints);

    const double scaleFactor = 255.0 / 65535.0;

    for (size_t i = 0; i < nPoints; ++i) {
      const double red_d = (double)pointDataRecords[i].red * scaleFactor;
      const double green_d = (double)pointDataRecords[i].green * scaleFactor;
      const double blue_d = (double)pointDataRecords[i].blue * scaleFactor;

      const unsigned char red = (unsigned char)red_d;
      const unsigned char green = (unsigned char)green_d;
      const unsigned char blue = (unsigned char)blue_d;

      const size_t offset = 3 * i;

      colors[offset + 0] = red;
      colors[offset + 1] = green;
      colors[offset + 2] = blue;
    }

    return colors;
  };

  inline bool validate() const {
    const size_t& nPoints = getNumPoints();
    const math::vecd_t& pointCoords = getPointCoords();

    math::vec3d_t minCoords, maxCoords;
    for (size_t iPoint = 0; iPoint < nPoints; ++iPoint) {
      const size_t offset = 3 * iPoint;

      if (iPoint == 0) {
        minCoords[0] = pointCoords[offset + 0];
        minCoords[1] = pointCoords[offset + 1];
        minCoords[2] = pointCoords[offset + 2];
        maxCoords[0] = pointCoords[offset + 0];
        maxCoords[1] = pointCoords[offset + 1];
        maxCoords[2] = pointCoords[offset + 2];
      } else {
        if (pointCoords[offset + 0] < minCoords[0]) {
          minCoords[0] = pointCoords[offset + 0];
        }
        if (pointCoords[offset + 1] < minCoords[1]) {
          minCoords[1] = pointCoords[offset + 1];
        }
        if (pointCoords[offset + 2] < minCoords[2]) {
          minCoords[2] = pointCoords[offset + 2];
        }
        if (pointCoords[offset + 0] > maxCoords[0]) {
          maxCoords[0] = pointCoords[offset + 0];
        }
        if (pointCoords[offset + 1] > maxCoords[1]) {
          maxCoords[1] = pointCoords[offset + 1];
        }
        if (pointCoords[offset + 2] > maxCoords[2]) {
          maxCoords[2] = pointCoords[offset + 2];
        }
      }
    }

    char logMessage[LLAS_BUFFER_SIZE];

#if defined(_WIN64)
    std::fill_n(logMessage, LLAS_BUFFER_SIZE, '\0');
    sprintf_s(logMessage, "minCoords        = (%.5lf, %.5lf, %.5lf)", minCoords[0], minCoords[1], minCoords[2]);
    _LLAS_logDebug(logMessage);

    std::fill_n(logMessage, LLAS_BUFFER_SIZE, '\0');
    sprintf_s(logMessage, "publicHeader.min = (%.5lf, %.5lf, %.5lf)", header.minX, header.minY, header.minZ);
    _LLAS_logDebug(logMessage);

    std::fill_n(logMessage, LLAS_BUFFER_SIZE, '\0');
    sprintf_s(logMessage, "maxCoords        = (%.5lf, %.5lf, %.5lf)", maxCoords[0], maxCoords[1], maxCoords[2]);
    _LLAS_logDebug(logMessage);

    std::fill_n(logMessage, LLAS_BUFFER_SIZE, '\0');
    sprintf_s(logMessage, "publicHeader.max = (%.5lf, %.5lf, %.5lf)", header.maxX, header.maxY, header.maxZ);
    _LLAS_logDebug(logMessage);
#else
    std::fill_n(logMessage, LLAS_BUFFER_SIZE, '\0');
    sprintf(logMessage, "minCoords        = (%.5lf, %.5lf, %.5lf)", minCoords[0], minCoords[1], minCoords[2]);
    _LLAS_logDebug(logMessage);

    std::fill_n(logMessage, LLAS_BUFFER_SIZE, '\0');
    sprintf(logMessage, "publicHeader.min = (%.5lf, %.5lf, %.5lf)", header.minX, header.minY, header.minZ);
    _LLAS_logDebug(logMessage);

    std::fill_n(logMessage, LLAS_BUFFER_SIZE, '\0');
    sprintf(logMessage, "maxCoords        = (%.5lf, %.5lf, %.5lf)", maxCoords[0], maxCoords[1], maxCoords[2]);
    _LLAS_logDebug(logMessage);

    std::fill_n(logMessage, LLAS_BUFFER_SIZE, '\0');
    sprintf(logMessage, "publicHeader.max = (%.5lf, %.5lf, %.5lf)", header.maxX, header.maxY, header.maxZ);
    _LLAS_logDebug(logMessage);
#endif

    return true;
  }
};

using LasData_ptr = std::shared_ptr<LasData>;

// ==========================================================================
// Functions
// ==========================================================================

LLAS_FUNC_DECL_PREFIX LasData_ptr read(const std::string& filePath,
                                       const bool pointDataOnly = true);

/// @brief Read '.las' format file
/// @param filePath Path to the las file
/// @param pointDataOnly Read only "PointDataRecords"
/// @return `Las data` (`LasData_ptr`): Las content
LLAS_FUNC_DECL_PREFIX LasData_ptr read(const std::string& filePath,
                                       const bool pointDataOnly) {
  const auto startTime = std::chrono::system_clock::now();

#if defined(LLAS_PRINT_BYTES)
  _LLAS_logDebug("LLAS_CHAR   = " + std::to_string(sizeof(LLAS_CHAR)));
  _LLAS_logDebug("LLAS_SCHAR  = " + std::to_string(sizeof(LLAS_SCHAR)));
  _LLAS_logDebug("LLAS_UCHAR  = " + std::to_string(sizeof(LLAS_UCHAR)));
  _LLAS_logDebug("LLAS_SHORT  = " + std::to_string(sizeof(LLAS_SHORT)));
  _LLAS_logDebug("LLAS_USHORT = " + std::to_string(sizeof(LLAS_USHORT)));
  _LLAS_logDebug("LLAS_LONG   = " + std::to_string(sizeof(LLAS_LONG)));
  _LLAS_logDebug("LLAS_ULONG  = " + std::to_string(sizeof(LLAS_ULONG)));
  _LLAS_logDebug("LLAS_LLONG  = " + std::to_string(sizeof(LLAS_LLONG)));
  _LLAS_logDebug("LLAS_ULLONG = " + std::to_string(sizeof(LLAS_ULLONG)));
  _LLAS_logDebug("LLAS_FLOAT  = " + std::to_string(sizeof(LLAS_FLOAT)));
  _LLAS_logDebug("LLAS_DOUBLE = " + std::to_string(sizeof(LLAS_DOUBLE)));
  _LLAS_logDebug("LLAS_STRING = " + std::to_string(sizeof(LLAS_STRING)));
#endif

#if defined(LLAS_CHECK_BYTE_SIZE)
  if (sizeof(LLAS_CHAR) != 1) {
    _LLAS_logError("Byte size check failed: LLAS_CHAR");
    return nullptr;
  }
  if (sizeof(LLAS_SCHAR) != 1) {
    _LLAS_logError("Byte size check failed: LLAS_SCHAR");
    return nullptr;
  }
  if (sizeof(LLAS_UCHAR) != 1) {
    _LLAS_logError("Byte size check failed: LLAS_UCHAR");
    return nullptr;
  }
  if (sizeof(LLAS_SHORT) != 2) {
    _LLAS_logError("Byte size check failed: LLAS_SHORT");
    return nullptr;
  }
  if (sizeof(LLAS_USHORT) != 2) {
    _LLAS_logError("Byte size check failed: LLAS_USHORT");
    return nullptr;
  }
  if (sizeof(LLAS_LONG) != 4) {
    _LLAS_logError("Byte size check failed: LLAS_LONG");
    return nullptr;
  }
  if (sizeof(LLAS_ULONG) != 4) {
    _LLAS_logError("Byte size check failed: LLAS_ULONG");
    return nullptr;
  }
  if (sizeof(LLAS_LLONG) != 8) {
    _LLAS_logError("Byte size check failed: LLAS_LLONG");
    return nullptr;
  }
  if (sizeof(LLAS_ULLONG) != 8) {
    _LLAS_logError("Byte size check failed: LLAS_ULLONG");
    return nullptr;
  }
  if (sizeof(LLAS_FLOAT) != 4) {
    _LLAS_logError("Byte size check failed: LLAS_FLOAT");
    return nullptr;
  }
  if (sizeof(LLAS_DOUBLE) != 8) {
    _LLAS_logError("Byte size check failed: LLAS_DOUBLE");
    return nullptr;
  }
#endif

  bool isOK = true;

  // ======================================================================================================================
  // Open file
  // ======================================================================================================================
  std::vector<char> fileBytes;
  {
    std::ifstream file = std::ifstream(filePath, std::ios::binary);
    if (!file) {
      _LLAS_logError("Failed to open file: " + filePath);
      return nullptr;  // return nullptr
    }

    file.seekg(0, std::ios::end);
    std::streampos fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    fileBytes.resize(fileSize);
    file.read(fileBytes.data(), fileSize);

    file.close();
  }

  // ======================================================================================================================
  // Read 'Public Header'
  // ======================================================================================================================
  const PublicHeader publicHeader = PublicHeader::readPublicHeader(fileBytes);

  const LLAS_UCHAR format = publicHeader.pointDataRecordFormat;
  _LLAS_logInfo("format: " + std::to_string(format));

  if (format < 0 || 10 < format) {
    // NOTE: Format is defined from 0 to 10
    _LLAS_logError("Invalid point data record format:  " + std::to_string(format));
    return nullptr;  // return nullptr
  }

  const bool isLegacyFormat = format <= 5;

  // ======================================================================================================================
  // Read 'Variable Length Records'
  // ======================================================================================================================

  std::vector<VariableLengthRecord> variableLengthRecords;
  {
    if (!pointDataOnly) {
      const LLAS_ULONG nVariableLengthRecords = publicHeader.numOfVariableLengthRecords;
      _LLAS_logInfo("nVariableLengthRecords: " + std::to_string(nVariableLengthRecords));

      variableLengthRecords.resize(nVariableLengthRecords);  // allocate

      // NOTE: Move to the starting point of VLR
      std::streamsize offset = publicHeader.headerSize;

      for (LLAS_ULONG iRecord = 0; iRecord < nVariableLengthRecords; ++iRecord) {
        if ((LLAS_ULONG)offset >= publicHeader.offsetToPointData) {
          isOK = false;
          _LLAS_logError("The total size of VLRs exceeds the start of Point Data records!");
          break;
        }

        const auto variableLengthRecord = VariableLengthRecord::readVariableLengthRecord(fileBytes, offset);
        variableLengthRecords[iRecord] = variableLengthRecord;
      }
    }
  }

  // ======================================================================================================================
  // Read 'Point Data Records'
  // ======================================================================================================================

  std::vector<PointDataRecord> pointDataRecords;
  {
    const LLAS_ULLONG nPointRecords = isLegacyFormat ? publicHeader.legacyNumOfPointRecords : publicHeader.numOfPointRecords;
    _LLAS_logInfo("nPointRecords: " + std::to_string(nPointRecords));

    pointDataRecords.resize(nPointRecords);  // allocate

    for (LLAS_ULLONG iRecord = 0; iRecord < nPointRecords; ++iRecord) {
      // NOTE: Move to the starting point of each Point Data Record
      std::streamsize offset = publicHeader.offsetToPointData + iRecord * publicHeader.pointDataRecordLength;

      // Read
      const auto pointDataRecord = PointDataRecord::readPointDataRecord(fileBytes, offset, format);
      pointDataRecords[iRecord] = pointDataRecord;
    }
  }

  // ======================================================================================================================
  // Read 'Extended Variable Length Records'
  // ======================================================================================================================

  std::vector<ExtendedVariableLengthRecord> extendedVariableLengthRecords;
  {
    // version >= 1.4
    if (!pointDataOnly && publicHeader.hasStartOfFirstExtendedVariableLengthRecord && publicHeader.hasNumOfExtendedVariableLengthRecords) {
      const LLAS_ULONG nExtendedVariableLengthRecords = publicHeader.numOfExtendedVariableLengthRecords;
      _LLAS_logInfo("nExtendedVariableLengthRecords: " + std::to_string(nExtendedVariableLengthRecords));

      // Allocate
      extendedVariableLengthRecords.resize(nExtendedVariableLengthRecords);

      // NOTE: Move to the starting point of EVLR
      std::streamsize offset = publicHeader.startOfFirstExtendedVariableLengthRecord;

      // Read
      for (LLAS_ULONG iRecord = 0; iRecord < nExtendedVariableLengthRecords; ++iRecord) {
        const auto extendedVariableLengthRecord = ExtendedVariableLengthRecord::readExtendedVariableLengthRecord(fileBytes, offset);
        extendedVariableLengthRecords[iRecord] = extendedVariableLengthRecord;
      }
    }
  }

  // ======================================================================================================================
  // Create output object
  // ======================================================================================================================
  LasData_ptr lasData = nullptr;

  if (isOK) {
    lasData = std::make_shared<LasData>();

    lasData->header = publicHeader;
    lasData->variableLengthRecords = variableLengthRecords;
    lasData->pointDataRecords = pointDataRecords;
    lasData->extendedVariableLengthRecord = extendedVariableLengthRecords;
  }

  const auto endTime = std::chrono::system_clock::now();
  const double elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
  _LLAS_logInfo("Elapsed time: " + std::to_string(elapsedTime * 1e-6) + " [sec]");

  return lasData;
};
};  // namespace llas

#endif  // __LLAS_HPP__
