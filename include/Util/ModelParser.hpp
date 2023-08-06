#pragma once
#ifndef _MODEL_PARSER_H_
#define _MODEL_PARSER_H_

#include <Model/Box.hpp>
#include <Model/Model.hpp>
#include <Model/Object.hpp>
#include <Model/ShaderCompiler.hpp>
#include <Model/Terrain.hpp>
#include <Util/FileUtil.hpp>
#include <memory>
#include <string>

#ifndef PICOJSON_IMPLEMENTATION
#define PICOJSON_IMPLEMENTATION
#include <picojson.hpp>
#endif

class ModelParser {
 private:
  // clang-format off
  inline static const std::string KEY_COMMON                              = "Common";
  inline static const std::string KEY_COMMON_ROOT_DIR                     = "RootDir";

  inline static const std::string KEY_BACKGROUND                          = "Background";
  inline static const std::string KEY_BACKGROUND_COLOR                    = "Color";

  inline static const std::string KEY_MODEL                               = "Model";
  inline static const std::string KEY_MODEL_OBJECT                        = "Object";

  inline static const std::string KEY_MODEL_OBJECT_OBJ_PATH               = "ObjPath";
  inline static const std::string KEY_MODEL_OBJECT_TEXTURE_PATH           = "TexturePath";
  inline static const std::string KEY_MODEL_OBJECT_DEFAULT_RENDER_TYPE    = "DefaultRenderType";
  inline static const std::string KEY_MODEL_OBJECT_SCALE                  = "Scale";
  inline static const std::string KEY_MODEL_OBJECT_OFFSET                 = "Offset";

  inline static const std::string KEY_MODEL_BACKGROUND                    = "Background";
  inline static const std::string KEY_MODEL_BACKGROUND_PATH               = "Path";

  inline static const std::string KEY_MODEL_BOX                           = "Box";
  inline static const std::string KEY_MODEL_BOX_CENTER                    = "Center";
  inline static const std::string KEY_MODEL_BOX_SCALE                     = "Scale";

  inline static const std::string KEY_MODEL_TERRAIN                       = "Terrain";
  inline static const std::string KEY_MODEL_TERRAIN_HEIGHT_MAP_PATH       = "HeightMapPath";
  inline static const std::string KEY_MODEL_TERRAIN_CENTER                = "Center";
  inline static const std::string KEY_MODEL_TERRAIN_SCALE_XY              = "ScaleXY";
  inline static const std::string KEY_MODEL_TERRAIN_SCALE_H               = "ScaleH";

  inline static const std::string KEY_SHADER                              = "Shader";
  inline static const std::string KEY_SHADER_VERT_SHADER_PATH             = "VertShaderPath";
  inline static const std::string KEY_SHADER_FRAG_SHADER_PATH             = "FragShaderPath";
  // clang-format on

 public:
  using pValue = std::shared_ptr<picojson::value>;
  using pModel = std::shared_ptr<Model>;
  using pString = std::shared_ptr<std::string>;

 private:
  static void autoCompPath(const pString path, const pString rootDirPath);

 public:
  static void parse(std::string filePath, pModel model);

  static void parseCommon(const pValue jsonValueCommon, pString rootDirPath);

  static void parseShader(const pValue jsonValueShader, GLuint& shaderID, const pString rootDirPath);

  static void parseBackgroundColor(const pValue jsonValueBackground, pModel model, const pString rootDirPath);

  static void parseModel(const pValue jsonValueModel, pModel model, const GLuint& shaderID, const pString rootDirPath);
  static void parseModelBackground(const pValue jsonValueModelBackground, pModel model, const GLuint& shaderID, const pString rootDirPath);
  static void parseModelBox(const pValue jsonValueModelBox, pModel model, const GLuint& shaderID, const pString rootDirPath);
  static void parseModelObject(const pValue jsonValueModelObject, pModel model, const GLuint& shaderID, const pString rootDirPath);
  static void parseModelTerrain(const pValue jsonValueModelTerrain, pModel model, const GLuint& shaderID, const pString rootDirPath);
};

#endif