#include <Util/ModelParser.hpp>

namespace simview {
namespace util {

using namespace model;
using namespace shader;

namespace GetValueHelpers {

template <class Type>
inline std::shared_ptr<Type> getValueFromJsonValue(picojson::value jsonValue) {
  return nullptr;
}

template <>
inline std::shared_ptr<int> getValueFromJsonValue<int>(picojson::value jsonValue) {
  std::shared_ptr<int> value = nullptr;
  if (jsonValue.is<double>()) {
    double &doubleValue = jsonValue.get<double>();
    value = std::make_shared<int>();
    *value = (int)doubleValue;
  }
  return value;
}

template <>
inline std::shared_ptr<bool> getValueFromJsonValue<bool>(picojson::value jsonValue) {
  std::shared_ptr<bool> value = nullptr;
  if (jsonValue.is<bool>()) {
    value = std::make_shared<bool>();
    *value = jsonValue.get<bool>();
  }
  return value;
}

template <>
inline std::shared_ptr<std::string> getValueFromJsonValue<std::string>(picojson::value jsonValue) {
  std::shared_ptr<std::string> value = nullptr;
  if (jsonValue.is<std::string>()) {
    value = std::make_shared<std::string>();
    *value = jsonValue.get<std::string>();
  }
  return value;
}

template <>
inline std::shared_ptr<double> getValueFromJsonValue<double>(picojson::value jsonValue) {
  std::shared_ptr<double> value = nullptr;
  if (jsonValue.is<double>()) {
    double &doubleValue = jsonValue.get<double>();
    value = std::make_shared<double>();
    *value = doubleValue;
  }
  return value;
}

template <class Type>
std::shared_ptr<Type> getValueFromJsonObject(picojson::object jsonObject) {
  picojson::value jsonValue = picojson::value(jsonObject);
  return getValueFromJsonValue<Type>(jsonValue);
}

template <class Type>
std::shared_ptr<Type> getScalarValue(std::string key, picojson::value jsonValue) {
  std::shared_ptr<Type> value = nullptr;
  if (jsonValue.contains(key)) {
    picojson::value childValue = jsonValue.get(key);
    value = getValueFromJsonValue<Type>(childValue);
  }
  return value;
};

template <>
std::shared_ptr<int> getScalarValue<int>(std::string key, picojson::value jsonValue) {
  std::shared_ptr<int> value = nullptr;
  if (jsonValue.contains(key)) {
    picojson::value childValue = jsonValue.get(key);
    value = getValueFromJsonValue<int>(childValue);
  }
  return value;
};

template <class Type>
std::vector<Type *> getValue(std::string key, picojson::value jsonValue) {
  std::vector<Type *> vec_values;

  if (jsonValue.contains(key)) {
    picojson::value &childValue = jsonValue.get(key);

    if (childValue.is<picojson::array>()) {
      // 一重配列 [...]
      picojson::array &array1 = childValue.get<picojson::array>();

      for (int i = 0; i < (int)array1.size(); i++) {
        picojson::value &iChaildValue1 = array1[i];
        std::shared_ptr<Type> value = GetValueHelpers::getValueFromJsonValue<Type>(iChaildValue1);
        Type *ptr_value = new Type();
        *ptr_value = *value;
        vec_values.push_back(ptr_value);
      }
    } else {
      std::shared_ptr<Type> value = GetValueHelpers::getValueFromJsonValue<Type>(childValue);
      Type *ptr_value = new Type();
      *ptr_value = *value;
      vec_values.push_back(ptr_value);
    }
  } else {
    Type *ptr_value = nullptr;
    vec_values.push_back(ptr_value);
  }

  return vec_values;
}

};  // namespace GetValueHelpers

void ModelParser::parseCommon(const Value_t jsonValueCommon, String_t rootDirPath) {
  auto tmp_rootDirPath = GetValueHelpers::getScalarValue<std::string>(KEY_COMMON_ROOT_DIR, *jsonValueCommon);

  if (tmp_rootDirPath == nullptr) {
    *rootDirPath = *tmp_rootDirPath;
  }
}

void ModelParser::parseShader(const Value_t jsonValueShader, Model_t model, const String_t rootDirPath) {
  String_t vertShaderPath;
  String_t fragShaderPath;

  if (jsonValueShader->contains(ShaderCompiler::KEY_SHADER_VERT_SHADER_PATH)) {
    vertShaderPath = GetValueHelpers::getScalarValue<std::string>(ShaderCompiler::KEY_SHADER_VERT_SHADER_PATH, *jsonValueShader);
  }

  if (jsonValueShader->contains(ShaderCompiler::KEY_SHADER_FRAG_SHADER_PATH)) {
    fragShaderPath = GetValueHelpers::getScalarValue<std::string>(ShaderCompiler::KEY_SHADER_FRAG_SHADER_PATH, *jsonValueShader);
  }

  if (vertShaderPath != nullptr) {
    ModelParser::autoCompPath(vertShaderPath, rootDirPath);
  }

  if (fragShaderPath != nullptr) {
    ModelParser::autoCompPath(fragShaderPath, rootDirPath);
  }

  model->setModelVertShaderPath(vertShaderPath);
  model->setModelFragShaderPath(fragShaderPath);
}

void ModelParser::parseBackgroundColor(const std::shared_ptr<picojson::value> jsonValueBackground, std::shared_ptr<Model> model, const String_t rootDirPath) {
  if (jsonValueBackground->contains(Model::KEY_BACKGROUND_COLOR)) {
    std::vector<double *> RGBA = GetValueHelpers::getValue<double>(Model::KEY_BACKGROUND_COLOR, *jsonValueBackground);

    std::array<float, 4> rgba = {0.0f, 0.0f, 0.0f, 0.0f};
    for (int i = 0; i < RGBA.size(); i++) {
      if (RGBA[i] != nullptr) {
        rgba[i] = *RGBA[i];
      }
    }

    model->setBackgroundColor(glm::vec4(rgba[0], rgba[1], rgba[2], rgba[3]));
  }
}

void ModelParser::parseModelBackground(const std::shared_ptr<picojson::value> jsonValueModelBackground, std::shared_ptr<Model> model, const String_t rootDirPath) {
  picojson::object jsonObject = jsonValueModelBackground->get<picojson::object>();

  for (picojson::object::const_iterator iter = jsonObject.begin(); iter != jsonObject.end(); ++iter) {
    std::string objectName = iter->first;
    picojson::value objectValue = iter->second;

    if (objectValue.contains(Background::KEY_MODEL_BACKGROUND_PATH)) {
      ModelParser::String_t filePath = GetValueHelpers::getScalarValue<std::string>(Background::KEY_MODEL_BACKGROUND_PATH, objectValue);

      if (filePath != nullptr) {
        ModelParser::autoCompPath(filePath, rootDirPath);
        std::shared_ptr<Background> background = std::make_shared<Background>(*filePath);
        background->setName(objectName);
        model->addBackground(background);
      }
    }
  }
}

void ModelParser::parseModelBox(const std::shared_ptr<picojson::value> jsonValueModelBox, std::shared_ptr<Model> model, const String_t rootDirPath) {
  picojson::object jsonObject = jsonValueModelBox->get<picojson::object>();

  for (picojson::object::const_iterator iter = jsonObject.begin(); iter != jsonObject.end(); ++iter) {
    std::string objectName = iter->first;
    picojson::value objectValue = iter->second;

    auto offset = GetValueHelpers::getValue<double>(Box::KEY_MODEL_BOX_CENTER, objectValue);
    auto scale = GetValueHelpers::getValue<double>(Box::KEY_MODEL_BOX_SCALE, objectValue);

    bool isValid = true;

    if (offset.size() > 2) {
      for (auto value : offset) {
        if (value == nullptr) {
          isValid = false;
          break;
        }
      }
    } else {
      isValid = false;
    }

    if (scale.size() > 2) {
      for (auto value : scale) {
        if (value == nullptr) {
          isValid = false;
          break;
        }
      }
    } else {
      isValid = false;
    }

    if (isValid) {
      std::shared_ptr<Box> box = std::make_shared<Box>((float)*offset[0], (float)*offset[1], (float)*offset[2], (float)*scale[0], (float)*scale[1], (float)*scale[2]);
      box->setName(objectName);
      model->addObject(std::move(box));
    } else {
      std::cerr << "Failed to parse Box model: " << objectName << std::endl;
    }
  }
}

void ModelParser::parseModelObject(const std::shared_ptr<picojson::value> jsonValueModelObject, std::shared_ptr<Model> model, const String_t rootDirPath) {
  picojson::object jsonObject = jsonValueModelObject->get<picojson::object>();

  for (picojson::object::const_iterator iter = jsonObject.begin(); iter != jsonObject.end(); ++iter) {
    std::string objectName = iter->first;
    picojson::value objectValue = iter->second;

    std::shared_ptr<std::string> objPath = nullptr;
    std::shared_ptr<std::string> texturePath = nullptr;
    std::shared_ptr<std::string> defaultRenderType = nullptr;
    std::shared_ptr<double> scale = nullptr;
    std::vector<double> offset;

    if (objectValue.contains(Object::KEY_MODEL_OBJECT_OBJ_PATH)) {
      objPath = GetValueHelpers::getScalarValue<std::string>(Object::KEY_MODEL_OBJECT_OBJ_PATH, objectValue);
    }

    if (objectValue.contains(Object::KEY_MODEL_OBJECT_TEXTURE_PATH)) {
      texturePath = GetValueHelpers::getScalarValue<std::string>(Object::KEY_MODEL_OBJECT_TEXTURE_PATH, objectValue);
    }

    if (objectValue.contains(Object::KEY_MODEL_OBJECT_DEFAULT_RENDER_TYPE)) {
      defaultRenderType = GetValueHelpers::getScalarValue<std::string>(Object::KEY_MODEL_OBJECT_DEFAULT_RENDER_TYPE, objectValue);
    }

    if (objectValue.contains(Object::KEY_MODEL_OBJECT_SCALE)) {
      scale = GetValueHelpers::getScalarValue<double>(Object::KEY_MODEL_OBJECT_SCALE, objectValue);

      if (scale == nullptr) {
        scale = std::make_shared<double>();
        *scale = 1.0;
      }
    }

    if (objectValue.contains(Object::KEY_MODEL_OBJECT_OFFSET)) {
      std::vector<double *> tmp_offset = GetValueHelpers::getValue<double>(Object::KEY_MODEL_OBJECT_OFFSET, objectValue);

      for (int i = 0; i < 3; i++) {
        if (tmp_offset.size() > 2 && tmp_offset[0] != nullptr) {
          offset.push_back(*tmp_offset[0]);
        } else {
          offset.push_back(0.0);
        }
      }
    }

    ModelParser::autoCompPath(objPath, rootDirPath);
    std::shared_ptr<Object> object = std::make_shared<Object>(*objPath, offset[0], offset[1], offset[2], *scale);

    if (texturePath != nullptr) {
      ModelParser::autoCompPath(texturePath, rootDirPath);
      object->loadTexture(*texturePath);
    }

    if (defaultRenderType != nullptr) {
      object->setDefaultRenderType(Primitives::getRenderType(*defaultRenderType));
    }

    object->setName(objectName);
    model->addObject(std::move(object));
  }
}

void ModelParser::parseModelTerrain(const std::shared_ptr<picojson::value> jsonValueModelTerrain, std::shared_ptr<Model> model, const String_t rootDirPath) {
  picojson::object jsonObject = jsonValueModelTerrain->get<picojson::object>();

  for (picojson::object::const_iterator iter = jsonObject.begin(); iter != jsonObject.end(); ++iter) {
    std::string objectName = iter->first;
    picojson::value objectValue = iter->second;

    auto heightMapPath = GetValueHelpers::getScalarValue<std::string>(Terrain::KEY_MODEL_TERRAIN_HEIGHT_MAP_PATH, objectValue);
    auto offset = GetValueHelpers::getValue<double>(Terrain::KEY_MODEL_TERRAIN_CENTER, objectValue);
    auto scaleXY = GetValueHelpers::getValue<double>(Terrain::KEY_MODEL_TERRAIN_SCALE_XY, objectValue);
    auto scaleH = GetValueHelpers::getScalarValue<double>(Terrain::KEY_MODEL_TERRAIN_SCALE_H, objectValue);

    bool isValid = true;

    if (heightMapPath == nullptr) {
      isValid = false;
    }

    if (offset.size() > 2) {
      for (auto value : offset) {
        if (value == nullptr) {
          isValid = false;
          break;
        }
      }
    } else {
      isValid = false;
    }

    if (scaleXY.size() > 1) {
      for (auto value : scaleXY) {
        if (value == nullptr) {
          isValid = false;
          break;
        }
      }
    } else {
      isValid = false;
    }

    if (scaleH == nullptr) {
      isValid = false;
    }

    if (isValid) {
      autoCompPath(heightMapPath, rootDirPath);
      std::shared_ptr<Terrain> terrain = std::make_shared<Terrain>(*heightMapPath, (float)*offset[0], (float)*offset[1], (float)*offset[2], (float)*scaleXY[0], (float)*scaleXY[1], (float)*scaleH);
      model->addObject(std::move(terrain));
    } else {
      std::cerr << "Failed to parse Box model: " << objectName << std::endl;
    }
  }
}

void ModelParser::parseModel(const std::shared_ptr<picojson::value> jsonValueModel, std::shared_ptr<Model> model, const String_t rootDirPath) {
  // Object
  if (jsonValueModel->contains(Object::KEY_MODEL_OBJECT)) {
    auto jsonValueModelObject = std::make_shared<picojson::value>(jsonValueModel->get(Object::KEY_MODEL_OBJECT));

    ModelParser::parseModelObject(jsonValueModelObject, model, rootDirPath);
  }

  if (jsonValueModel->contains(Background::KEY_MODEL_BACKGROUND)) {
    auto jsonValueModelBackground = std::make_shared<picojson::value>(jsonValueModel->get(Background::KEY_MODEL_BACKGROUND));

    ModelParser::parseModelBackground(jsonValueModelBackground, model, rootDirPath);
  }

  if (jsonValueModel->contains(Box::KEY_MODEL_BOX)) {
    auto jsonValueModelBox = std::make_shared<picojson::value>(jsonValueModel->get(Box::KEY_MODEL_BOX));

    ModelParser::parseModelBox(jsonValueModelBox, model, rootDirPath);
  }

  if (jsonValueModel->contains(Terrain::KEY_MODEL_TERRAIN)) {
    auto jsonValueModelTerrain = std::make_shared<picojson::value>(jsonValueModel->get(Terrain::KEY_MODEL_TERRAIN));

    ModelParser::parseModelTerrain(jsonValueModelTerrain, model, rootDirPath);
  }
}

void ModelParser::parse(std::string filePath, std::shared_ptr<Model> model) {
  std::shared_ptr<picojson::value> jsonValue(new picojson::value());

  std::ifstream fs;
  fs.open(filePath, std::ios::binary);
  if (fs) {
    fs >> (*jsonValue);
  } else {
    std::cerr << "Failed to open config file: " + FileUtil::absPath(filePath) << std::endl;
    throw std::runtime_error("Failed to open config file: " + FileUtil::absPath(filePath));
  }
  fs.close();

  String_t rootDirPath = nullptr;

  if (jsonValue->contains(KEY_COMMON)) {
    auto jsonValueCommon = std::make_shared<picojson::value>(jsonValue->get(KEY_COMMON));
    ModelParser::parseCommon(jsonValueCommon, rootDirPath);
  }

  if (rootDirPath == nullptr) {
    rootDirPath = std::make_shared<std::string>(FileUtil::dirPath(filePath));
  }
  std::cout << "### RootDirPath=" << *rootDirPath << std::endl;

  if (jsonValue->contains(ShaderCompiler::KEY_SHADER)) {
    auto jsonValueShader = std::make_shared<picojson::value>(jsonValue->get(ShaderCompiler::KEY_SHADER));
    ModelParser::parseShader(jsonValueShader, model, rootDirPath);
  }

  if (jsonValue->contains(Model::KEY_BACKGROUND)) {
    auto jsonValueBackground = std::make_shared<picojson::value>(jsonValue->get(Model::KEY_BACKGROUND));
    ModelParser::parseBackgroundColor(jsonValueBackground, model, rootDirPath);
  }

  if (jsonValue->contains(Model::KEY_MODEL)) {
    auto jsonValueModel = std::make_shared<picojson::value>(jsonValue->get(Model::KEY_MODEL));
    ModelParser::parseModel(jsonValueModel, model, rootDirPath);
  }
}

void ModelParser::autoCompPath(const String_t path, const String_t rootDirPath) {
  if (path != nullptr && rootDirPath != nullptr && !FileUtil::isAbsolute(*path)) {
    *path = FileUtil::join(*rootDirPath, *path);
  }
}

}  // namespace util
}  // namespace simview