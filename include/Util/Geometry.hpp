#pragma once

#include <Util/DataStructure.hpp>
#include <Util/Logging.hpp>
#include <Util/Math.hpp>
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

  static vecf_pt calcBaryCentricCoord(const veci_pt &triangles,
                                      const vecf_pt &vertexCoords);
  static std::pair<vec3f_t, vec3f_t> calcModelBounds(const vecf_pt &vertexCoords);
  static veci_pt extractSurfaceTriangle(const int &nDivsBucket,
                                        const veci_pt &originalTriangles,
                                        const vecf_pt &vertexCoords);
  static void calcVertexNormals(const int &nNodes,
                                const veci_pt &triangles);
  static void removeDuplecatedVertices(
      const vecf_pt &srcVertices,
      const veci_pt &srcIndices,
      vecf_pt &distVertices,
      veci_pt &distIndices,
      const float &threshold);
};

}  // namespace util
}  // namespace simview