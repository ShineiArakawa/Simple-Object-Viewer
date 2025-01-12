#pragma once

#include <SimView/Util/DataStructure.hpp>
#include <SimView/Util/Logging.hpp>
#include <SimView/Util/Math.hpp>
#include <array>
#include <memory>
#include <vector>

namespace simview {
namespace util {

class TriangleBucket {
 public:
  using Bucket_t = vec_pt<vec_pt<vec_pt<vec_pt<int>>>>;

 private:
  inline static const int INVALID_ID = -1;
  inline static const float COEFF_FOR_INTERVAL_OF_BUCKETS = 1.05f;

  float _interval;
  vec3f_t _minCoords;
  vec3f_t _maxCoords;
  int _nBucketsX;
  int _nBucketsY;
  int _nBucketsZ;

  Bucket_t _bucket;

 public:
  TriangleBucket();
  ~TriangleBucket();

  void initBucket(const float &, const vec3f_t &, const vec3f_t &);
  void resetBuckets();

  int toBucketIndexX(const float &) const;
  int toBucketIndexY(const float &) const;
  int toBucketIndexZ(const float &) const;

  vec_pt<int> getIdsInBucket(const int &, const int &, const int &) const;
  vec_pt<int> getIdsInBucket(const float &, const float &, const float &) const;
  void addToBucket(const float &, const float &, const float &, const int &);
};

using TriangleBucket_t = std::shared_ptr<TriangleBucket>;

class Geometry {
 public:
  template <class Indexing>
  inline static void sort3Elems(Indexing &a, Indexing &b, Indexing &c) {
    if (a > b) {
      std::swap(a, b);
    }
    if (b > c) {
      std::swap(b, c);
    }
    if (a > b) {
      std::swap(a, b);
    }
  }

  template <class Indexing_t = uint32_t, class Coord_t = float>
  static vec_pt<Coord_t> calcBaryCentricCoord(const vec_pt<Indexing_t> &triangles,
                                              const vec_pt<Coord_t> &vertexCoords);

  template <class Coord_t = float>
  static std::pair<vec3_t<Coord_t>, vec3_t<Coord_t>> calcModelBounds(const vec_pt<Coord_t> &vertexCoords);

  template <class Indexing_t = uint32_t, class Coord_t = float>
  static vec_pt<Indexing_t> extractSurfaceTriangle(const int &nDivsBucket,
                                                   const vec_pt<Indexing_t> &originalTriangles,
                                                   const vec_pt<Coord_t> &vertexCoords);

  template <class Coord_t = float>
  static void calcVertexNormals(const int &nNodes,
                                const vec_pt<Coord_t> &triangles);

  static void removeDuplecatedVertices(
      const vecf_pt &srcVertices,
      const veci_pt &srcIndices,
      vecf_pt &distVertices,
      veci_pt &distIndices,
      const float &threshold);
};

// Type specialization
template <>
vec_pt<uint32_t> Geometry::extractSurfaceTriangle<uint32_t, float>(const int &,
                                                                   const vec_pt<uint32_t> &,
                                                                   const vec_pt<float> &);

}  // namespace util
}  // namespace simview