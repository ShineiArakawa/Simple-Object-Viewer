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
// Simple logging function
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

inline static float innerProduct(const vec3f_t& vec0,
                                 const vec3f_t& vec1) {
  return vec0[0] * vec1[0] + vec0[1] * vec1[1] + vec0[2] * vec1[2];
}

inline static vec3f_t outerProduct(const vec3f_t& vec0,
                                   const vec3f_t& vec1) {
  return {
      vec0[1] * vec1[2] - vec0[2] * vec1[1],  // vec0.y * vec1.z - vec0.z * vec1.y
      vec0[2] * vec1[0] - vec0[0] * vec1[2],  // vec0.z * vec1.x - vec0.x * vec1.z
      vec0[0] * vec1[1] - vec0[1] * vec1[0]   // vec0.x * vec1.y - vec0.y * vec1.x
  };
}

inline static void outerProduct(const float& x0, const float& y0, const float& z0,
                                const float& x1, const float& y1, const float& z1,
                                float& x, float& y, float& z) {
  x = y0 * z1 - z0 * y1;
  y = z0 * x1 - x0 * z1;
  z = x0 * y1 - y0 * x1;
}

inline static float length(vec3f_t& vec) {
  return std::sqrt(vec[0] * vec[0] +
                   vec[1] * vec[1] +
                   vec[2] * vec[2]);
}

inline static void normalize(vec3f_t& vec) {
  const float len = length(vec);
  vec[0] = vec[0] / len;
  vec[1] = vec[1] / len;
  vec[2] = vec[2] / len;
}

inline static void normalize(float& x, float& y, float& z) {
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
        numOfPointsByReturn() {
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
  // clang-format on
};

struct VariableLengthRecord {
  VariableLengthRecord() {}
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
};

struct ExtendedVariableLengthRecord {
  ExtendedVariableLengthRecord() {}
};

struct LasData {
  LasData()
      : publicHeader(),
        variableLengthRecords(),
        pointDataRecords(),
        extendedVariableLengthRecord() {}

  PublicHeader publicHeader;
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
        x = x * publicHeader.xScaleFactor + publicHeader.xOffset;
        y = y * publicHeader.yScaleFactor + publicHeader.yOffset;
        z = z * publicHeader.zScaleFactor + publicHeader.zOffset;
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
    sprintf_s(logMessage, "publicHeader.min = (%.5lf, %.5lf, %.5lf)", publicHeader.minX, publicHeader.minY, publicHeader.minZ);
    _LLAS_logDebug(logMessage);

    std::fill_n(logMessage, LLAS_BUFFER_SIZE, '\0');
    sprintf_s(logMessage, "maxCoords        = (%.5lf, %.5lf, %.5lf)", maxCoords[0], maxCoords[1], maxCoords[2]);
    _LLAS_logDebug(logMessage);

    std::fill_n(logMessage, LLAS_BUFFER_SIZE, '\0');
    sprintf_s(logMessage, "publicHeader.max = (%.5lf, %.5lf, %.5lf)", publicHeader.maxX, publicHeader.maxY, publicHeader.maxZ);
    _LLAS_logDebug(logMessage);
#else
    std::fill_n(logMessage, LLAS_BUFFER_SIZE, '\0');
    sprintf(logMessage, "minCoords        = (%.5lf, %.5lf, %.5lf)", minCoords[0], minCoords[1], minCoords[2]);
    _logDebug(logMessage);

    std::fill_n(logMessage, LLAS_BUFFER_SIZE, '\0');
    sprintf(logMessage, "publicHeader.min = (%.5lf, %.5lf, %.5lf)", publicHeader.minX, publicHeader.minY, publicHeader.minZ);
    _logDebug(logMessage);

    std::fill_n(logMessage, LLAS_BUFFER_SIZE, '\0');
    sprintf(logMessage, "maxCoords        = (%.5lf, %.5lf, %.5lf)", maxCoords[0], maxCoords[1], maxCoords[2]);
    _logDebug(logMessage);

    std::fill_n(logMessage, LLAS_BUFFER_SIZE, '\0');
    sprintf(logMessage, "publicHeader.max = (%.5lf, %.5lf, %.5lf)", publicHeader.maxX, publicHeader.maxY, publicHeader.maxZ);
    _logDebug(logMessage);
#endif

    return true;
  }
};

using LasData_t = std::shared_ptr<LasData>;

