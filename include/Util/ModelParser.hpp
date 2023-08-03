#pragma once
#ifndef _MODEL_PARSER_H_
#define _MODEL_PARSER_H_

#include <Model/Model.hpp>
#include <string>

class ModelParser {
 private:
  inline static const std::string KEY_BACKGROUND = "Background";
  inline static const std::string KEY_BACKGROUND_COLOR = "Color";

  inline static const std::string KEY_MODEL = "Model";
  inline static const std::string KEY_MODEL_OBJECT = "Object";

  inline static const std::string KEY_MODEL_OBJECT_OBJ_PATH = "ObjPath";
  inline static const std::string KEY_MODEL_OBJECT_TEXTURE_PATH = "TexturePath";
  inline static const std::string KEY_MODEL_OBJECT_DEFAULT_RENDER_TYPE = "DefaultRenderType";
  inline static const std::string KEY_MODEL_OBJECT_SCALE = "Scale";
  inline static const std::string KEY_MODEL_OBJECT_OFFSET = "Offset";

 public:
  static void parse(std::string filePath, std::shared_ptr<Model> model);
};

#endif