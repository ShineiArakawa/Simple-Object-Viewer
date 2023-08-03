#include <Util/ModelParser.hpp>

#ifndef PICOJSON_IMPLEMENTATION
#define PICOJSON_IMPLEMENTATION
#include <picojson.hpp>
#endif

namespace GetValueHelpers {

template <class Type>
std::shared_ptr<Type> getValueFromJsonObject(picojson::object jsonObject) {
  picojson::value jsonValue = picojson::value(jsonObject);
  return getValueFromJsonValue<Type>(jsonValue);
}

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

void ModelParser::parse(std::string filePath, std::shared_ptr<Model> model) {
  std::shared_ptr<picojson::value> jsonValue(new picojson::value());

  std::ifstream fs;
  fs.open(filePath, std::ios::binary);
  if (fs) {
    fs >> (*jsonValue);
  } else {
    throw std::runtime_error("Failed to open parameters file: " + filePath);
  }
  fs.close();

  if (jsonValue->contains(KEY_BACKGROUND)) {
    picojson::value jsonValueBackground = jsonValue->get(KEY_BACKGROUND);

    if (jsonValueBackground.contains(KEY_BACKGROUND_COLOR)) {
      std::vector<double *> RGB = GetValueHelpers::getValue<double>(KEY_BACKGROUND_COLOR, jsonValueBackground);
    }
  }

  if (jsonValue->contains(KEY_MODEL)) {
    picojson::value jsonValueModel = jsonValue->get(KEY_MODEL);

    if (jsonValueModel.contains(KEY_MODEL_OBJECT)) {
      picojson::object jsonValueModelObject = jsonValueModel.get(KEY_MODEL_OBJECT).get<picojson::object>();

      for (picojson::object::const_iterator iter = jsonValueModelObject.begin(); iter != jsonValueModelObject.end(); ++iter) {
        std::string objectName = iter->first;
        picojson::value objectValue = iter->second;

        
      }
    }
  }
}