// ==========================================================================
// Utility Functions
// ==========================================================================
inline static void _readBytes(std::ifstream& ifs,
                              std::vector<char>& bytes,
                              const std::streamsize& nBytes) {
  ifs.read(bytes.data(), nBytes);
}

// ==========================================================================
// Functions
// ==========================================================================

PublicHeader _readPublicHeader(std::ifstream& file) {
  PublicHeader publicHeader;
  std::vector<char> buffer(LLAS_BUFFER_SIZE);

  {
    // File signature
    const std::streamsize nBytes = PublicHeader::NUM_BYTES_FILE_SIGNATURE;
    _readBytes(file, buffer, nBytes);
    std::copy(buffer.begin(), buffer.begin() + nBytes, publicHeader.fileSignature);
  }

  {
    // File source ID
    const std::streamsize nBytes = PublicHeader::NUM_BYTES_FILE_SOURCE_ID;
    _readBytes(file, buffer, nBytes);
    std::memcpy(&publicHeader.fileSourceID, buffer.data(), sizeof(LLAS_USHORT));
  }

  {
    // Global encoding
    const std::streamsize nBytes = PublicHeader::NUM_BYTES_GLOBAL_ENCODING;
    _readBytes(file, buffer, nBytes);
    std::memcpy(&publicHeader.globalEncoding, buffer.data(), sizeof(LLAS_USHORT));
  }

  {
    // Project ID-GUID Data 1
    const std::streamsize nBytes = PublicHeader::NUM_BYTES_PROJECT_ID_1;
    _readBytes(file, buffer, nBytes);
    std::memcpy(&publicHeader.projectID1, buffer.data(), sizeof(LLAS_ULONG));
  }

  {
    // Project ID-GUID Data 2
    const std::streamsize nBytes = PublicHeader::NUM_BYTES_PROJECT_ID_2;
    _readBytes(file, buffer, nBytes);
    std::memcpy(&publicHeader.projectID2, buffer.data(), sizeof(LLAS_USHORT));
  }

  {
    // Project ID-GUID Data 3
    const std::streamsize nBytes = PublicHeader::NUM_BYTES_PROJECT_ID_3;
    _readBytes(file, buffer, nBytes);
    std::memcpy(&publicHeader.projectID3, buffer.data(), sizeof(LLAS_USHORT));
  }

  {
    // Project ID-GUID Data 4
    const std::streamsize nBytes = PublicHeader::NUM_BYTES_PROJECT_ID_4;
    _readBytes(file, buffer, nBytes);
    std::copy(buffer.begin(), buffer.begin() + nBytes, publicHeader.projectID4);
  }

  {
    // Version major
    const std::streamsize nBytes = PublicHeader::NUM_BYTES_VERSION_MAJOR;
    _readBytes(file, buffer, nBytes);
    std::memcpy(&publicHeader.versionMajor, buffer.data(), sizeof(LLAS_UCHAR));
  }

  {
    // Version minor
    const std::streamsize nBytes = PublicHeader::NUM_BYTES_VERSION_MINOR;
    _readBytes(file, buffer, nBytes);
    std::memcpy(&publicHeader.versionMinor, buffer.data(), sizeof(LLAS_UCHAR));
  }

  {
    // System Identifier
    const std::streamsize nBytes = PublicHeader::NUM_BYTES_SYSTEM_IDENTIFIER;
    _readBytes(file, buffer, nBytes);
    std::copy(buffer.begin(), buffer.begin() + nBytes, publicHeader.systemIdentifier);
  }

  {
    // Generating Software
    const std::streamsize nBytes = PublicHeader::NUM_BYTES_GENERATING_SOFTWARE;
    _readBytes(file, buffer, nBytes);
    std::copy(buffer.begin(), buffer.begin() + nBytes, publicHeader.generatingSoftware);
  }

  {
    // File Creation Day of Year
    const std::streamsize nBytes = PublicHeader::NUM_BYTES_FILE_CREATION_DAY_OF_YEAR;
    _readBytes(file, buffer, nBytes);
    std::memcpy(&publicHeader.fileCreationDayOfYear, buffer.data(), sizeof(LLAS_USHORT));
  }

  {
    // File Creation Year
    const std::streamsize nBytes = PublicHeader::NUM_BYTES_FILE_CREATION_YEAR;
    _readBytes(file, buffer, nBytes);
    std::memcpy(&publicHeader.fileCreationYear, buffer.data(), sizeof(LLAS_USHORT));
  }

  {
    // Header size
    const std::streamsize nBytes = PublicHeader::NUM_BYTES_HEADER_SIZE;
    _readBytes(file, buffer, nBytes);
    std::memcpy(&publicHeader.headerSize, buffer.data(), sizeof(LLAS_USHORT));
  }

  {
    // Offset to point data
    const std::streamsize nBytes = PublicHeader::NUM_BYTES_OFFSET_TO_POINT_DATA;
    _readBytes(file, buffer, nBytes);
    std::memcpy(&publicHeader.offsetToPointData, buffer.data(), sizeof(LLAS_ULONG));
  }

  {
    // Number of Variable Length Records
    const std::streamsize nBytes = PublicHeader::NUM_BYTES_NUM_OF_VARIABLE_LENGTH_RECORDS;
    _readBytes(file, buffer, nBytes);
    std::memcpy(&publicHeader.numOfVariableLengthRecords, buffer.data(), sizeof(LLAS_ULONG));
  }

  {
    // Point Data Record Format
    const std::streamsize nBytes = PublicHeader::NUM_BYTES_POINT_DATA_RECORD_FORMAT;
    _readBytes(file, buffer, nBytes);
    std::memcpy(&publicHeader.pointDataRecordFormat, buffer.data(), sizeof(LLAS_UCHAR));
  }

  {
    // Point Data Record Length
    const std::streamsize nBytes = PublicHeader::NUM_BYTES_POINT_DATA_RECORD_LENGTH;
    _readBytes(file, buffer, nBytes);
    std::memcpy(&publicHeader.pointDataRecordLength, buffer.data(), sizeof(LLAS_USHORT));
  }

  {
    // Legacy Number of Point Records
    const std::streamsize nBytes = PublicHeader::NUM_BYTES_LEGACY_NUM_OF_POINT_RECORDS;
    _readBytes(file, buffer, nBytes);
    std::memcpy(&publicHeader.legacyNumOfPointRecords, buffer.data(), sizeof(LLAS_ULONG));
  }

  {
    // Legacy Number of Point by Return
    const std::streamsize nBytes = PublicHeader::NUM_BYTES_LEGACY_NUM_OF_POINT_BY_RETURN;
    _readBytes(file, buffer, nBytes);
    for (std::streamsize i = 0; i < nBytes / sizeof(LLAS_ULONG); ++i) {
      std::memcpy(&publicHeader.legacyNumOfPointByReturn[i],
                  buffer.data() + i * sizeof(LLAS_ULONG),
                  sizeof(LLAS_ULONG));
    }
  }

  {
    // X Scale Factor
    const std::streamsize nBytes = PublicHeader::NUM_BYTES_X_SCALE_FACTOR;
    _readBytes(file, buffer, nBytes);
    std::memcpy(&publicHeader.xScaleFactor, buffer.data(), sizeof(LLAS_DOUBLE));
  }

  {
    // Y Scale Factor
    const std::streamsize nBytes = PublicHeader::NUM_BYTES_Y_SCALE_FACTOR;
    _readBytes(file, buffer, nBytes);
    std::memcpy(&publicHeader.yScaleFactor, buffer.data(), sizeof(LLAS_DOUBLE));
  }

  {
    // Z Scale Factor
    const std::streamsize nBytes = PublicHeader::NUM_BYTES_Z_SCALE_FACTOR;
    _readBytes(file, buffer, nBytes);
    std::memcpy(&publicHeader.zScaleFactor, buffer.data(), sizeof(LLAS_DOUBLE));
  }

  {
    // X Offset
    const std::streamsize nBytes = PublicHeader::NUM_BYTES_X_OFFSET;
    _readBytes(file, buffer, nBytes);
    std::memcpy(&publicHeader.xOffset, buffer.data(), sizeof(LLAS_DOUBLE));
  }

  {
    // Y Offset
    const std::streamsize nBytes = PublicHeader::NUM_BYTES_Y_OFFSET;
    _readBytes(file, buffer, nBytes);
    std::memcpy(&publicHeader.yOffset, buffer.data(), sizeof(LLAS_DOUBLE));
  }

  {
    // Z Offset
    const std::streamsize nBytes = PublicHeader::NUM_BYTES_Z_OFFSET;
    _readBytes(file, buffer, nBytes);
    std::memcpy(&publicHeader.zOffset, buffer.data(), sizeof(LLAS_DOUBLE));
  }

  {
    // Max X
    const std::streamsize nBytes = PublicHeader::NUM_BYTES_MAX_X;
    _readBytes(file, buffer, nBytes);
    std::memcpy(&publicHeader.maxX, buffer.data(), sizeof(LLAS_DOUBLE));
  }

  {
    // Min X
    const std::streamsize nBytes = PublicHeader::NUM_BYTES_MIN_X;
    _readBytes(file, buffer, nBytes);
    std::memcpy(&publicHeader.minX, buffer.data(), sizeof(LLAS_DOUBLE));
  }

  {
    // Max Y
    const std::streamsize nBytes = PublicHeader::NUM_BYTES_MAX_Y;
    _readBytes(file, buffer, nBytes);
    std::memcpy(&publicHeader.maxY, buffer.data(), sizeof(LLAS_DOUBLE));
  }

  {
    // Min Y
    const std::streamsize nBytes = PublicHeader::NUM_BYTES_MIN_Y;
    _readBytes(file, buffer, nBytes);
    std::memcpy(&publicHeader.minY, buffer.data(), sizeof(LLAS_DOUBLE));
  }

  {
    // Max Z
    const std::streamsize nBytes = PublicHeader::NUM_BYTES_MAX_Z;
    _readBytes(file, buffer, nBytes);
    std::memcpy(&publicHeader.maxZ, buffer.data(), sizeof(LLAS_DOUBLE));
  }

  {
    // Min Z
    const std::streamsize nBytes = PublicHeader::NUM_BYTES_MIN_Z;
    _readBytes(file, buffer, nBytes);
    std::memcpy(&publicHeader.minZ, buffer.data(), sizeof(LLAS_DOUBLE));
  }

  {
    // Start of Waveform Data Packet Record
    const std::streamsize nBytes = PublicHeader::NUM_BYTES_START_OF_WAVEFORM_DATA_PACKET_RECORD;
    _readBytes(file, buffer, nBytes);
    std::memcpy(&publicHeader.startOfWaveformDataPacketRecord, buffer.data(), sizeof(LLAS_ULLONG));
  }

  {
    // Start of First Extended Variable Length Record
    const std::streamsize nBytes = PublicHeader::NUM_BYTES_START_OF_FIRST_EXTENDED_VARIABLE_LENGTH_RECORD;
    _readBytes(file, buffer, nBytes);
    std::memcpy(&publicHeader.startOfFirstExtendedVariableLengthRecord, buffer.data(), sizeof(LLAS_ULLONG));
  }

  {
    // Number of Extended Variable Length Records
    const std::streamsize nBytes = PublicHeader::NUM_BYTES_NUM_OF_EXTENDED_VARIABLE_LENGTH_RECORDS;
    _readBytes(file, buffer, nBytes);
    std::memcpy(&publicHeader.numOfExtendedVariableLengthRecords, buffer.data(), sizeof(LLAS_ULONG));
  }

  {
    // Number of Point Records
    const std::streamsize nBytes = PublicHeader::NUM_BYTES_NUM_OF_POINT_RECORDS;
    _readBytes(file, buffer, nBytes);
    std::memcpy(&publicHeader.numOfPointRecords, buffer.data(), sizeof(LLAS_ULLONG));
  }

  {
    // Number of Points by Return
    const std::streamsize nBytes = PublicHeader::NUM_BYTES_NUM_OF_POINTS_BY_RETURN;
    _readBytes(file, buffer, nBytes);
    for (std::streamsize i = 0; i < nBytes / sizeof(LLAS_ULLONG); ++i) {
      std::memcpy(&publicHeader.numOfPointsByReturn[i],
                  buffer.data() + i * sizeof(LLAS_ULLONG),
                  sizeof(LLAS_ULLONG));
    }
  }

  return publicHeader;
};

