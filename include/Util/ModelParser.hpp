#pragma once
#ifndef _MODEL_PARSER_H_
#define _MODEL_PARSER_H_

#include <Model/Model.hpp>
#include <Model/Object.hpp>
#include <Model/ShaderCompiler.hpp>
#include <memory>
#include <string>

#ifndef PICOJSON_IMPLEMENTATION
#define PICOJSON_IMPLEMENTATION
#include <picojson.hpp>
#endif

class ModelParser {
 private:
  // clang-format off
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

  inline static const std::string KEY_MODEL_BOX                           = "Box";

  inline static const std::string KEY_MODEL_TERRAIN                       = "Terrain";
  
  inline static const std::string KEY_SHADER                              = "Shader";
  inline static const std::string KEY_SHADER_VERT_SHADER_PATH             = "VertShaderPath";
  inline static const std::string KEY_SHADER_FRAG_SHADER_PATH             = "FragShaderPath";
  // clang-format on

 public:
  static void parse(std::string filePath, std::shared_ptr<Model> model);

  static void parseShader(
      const std::shared_ptr<picojson::value> jsonValueShader, GLuint& shaderID);

  static void parseBackgroundColor(
      const std::shared_ptr<picojson::value> jsonValueBackground,
      std::shared_ptr<Model> model);

  static void parseModel(const std::shared_ptr<picojson::value> jsonValueModel,
                         std::shared_ptr<Model> model, const GLuint& shaderID);
  static void parseModelBackground(
      const std::shared_ptr<picojson::value> jsonValueModelBackground,
      std::shared_ptr<Model> model, const GLuint& shaderID);
  static void parseModelBox(
      const std::shared_ptr<picojson::value> jsonValueModelBox,
      std::shared_ptr<Model> model, const GLuint& shaderID);
  static void parseModelObject(
      const std::shared_ptr<picojson::value> jsonValueModelObject,
      std::shared_ptr<Model> model, const GLuint& shaderID);
  static void parseModelTerrain(
      const std::shared_ptr<picojson::value> jsonValueModelTerrain,
      std::shared_ptr<Model> model, const GLuint& shaderID);
};

#endif