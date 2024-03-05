#pragma once

#include <Model/Box.hpp>
#include <Model/Model.hpp>
#include <Model/Object.hpp>
#include <Model/Terrain.hpp>
#include <Util/FileUtil.hpp>
#include <memory>
#include <string>

#ifndef PICOJSON_IMPLEMENTATION
#define PICOJSON_IMPLEMENTATION

#include <picojson.h>

#endif

namespace oglt {
namespace util {
class ModelParser {
 private:
  // clang-format off
    inline static const std::string KEY_COMMON = "Common";
    inline static const std::string KEY_COMMON_ROOT_DIR = "RootDir";
  // clang-format on

 public:
  using pValue = std::shared_ptr<picojson::value>;
  using pModel = std::shared_ptr<model::Model>;
  using pString = std::shared_ptr<std::string>;

 private:
  static void autoCompPath(const pString path, const pString rootDirPath);

 public:
  static void parse(std::string filePath, pModel model);

  static void parseCommon(const pValue jsonValueCommon, pString rootDirPath);

  static void parseShader(const pValue jsonValueShader, pModel model, const pString rootDirPath);

  static void parseBackgroundColor(const pValue jsonValueBackground, pModel model, const pString rootDirPath);

  static void parseModel(const pValue jsonValueModel, pModel model, const pString rootDirPath);

  static void parseModelBackground(const pValue jsonValueModelBackground, pModel model, const pString rootDirPath);

  static void parseModelBox(const pValue jsonValueModelBox, pModel model, const pString rootDirPath);

  static void parseModelObject(const pValue jsonValueModelObject, pModel model, const pString rootDirPath);

  static void parseModelTerrain(const pValue jsonValueModelTerrain, pModel model, const pString rootDirPath);
};

}  // namespace util
}  // namespace oglt