VariableLengthRecord _readVariableLengthRecord(std::ifstream& file) {
  VariableLengthRecord variableLengthRecord;
  return variableLengthRecord;
}

PointDataRecord _readPointDataRecordFotmat0to4(std::ifstream& file,
                                               std::vector<char>& buffer,
                                               const LLAS_UCHAR format) {
  PointDataRecord pointDataRecord;

  {
    // X
    const std::streamsize nBytes = PointDataRecord::NUM_BYTES_X;
    _readBytes(file, buffer, nBytes);
    std::memcpy(&pointDataRecord.x, buffer.data(), sizeof(LLAS_LONG));
  }

  {
    // Y
    const std::streamsize nBytes = PointDataRecord::NUM_BYTES_Y;
    _readBytes(file, buffer, nBytes);
    std::memcpy(&pointDataRecord.y, buffer.data(), sizeof(LLAS_LONG));
  }

  {
    // Z
    const std::streamsize nBytes = PointDataRecord::NUM_BYTES_Z;
    _readBytes(file, buffer, nBytes);
    std::memcpy(&pointDataRecord.z, buffer.data(), sizeof(LLAS_LONG));
  }

  {
    // Intensity
    const std::streamsize nBytes = PointDataRecord::NUM_BYTES_INTENSITY;
    _readBytes(file, buffer, nBytes);
    std::memcpy(&pointDataRecord.intensity, buffer.data(), sizeof(LLAS_USHORT));
  }

  {
    // Sensor Data
    const std::streamsize nBytes = PointDataRecord::NUM_BYTES_SENSOR_DATA;
    _readBytes(file, buffer, nBytes);

    // TODO: set variables
  }

  {
    // Classification
    const std::streamsize nBytes = PointDataRecord::NUM_BYTES_CLASSIFICATION;
    _readBytes(file, buffer, nBytes);
    std::memcpy(&pointDataRecord.classification, buffer.data(), sizeof(LLAS_UCHAR));
  }

  {
    // Scan Angle Rank
    const std::streamsize nBytes = PointDataRecord::NUM_BYTES_SCAN_ANGLE_RANK;
    _readBytes(file, buffer, nBytes);
    std::memcpy(&pointDataRecord.scanAngleRank, buffer.data(), sizeof(LLAS_SCHAR));
  }

  {
    // User Data
    const std::streamsize nBytes = PointDataRecord::NUM_BYTES_USER_DATA;
    _readBytes(file, buffer, nBytes);
    std::memcpy(&pointDataRecord.userData, buffer.data(), sizeof(LLAS_UCHAR));
  }

  {
    // Point Soruce ID
    const std::streamsize nBytes = PointDataRecord::NUM_BYTES_POINT_SOURCE_ID;
    _readBytes(file, buffer, nBytes);
    std::memcpy(&pointDataRecord.pointSourceID, buffer.data(), sizeof(LLAS_USHORT));
  }

  if (format == 1 || format == 3 || format == 4) {
    // GPS Time
    const std::streamsize nBytes = PointDataRecord::NUM_BYTES_GPS_TIME;
    _readBytes(file, buffer, nBytes);
    std::memcpy(&pointDataRecord.GPSTime, buffer.data(), sizeof(LLAS_DOUBLE));
  }

  if (format == 2 || format == 3) {
    {
      // Red
      const std::streamsize nBytes = PointDataRecord::NUM_BYTES_RED;
      _readBytes(file, buffer, nBytes);
      std::memcpy(&pointDataRecord.red, buffer.data(), sizeof(LLAS_USHORT));
    }

    {
      // Green
      const std::streamsize nBytes = PointDataRecord::NUM_BYTES_GREEN;
      _readBytes(file, buffer, nBytes);
      std::memcpy(&pointDataRecord.green, buffer.data(), sizeof(LLAS_USHORT));
    }

    {
      // Blue
      const std::streamsize nBytes = PointDataRecord::NUM_BYTES_BLUE;
      _readBytes(file, buffer, nBytes);
      std::memcpy(&pointDataRecord.blue, buffer.data(), sizeof(LLAS_USHORT));
    }
  }

  return pointDataRecord;
}

