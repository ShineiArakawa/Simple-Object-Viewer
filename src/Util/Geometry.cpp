#include <SimView/Util/Geometry.hpp>

namespace simview {
namespace util {

TriangleBucket::TriangleBucket()
    : _interval(0.0f),
      _minCoords({0.0f, 0.0f, 0.0f}),
      _maxCoords({0.0f, 0.0f, 0.0f}),
      _nBucketsX(0),
      _nBucketsY(0),
      _nBucketsZ(0),
      _bucket(nullptr) {}

TriangleBucket::~TriangleBucket() {}

void TriangleBucket::initBucket(const float &interval,
                                const vec3f_t &minCoords,
                                const vec3f_t &maxCoords) {
  _interval = interval;

  _minCoords = minCoords;
  _maxCoords = maxCoords;

  _nBucketsX = (int)((_maxCoords[0] - _minCoords[0]) / _interval) + 1;
  _nBucketsY = (int)((_maxCoords[1] - _minCoords[1]) / _interval) + 1;
  _nBucketsZ = (int)((_maxCoords[2] - _minCoords[2]) / _interval) + 1;

  LOG_INFO("[Bucket Info]");
  LOG_INFO("  interval  = " + std::to_string(_interval));
  LOG_INFO("  minCoords = (" + std::to_string(_minCoords[0]) + ", " + std::to_string(_minCoords[1]) + ", " + std::to_string(_minCoords[2]) + ")");
  LOG_INFO("  maxCoords = (" + std::to_string(_maxCoords[0]) + ", " + std::to_string(_maxCoords[1]) + ", " + std::to_string(_maxCoords[2]) + ")");
  LOG_INFO("  nBuckets  = (" + std::to_string(_nBucketsX) + ", " + std::to_string(_nBucketsY) + ", " + std::to_string(_nBucketsZ) + ")");

  resetBuckets();
}

void TriangleBucket::resetBuckets() {
  _bucket = std::make_shared<std::vector<vec_pt<vec_pt<vec_pt<int>>>>>();

  for (int x = 0; x < _nBucketsX; ++x) {
    _bucket->push_back(std::make_shared<std::vector<vec_pt<vec_pt<int>>>>());

    for (int y = 0; y < _nBucketsY; ++y) {
      (*_bucket)[x]->push_back(std::make_shared<std::vector<vec_pt<int>>>());

      for (int z = 0; z < _nBucketsZ; ++z) {
        (*(*_bucket)[x])[y]->push_back(std::make_shared<std::vector<int>>());
      }
    }
  }
}

int TriangleBucket::toBucketIndexX(const float &x) const {
  int index = (int)((x - _minCoords[0]) / _interval);

  if (index < 0) {
    index = 0;
  }

  if (index >= _nBucketsX) {
    index = _nBucketsX - 1;
  }

  return index;
}

int TriangleBucket::toBucketIndexY(const float &y) const {
  int index = (int)((y - _minCoords[1]) / _interval);

  if (index < 0) {
    index = 0;
  }

  if (index >= _nBucketsY) {
    index = _nBucketsY - 1;
  }

  return index;
}

int TriangleBucket::toBucketIndexZ(const float &z) const {
  int index = (int)((z - _minCoords[2]) / _interval);

  if (index < 0) {
    index = 0;
  }

  if (index >= _nBucketsZ) {
    index = _nBucketsZ - 1;
  }

  return index;
}

vec_pt<int> TriangleBucket::getIdsInBucket(const int &x,
                                           const int &y,
                                           const int &z) const {
  vec_pt<int> paticleIds = nullptr;

  if (x >= 0 && x < _nBucketsX && y >= 0 && y < _nBucketsY && z >= 0 && z < _nBucketsZ) {
    paticleIds = (*(*(*_bucket)[x])[y])[z];
  }

  return paticleIds;
}

vec_pt<int> TriangleBucket::getIdsInBucket(const float &x,
                                           const float &y,
                                           const float &z) const {
  const int bucketIndexX = toBucketIndexX(x);
  const int bucketIndexY = toBucketIndexY(y);
  const int bucketIndexZ = toBucketIndexZ(z);
  return getIdsInBucket(bucketIndexX, bucketIndexY, bucketIndexZ);
}

void TriangleBucket::addToBucket(const float &x,
                                 const float &y,
                                 const float &z,
                                 const int &index) {
  const int bucketIndexX = toBucketIndexX(x);
  const int bucketIndexY = toBucketIndexY(y);
  const int bucketIndexZ = toBucketIndexZ(z);

  (*(*(*_bucket)[bucketIndexX])[bucketIndexY])[bucketIndexZ]->push_back(index);
}

template <class Indexing_t, class Coord_t>
vec_pt<Coord_t> Geometry::calcBaryCentricCoord(const vec_pt<Indexing_t> &triangles,
                                               const vec_pt<Coord_t> &vertexCoords) {
  const Indexing_t nTriangles = static_cast<Indexing_t>(triangles->size()) / 3U;

  vec_pt<Coord_t> baryCenterCoords = std::make_shared<std::vector<Coord_t>>();
  baryCenterCoords->resize(3U * nTriangles);

  for (Indexing_t iTriangle = 0U; iTriangle < nTriangles; ++iTriangle) {
    const Indexing_t offset = 3U * iTriangle;

    const Indexing_t vertexIndex0 = (*triangles)[offset + 0U];
    const Indexing_t vertexIndex1 = (*triangles)[offset + 1U];
    const Indexing_t vertexIndex2 = (*triangles)[offset + 2U];

    (*baryCenterCoords)[offset + 0U] = ((*vertexCoords)[vertexIndex0 + 0U] + (*vertexCoords)[vertexIndex1 + 0U] + (*vertexCoords)[vertexIndex2 + 0U]) / static_cast<Coord_t>(3.0);
    (*baryCenterCoords)[offset + 1U] = ((*vertexCoords)[vertexIndex0 + 1U] + (*vertexCoords)[vertexIndex1 + 1U] + (*vertexCoords)[vertexIndex2 + 1U]) / static_cast<Coord_t>(3.0);
    (*baryCenterCoords)[offset + 2U] = ((*vertexCoords)[vertexIndex0 + 2U] + (*vertexCoords)[vertexIndex1 + 2U] + (*vertexCoords)[vertexIndex2 + 2U]) / static_cast<Coord_t>(3.0);
  }

  return baryCenterCoords;
}

template <class Coord_t>
std::pair<vec3_t<Coord_t>, vec3_t<Coord_t>> Geometry::calcModelBounds(const vec_pt<Coord_t> &vertexCoords) {
  const size_t nVertices = static_cast<size_t>(vertexCoords->size()) / 3ULL;

  vec3_t<Coord_t> minCoords;
  vec3_t<Coord_t> maxCoords;

  for (size_t iVertex = 0ULL; iVertex < nVertices; ++iVertex) {
    const int offset = 3ULL * iVertex;

    if (iVertex == 0ULL) {
      minCoords[0ULL] = (*vertexCoords)[offset + 0ULL];
      minCoords[1ULL] = (*vertexCoords)[offset + 1ULL];
      minCoords[2ULL] = (*vertexCoords)[offset + 2ULL];
      maxCoords[0ULL] = (*vertexCoords)[offset + 0ULL];
      maxCoords[1ULL] = (*vertexCoords)[offset + 1ULL];
      maxCoords[2ULL] = (*vertexCoords)[offset + 2ULL];
    } else {
      minCoords[0ULL] = std::min(minCoords[0ULL], (*vertexCoords)[offset + 0ULL]);
      minCoords[1ULL] = std::min(minCoords[1ULL], (*vertexCoords)[offset + 1ULL]);
      minCoords[2ULL] = std::min(minCoords[2ULL], (*vertexCoords)[offset + 2ULL]);
      maxCoords[0ULL] = std::max(maxCoords[0ULL], (*vertexCoords)[offset + 0ULL]);
      maxCoords[1ULL] = std::max(maxCoords[1ULL], (*vertexCoords)[offset + 1ULL]);
      maxCoords[2ULL] = std::max(maxCoords[2ULL], (*vertexCoords)[offset + 2ULL]);
    }
  }

  return {std::move(minCoords), std::move(maxCoords)};
}

template <class Indexing_t, class Coord_t>
vec_pt<Indexing_t> Geometry::extractSurfaceTriangle(const int &nDivsBucket,
                                                    const vec_pt<Indexing_t> &originalTriangles,
                                                    const vec_pt<Coord_t> &vertexCoords) {
  const Indexing_t nTriangles = static_cast<Indexing_t>(originalTriangles->size()) / 3U;

  // Calc bary centric coords of each triangle
  const vec_pt<Coord_t> &baryCentricCoords = calcBaryCentricCoord(originalTriangles, vertexCoords);

  // Calc model scale
  const auto [minCoords, maxCoords] = calcModelBounds(vertexCoords);

  // Create bucket
  TriangleBucket bucket;

  const Coord_t modelWidhtX = maxCoords[0] - minCoords[0];
  const Coord_t modelWidhtY = maxCoords[1] - minCoords[1];
  const Coord_t modelWidhtZ = maxCoords[2] - minCoords[2];
  const Coord_t maxModelWidth = std::max(std::max(modelWidhtX, modelWidhtY), modelWidhtZ);
  const Coord_t interval = maxModelWidth / (Coord_t)nDivsBucket;

  bucket.initBucket(interval, minCoords, maxCoords);

  // Register to bucket
  for (Indexing_t iTriangle = 0U; iTriangle < nTriangles; ++iTriangle) {
    const Indexing_t offset = 3U * iTriangle;
    const Coord_t baryCenterX = (*baryCentricCoords)[offset + 0U];
    const Coord_t baryCenterY = (*baryCentricCoords)[offset + 1U];
    const Coord_t baryCenterZ = (*baryCentricCoords)[offset + 2U];
    bucket.addToBucket(baryCenterX, baryCenterY, baryCenterZ, iTriangle);
  }

  // Extract surface
  vec_pt<Indexing_t> surfaceTriangles = std::make_shared<std::vector<Indexing_t>>();

  for (Indexing_t iTriangle = 0U; iTriangle < nTriangles; ++iTriangle) {
    bool isSurface = true;

    const Indexing_t iOffset = 3U * iTriangle;
    Indexing_t iIndex0 = (*originalTriangles)[iOffset + 0U];
    Indexing_t iIndex1 = (*originalTriangles)[iOffset + 1U];
    Indexing_t iIndex2 = (*originalTriangles)[iOffset + 2U];

    const auto &idsInBucket = bucket.getIdsInBucket((*baryCentricCoords)[iOffset + 0U],
                                                    (*baryCentricCoords)[iOffset + 1U],
                                                    (*baryCentricCoords)[iOffset + 2U]);
    const Indexing_t nInBucket = static_cast<Indexing_t>(idsInBucket->size());

    sort3Elems(iIndex0, iIndex1, iIndex2);

    for (Indexing_t jTriangle = 0U; jTriangle < nInBucket; ++jTriangle) {
      const Indexing_t id = (*idsInBucket)[jTriangle];

      if (id == iTriangle) {
        continue;
      }

      const Indexing_t jOffset = 3U * id;

      Indexing_t jIndex0 = (*originalTriangles)[jOffset + 0U];
      Indexing_t jIndex1 = (*originalTriangles)[jOffset + 1U];
      Indexing_t jIndex2 = (*originalTriangles)[jOffset + 2U];
      sort3Elems(jIndex0, jIndex1, jIndex2);

      if (iIndex0 == jIndex0 && iIndex1 == jIndex1 && iIndex2 == jIndex2) {
        isSurface = false;
        break;
      }
    }

    if (isSurface) {
      surfaceTriangles->push_back((*originalTriangles)[iOffset + 0U]);
      surfaceTriangles->push_back((*originalTriangles)[iOffset + 1U]);
      surfaceTriangles->push_back((*originalTriangles)[iOffset + 2U]);
    }
  }

  return surfaceTriangles;
}

template <class Coord_t>
void Geometry::calcVertexNormals(const int &nNodes,
                                 const vec_pt<Coord_t> &triangles) {
  // Count
  ReservedArrayi countNodes(nNodes);
  for (const auto &triangleId : *triangles) {
    countNodes.setValue(triangleId, countNodes.getValue(triangleId) + 1);
  }
}

void Geometry::removeDuplecatedVertices(
    const vecf_pt &srcVertices,
    const veci_pt &srcIndices,
    vecf_pt &distVertices,
    veci_pt &distIndices,
    const float &threshold) {
  distVertices->clear();
  distIndices->clear();

  const int nSrcCoords = srcVertices->size() / 3;

  std::vector<int> indexMap(nSrcCoords);  // "Vertex Id" to "non-deplication id"

  for (int iSrcCoord = 0; iSrcCoord < nSrcCoords; ++iSrcCoord) {
    const int srcOffset = 3 * iSrcCoord;
    const float srcCoordX = (*srcVertices)[srcOffset + 0];
    const float srcCoordY = (*srcVertices)[srcOffset + 1];
    const float srcCoordZ = (*srcVertices)[srcOffset + 2];

    bool isDeplicated = false;
    const int indexMapSize = iSrcCoord;
    int iCoord;

    for (iCoord = 0; iCoord < indexMapSize; ++iCoord) {
      const int iOffset = 3 * iCoord;
      const float iCoordX = (*distVertices)[iOffset + 0];
      const float iCoordY = (*distVertices)[iOffset + 1];
      const float iCoordZ = (*distVertices)[iOffset + 2];

      if (std::abs(srcCoordX - iCoordX) < threshold &&  // X
          std::abs(srcCoordY - iCoordY) < threshold &&  // Y
          std::abs(srcCoordZ - iCoordZ) < threshold     // Z
      ) {
        isDeplicated = true;
        break;
      }
    }

    if (isDeplicated) {
      indexMap[iSrcCoord] = iCoord;
    } else {
      indexMap[iSrcCoord] = distVertices->size() / 3;
      distVertices->push_back(srcCoordX);
      distVertices->push_back(srcCoordY);
      distVertices->push_back(srcCoordZ);
    }
  }

  const int nIndices = srcIndices->size();
  distIndices->resize(nIndices);

  for (int index = 0; index < nIndices; ++index) {
    (*distIndices)[index] = indexMap[(*srcIndices)[index]];
  }
}

}  // namespace util
}  // namespace simview