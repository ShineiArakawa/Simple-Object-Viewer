#pragma once
#ifndef _OBJECT_LOADER_H_
#define _OBJECT_LOADER_H_

#include <Model/Primitives.hpp>
#include <OpenGL.hpp>
#include <array>
#include <memory>
#include <string>
#include <vector>

class ObjectLoader {
 private:
  /* data */
 public:
  static void readObjFile(const std::string& filePath,
                          std::shared_ptr<std::vector<Vertex>> vertices,
                          std::shared_ptr<std::vector<uint32_t>> indices,
                          const float offsetX = 0.0f,
                          const float offsetY = 0.0f,
                          const float offsetZ = 0.0f);
  static void scaleObject(std::shared_ptr<std::vector<Vertex>> vertices,
                          const float scale);
  static void scaleObject(std::shared_ptr<std::vector<Vertex>> vertices,
                          const float scaleX, const float scaleY,
                          const float scaleZ);
  static void moveToOrigin(std::shared_ptr<std::vector<Vertex>> vertices);
  static void move(std::shared_ptr<std::vector<Vertex>> vertices,
                   const float offsetX = 0.0f, const float offsetY = 0.0f,
                   const float offsetZ = 0.0f);
};
#endif