PointDataRecord _readPointDataRecordFotmat5to15(std::ifstream& file,
                                                std::vector<char>& buffer,
                                                const LLAS_UCHAR format) {
  _LLAS_logError("Unsupported point data record format: " + std::to_string((int)format));
  PointDataRecord pointDataRecord;
  return pointDataRecord;
}

PointDataRecord _readPointDataRecord(std::ifstream& file,
                                     std::vector<char>& buffer,
                                     const LLAS_UCHAR format) {
  PointDataRecord pointDataRecord;

  if (0 <= (int)format && (int)format < 5) {
    pointDataRecord = _readPointDataRecordFotmat0to4(file, buffer, format);
  } else if (5 <= (int)format && (int)format < 16) {
    pointDataRecord = _readPointDataRecordFotmat5to15(file, buffer, format);
  } else {
    _LLAS_logError("Unsupported point data record format: " + std::to_string((int)format));
  }

  return pointDataRecord;
}

ExtendedVariableLengthRecord _readExtendedVariableLengthRecord(std::ifstream& file) {
  ExtendedVariableLengthRecord extendedVariableLengthRecord;
  return extendedVariableLengthRecord;
}

LasData_t read(const std::string& filePath,
               const bool pointDataOnly = true);

/// @brief Read '.las' format file
/// @param filePath Path to the las file
/// @param pointDataOnly Read only "PointDataRecords"
/// @return `Las data` (`LasData_t`): Las content
LasData_t read(const std::string& filePath,
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

  bool isOK = true;

  // ======================================================================================================================
  // Open file
  // ======================================================================================================================
  std::ifstream file = std::ifstream(filePath, std::ios::binary);
  if (!file) {
    _LLAS_logError("Failed to open file: " + filePath);
    return nullptr;  // return nullptr
  }

  // ======================================================================================================================
  // Read 'Public Header'
  // ======================================================================================================================
  const PublicHeader publicHeader = _readPublicHeader(file);

  const LLAS_UCHAR format = publicHeader.pointDataRecordFormat;
  _LLAS_logInfo("format: " + std::to_string(format));

  if (format < 0 || 10 < format) {
    // NOTE: Format is defined from 0 to 10
    _LLAS_logError("Invalid point data record format:  " + std::to_string(format));
    return nullptr;  // return nullptr
  }

  const bool isLegacyFormat = format < 6;

  // ======================================================================================================================
  // Read 'Variable Length Records'
  // ======================================================================================================================
  const LLAS_ULONG nVariableLengthRecords = publicHeader.numOfVariableLengthRecords;

  std::vector<VariableLengthRecord> variableLengthRecords;

  if (!pointDataOnly) {
    _LLAS_logInfo("nVariableLengthRecords: " + std::to_string(nVariableLengthRecords));

    variableLengthRecords.resize(nVariableLengthRecords);

    for (LLAS_ULONG iRecord = 0; iRecord < nVariableLengthRecords; ++iRecord) {
      VariableLengthRecord variableLengthRecord = _readVariableLengthRecord(file);
      variableLengthRecords[iRecord] = variableLengthRecord;
    }
  }

  // ======================================================================================================================
  // Read 'Point Data Records'
  // ======================================================================================================================
  const LLAS_ULLONG nPointRecords = isLegacyFormat ? publicHeader.legacyNumOfPointRecords : publicHeader.numOfPointRecords;
  _LLAS_logInfo("nPointRecords: " + std::to_string(nPointRecords));

  std::vector<PointDataRecord> pointDataRecords;
  pointDataRecords.resize(nPointRecords);

  std::vector<char> buffer(LLAS_BUFFER_SIZE);

  for (LLAS_ULLONG iRecord = 0; iRecord < nPointRecords; ++iRecord) {
    // Move to the starting point of Point Data Record
    const std::streamsize byteOffset = publicHeader.offsetToPointData + iRecord * publicHeader.pointDataRecordLength;

    file.seekg(byteOffset, std::ios::beg);

    const PointDataRecord pointDataRecord = _readPointDataRecord(file, buffer, format);
    pointDataRecords[iRecord] = pointDataRecord;
  }

  // ======================================================================================================================
  // Read 'Extended Variable Length Records'
  // ======================================================================================================================
  const LLAS_ULONG nExtendedVariableLengthRecords = publicHeader.numOfExtendedVariableLengthRecords;

  std::vector<ExtendedVariableLengthRecord> extendedVariableLengthRecords;

  if (!pointDataOnly) {
    _LLAS_logInfo("nExtendedVariableLengthRecords: " + std::to_string(nExtendedVariableLengthRecords));

    extendedVariableLengthRecords.resize(nExtendedVariableLengthRecords);

    for (LLAS_ULONG iRecord = 0; iRecord < nExtendedVariableLengthRecords; ++iRecord) {
      const ExtendedVariableLengthRecord extendedVariableLengthRecord = _readExtendedVariableLengthRecord(file);
      extendedVariableLengthRecords[iRecord] = extendedVariableLengthRecord;
    }
  }

  file.close();

  // ======================================================================================================================
  // Create output object
  // ======================================================================================================================
  LasData_t lasData = nullptr;

  if (isOK) {
    lasData = std::make_shared<LasData>();

    lasData->publicHeader = publicHeader;
    lasData->variableLengthRecords = variableLengthRecords;
    lasData->pointDataRecords = pointDataRecords;
    lasData->extendedVariableLengthRecord = extendedVariableLengthRecords;

    lasData->validate();
  }

  const auto endTime = std::chrono::system_clock::now();
  const double elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
  _LLAS_logInfo("Elapsed time: " + std::to_string(elapsedTime * 1e-6) + " [sec]");

  return lasData;
};
};  // namespace llas

#endif  // __LLAS_HPP__
