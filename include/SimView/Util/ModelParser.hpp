#pragma once

#include <SimView/Model/Box.hpp>
#include <SimView/Model/Model.hpp>
#include <SimView/Model/Object.hpp>
#include <SimView/Model/Terrain.hpp>
#include <SimView/Util/FileUtil.hpp>
#include <memory>
#include <string>

#ifndef PICOJSON_IMPLEMENTATION
#define PICOJSON_IMPLEMENTATION

#include <picojson.h>

#endif

namespace simview {
namespace util {

class ModelParser {
 private:
  // clang-format off
    inline static const std::string KEY_COMMON = "Common";
    inline static const std::string KEY_COMMON_ROOT_DIR = "RootDir";
  // clang-format on

 public:
  using Value_t = std::shared_ptr<picojson::value>;
  using Model_t = std::shared_ptr<model::Model>;
  using String_t = std::shared_ptr<std::string>;

 private:
  static void autoCompPath(const String_t path, const String_t rootDirPath);

 public:
  static void parse(std::string filePath, Model_t model);

  static void parseCommon(const Value_t jsonValueCommon, String_t rootDirPath);

  static void parseShader(const Value_t jsonValueShader, Model_t model, const String_t rootDirPath);

  static void parseBackgroundColor(const Value_t jsonValueBackground, Model_t model, const String_t rootDirPath);

  static void parseModel(const Value_t jsonValueModel, Model_t model, const String_t rootDirPath);

  static void parseModelBackground(const Value_t jsonValueModelBackground, Model_t model, const String_t rootDirPath);

  static void parseModelBox(const Value_t jsonValueModelBox, Model_t model, const String_t rootDirPath);

  static void parseModelObject(const Value_t jsonValueModelObject, Model_t model, const String_t rootDirPath);

  static void parseModelTerrain(const Value_t jsonValueModelTerrain, Model_t model, const String_t rootDirPath);
};

}  // namespace util
}  // namespace simview