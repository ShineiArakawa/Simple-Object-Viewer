#include <Util/Geometry.hpp>

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

vecf_pt Geometry::calcBaryCentricCoord(const veci_pt &triangles,
                                       const vecf_pt &vertexCoords) {
  const int nTriangles = triangles->size() / 3;

  vecf_pt baryCenterCoords = std::make_shared<std::vector<float>>();
  baryCenterCoords->resize(3 * nTriangles);

  for (int iTriangle = 0; iTriangle < nTriangles; ++iTriangle) {
    const int offset = 3 * iTriangle;

    const int vertexIndex0 = (*triangles)[offset + 0];
    const int vertexIndex1 = (*triangles)[offset + 1];
    const int vertexIndex2 = (*triangles)[offset + 2];

    for (int axis = 0; axis < 3; ++axis) {
      const float coord0 = (*vertexCoords)[vertexIndex0 + axis];
      const float coord1 = (*vertexCoords)[vertexIndex1 + axis];
      const float coord2 = (*vertexCoords)[vertexIndex2 + axis];

      (*baryCenterCoords)[offset + axis] = (coord0 + coord1 + coord2) / 3.0f;
    }
  }

  return baryCenterCoords;
}

std::pair<vec3f_t, vec3f_t> Geometry::calcModelBounds(const vecf_pt &vertexCoords) {
  const int nVertices = vertexCoords->size() / 3;

  vec3f_t minCoords;
  vec3f_t maxCoords;

  for (int iVertex = 0; iVertex < nVertices; ++iVertex) {
    const int offset = 3 * iVertex;

    if (iVertex == 0) {
      minCoords[0] = (*vertexCoords)[offset + 0];
      minCoords[1] = (*vertexCoords)[offset + 1];
      minCoords[2] = (*vertexCoords)[offset + 2];
      maxCoords[0] = (*vertexCoords)[offset + 0];
      maxCoords[1] = (*vertexCoords)[offset + 1];
      maxCoords[2] = (*vertexCoords)[offset + 2];
    } else {
      minCoords[0] = std::min(minCoords[0], (*vertexCoords)[offset + 0]);
      minCoords[1] = std::min(minCoords[1], (*vertexCoords)[offset + 1]);
      minCoords[2] = std::min(minCoords[2], (*vertexCoords)[offset + 2]);
      maxCoords[0] = std::max(maxCoords[0], (*vertexCoords)[offset + 0]);
      maxCoords[1] = std::max(maxCoords[1], (*vertexCoords)[offset + 1]);
      maxCoords[2] = std::max(maxCoords[2], (*vertexCoords)[offset + 2]);
    }
  }

  return {std::move(minCoords), std::move(maxCoords)};
}

veci_pt Geometry::extractSurfaceTriangle(const int &nDivsBucket,
                                         const veci_pt &originalTriangles,
                                         const vecf_pt &vertexCoords) {
  const int nTriangles = originalTriangles->size() / 3;

  // Calc bary centric coords of each triangle
  const auto &baryCentricCoords = calcBaryCentricCoord(originalTriangles, vertexCoords);

  // Calc model scale
  vec3f_t minCoords, maxCoords;
  std::tie(minCoords, maxCoords) = calcModelBounds(vertexCoords);

  // Create bucket
  TriangleBucket bucket;

  const float modelWidhtX = maxCoords[0] - minCoords[0];
  const float modelWidhtY = maxCoords[1] - minCoords[1];
  const float modelWidhtZ = maxCoords[2] - minCoords[2];
  const float maxModelWidth = std::max(std::max(modelWidhtX, modelWidhtY), modelWidhtZ);
  const float interval = maxModelWidth / (float)nDivsBucket;

  bucket.initBucket(interval, minCoords, maxCoords);

  // Register to bucket
  for (int iTriangle = 0; iTriangle < nTriangles; ++iTriangle) {
    const int offset = 3 * iTriangle;
    const float baryCenterX = (*baryCentricCoords)[offset + 0];
    const float baryCenterY = (*baryCentricCoords)[offset + 1];
    const float baryCenterZ = (*baryCentricCoords)[offset + 2];
    bucket.addToBucket(baryCenterX, baryCenterY, baryCenterZ, iTriangle);
  }

  // Extract surface
  veci_pt surfaceTriangles = std::make_shared<std::vector<int>>();

  for (int iTriangle = 0; iTriangle < nTriangles; ++iTriangle) {
    bool isSurface = true;

    const int iOffset = 3 * iTriangle;
    int iIndex0 = (*originalTriangles)[iOffset + 0];
    int iIndex1 = (*originalTriangles)[iOffset + 1];
    int iIndex2 = (*originalTriangles)[iOffset + 2];

    const auto &idsInBucket = bucket.getIdsInBucket((*baryCentricCoords)[iOffset + 0],
                                                    (*baryCentricCoords)[iOffset + 1],
                                                    (*baryCentricCoords)[iOffset + 2]);
    const int nInBucket = idsInBucket->size();

    sort3Elems(iIndex0, iIndex1, iIndex2);

    for (int jTriangle = 0; jTriangle < nInBucket; ++jTriangle) {
      const int id = (*idsInBucket)[jTriangle];

      if (id == iTriangle) {
        continue;
      }

      const int jOffset = 3 * id;

      int jIndex0 = (*originalTriangles)[jOffset + 0];
      int jIndex1 = (*originalTriangles)[jOffset + 1];
      int jIndex2 = (*originalTriangles)[jOffset + 2];
      sort3Elems(jIndex0, jIndex1, jIndex2);

      if (iIndex0 == jIndex0 && iIndex1 == jIndex1 && iIndex2 == jIndex2) {
        isSurface = false;
        break;
      }
    }

    if (isSurface) {
      surfaceTriangles->push_back((*originalTriangles)[iOffset + 0]);
      surfaceTriangles->push_back((*originalTriangles)[iOffset + 1]);
      surfaceTriangles->push_back((*originalTriangles)[iOffset + 2]);
    }
  }

  return surfaceTriangles;
}

void Geometry::calcVertexNormals(const int &nNodes,
                                 const veci_pt &triangles) {
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