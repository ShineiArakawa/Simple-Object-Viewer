#include <Window/ObjectAddPopup.hpp>

namespace oglt {
namespace window {

using namespace model;
using namespace util;

ObjectAddFileDialog::ObjectAddFileDialog(std::shared_ptr<ViewerModel> model) : _model(model) {
  _objectTypes = ""s;
  _objectTypes += Object::KEY_MODEL_OBJECT + "\0"s;
  _objectTypes += Box::KEY_MODEL_BOX + "\0"s;
  _objectTypes += Terrain::KEY_MODEL_TERRAIN + "\0"s;
  _objectTypes += Background::KEY_MODEL_BACKGROUND + "\0"s;
  _objectTypes += Sphere::KEY_MODEL_SPHERE + "\0"s;
  _objectTypes += PointCloudPoly::KEY_MODEL_POINT_CLOUD_POLY + "\0"s;
  _objectTypes += PointCloud::KEY_MODEL_POINT_CLOUD + "\0"s;
  _objectTypes += MaterialObject::KEY_MODEL_MATERIAL_OBJECT + "\0"s;
}

ObjectAddFileDialog::~ObjectAddFileDialog() {}

void ObjectAddFileDialog::paint() {
  static int objectTypeID = 0;
  static char objName[256];
  static char objFilePath[256];
  static char textureFilePath[256];
  static char heightMapFilePath[256];
  static char normalMapFilePath[256];
  static float offsetXYZ[3] = {0.0f, 0.0f, 0.0f};
  static float scale = 1.0f;
  static float scaleXYZ[3] = {1.0f, 1.0f, 1.0f};
  static float color[3] = {1.0f, 1.0f, 1.0f};
  static int nDivs = 100;
  static float pointSize = 0.01f;
  static bool isDoubled = true;

  ImGui::Text("Step 1. Select the object type");
  ImGui::Combo("Object Type", &objectTypeID, _objectTypes.c_str());

  ImGui::Text("Step 2. Set each attribute");
  // ========================================================================================================================
  // Create GUI for attributes
  // ========================================================================================================================
  if (objectTypeID == 0) {
    // ====================================================================
    // Object
    // ====================================================================
    ImGui::InputText("Obj Name", objName, 256);

    ImGui::InputText("Obj file path", objFilePath, 256);
    ImGui::SameLine();
    if (ImGui::Button("Browse Obj")) {
      nfdchar_t* outPath;
      nfdfilteritem_t filterItem[1] = {{"Mesh", "obj"}};
      nfdresult_t result = NFD_OpenDialog(&outPath, filterItem, 1, FileUtil::cwd().c_str());

      if (result == NFD_OKAY) {
        strcpy(objFilePath, outPath);
      }
    }

    ImGui::InputText("Texture file path", textureFilePath, 256);
    ImGui::SameLine();
    if (ImGui::Button("Browse Texture")) {
      nfdchar_t* outPath;
      nfdfilteritem_t filterItem[1] = {{"Texture", "png,jpg"}};
      nfdresult_t result = NFD_OpenDialog(&outPath, filterItem, 1, FileUtil::cwd().c_str());

      if (result == NFD_OKAY) {
        strcpy(textureFilePath, outPath);
      }
    }

    ImGui::InputText("Normal map file path", normalMapFilePath, 256);
    ImGui::SameLine();
    if (ImGui::Button("Browse Normal Map")) {
      nfdchar_t* outPath;
      nfdfilteritem_t filterItem[1] = {{"Normal map", "png,jpg"}};
      nfdresult_t result = NFD_OpenDialog(&outPath, filterItem, 1, FileUtil::cwd().c_str());

      if (result == NFD_OKAY) {
        strcpy(normalMapFilePath, outPath);
      }
    }

    ImGui::InputFloat3("Offset (X, Y, Z)", offsetXYZ, FLOAT_FORMAT);

    ImGui::InputFloat("Scale", &scale, 0.0f, 0.0f, FLOAT_FORMAT);
  } else if (objectTypeID == 1) {
    // ====================================================================
    // Box
    // ====================================================================
    ImGui::InputText("Obj Name", objName, 256);
    ImGui::InputFloat3("Offset (X, Y, Z)", offsetXYZ, FLOAT_FORMAT);
    ImGui::InputFloat3("Scale (X, Y, Z)", scaleXYZ, FLOAT_FORMAT);

    ImGui::InputText("Texture file path", textureFilePath, 256);
    ImGui::SameLine();
    if (ImGui::Button("Browse Texture")) {
      nfdchar_t* outPath;
      nfdfilteritem_t filterItem[1] = {{"Texture", "png,jpg"}};
      nfdresult_t result = NFD_OpenDialog(&outPath, filterItem, 1, FileUtil::cwd().c_str());

      if (result == NFD_OKAY) {
        strcpy(textureFilePath, outPath);
      }
    }
  } else if (objectTypeID == 2) {
    // ====================================================================
    // Terrain
    // ====================================================================
    ImGui::InputText("Obj Name", objName, 256);
    ImGui::InputText("Height map file path", heightMapFilePath, 256);
    ImGui::SameLine();
    if (ImGui::Button("Browse Obj")) {
      nfdchar_t* outPath;
      nfdfilteritem_t filterItem[1] = {{"Height map", "png,jpg"}};
      nfdresult_t result = NFD_OpenDialog(&outPath, filterItem, 1, FileUtil::cwd().c_str());

      if (result == NFD_OKAY) {
        strcpy(heightMapFilePath, outPath);
      }
    }
    ImGui::InputFloat3("Offset (X, Y, Z)", offsetXYZ, FLOAT_FORMAT);
    ImGui::InputFloat3("Scale (X, Y, H)", scaleXYZ, FLOAT_FORMAT);
  } else if (objectTypeID == 3) {
    // ====================================================================
    // Background
    // ====================================================================
    ImGui::InputText("Obj Name", objName, 256);
    ImGui::InputText("Texture file path", textureFilePath, 256);
    ImGui::SameLine();
    if (ImGui::Button("Browse texture")) {
      nfdchar_t* outPath;
      nfdfilteritem_t filterItem[1] = {{"Texture", "png,jpg"}};
      nfdresult_t result = NFD_OpenDialog(&outPath, filterItem, 1, FileUtil::cwd().c_str());

      if (result == NFD_OKAY) {
        strcpy(textureFilePath, outPath);
      }
    }
  } else if (objectTypeID == 4) {
    // ====================================================================
    // Sphere
    // ====================================================================
    ImGui::InputText("Obj Name", objName, 256);
    ImGui::InputInt("Num divisions", &nDivs);
    ImGui::InputFloat3("Offset (X, Y, Z)", offsetXYZ, FLOAT_FORMAT);
    ImGui::InputFloat3("Scale (X, Y, Z)", scaleXYZ, FLOAT_FORMAT);
    ImGui::ColorEdit3("Color", color);
  } else if (objectTypeID == 5) {
    // ====================================================================
    // Point cloud with polygon
    // ====================================================================
    ImGui::InputText("Obj Name", objName, 256);
    ImGui::InputText("Point cloud file path", objFilePath, 256);
    ImGui::SameLine();
    if (ImGui::Button("Browse object file")) {
      nfdchar_t* outPath;
      nfdfilteritem_t filterItem[1] = {{"Mesh", "obj"}};
      nfdresult_t result = NFD_OpenDialog(&outPath, filterItem, 1, FileUtil::cwd().c_str());

      if (result == NFD_OKAY) {
        strcpy(objFilePath, outPath);
      }
    }
    ImGui::InputFloat3("Offset (X, Y, Z)", offsetXYZ, FLOAT_FORMAT);
    ImGui::InputFloat("Scale", &scale);
    ImGui::InputFloat("Point size", &pointSize, 0.0f, 0.0f, FLOAT_FORMAT);
    ImGui::Checkbox("Doubled mesh", &isDoubled);
  } else if (objectTypeID == 6) {
    // ====================================================================
    // Point cloud
    // ====================================================================
    ImGui::InputText("Obj Name", objName, 256);
    ImGui::InputText("Point cloud file path", objFilePath, 256);
    ImGui::SameLine();
    if (ImGui::Button("Browse object file")) {
      nfdchar_t* outPath;
      nfdfilteritem_t filterItem[1] = {{"Mesh", "obj"}};
      nfdresult_t result = NFD_OpenDialog(&outPath, filterItem, 1, FileUtil::cwd().c_str());

      if (result == NFD_OKAY) {
        strcpy(objFilePath, outPath);
      }
    }
    ImGui::InputFloat3("Offset (X, Y, Z)", offsetXYZ, FLOAT_FORMAT);
    ImGui::InputFloat("Scale", &scale);
    ImGui::InputFloat("Point size", &pointSize, 0.0f, 0.0f, FLOAT_FORMAT);
  } else if (objectTypeID == 7) {
    // ====================================================================
    // Materialed object
    // ====================================================================
    ImGui::InputText("Obj Name", objName, 256);
    ImGui::InputText("Obj file path", objFilePath, 256);
    ImGui::SameLine();
    if (ImGui::Button("Browse Obj")) {
      nfdchar_t* outPath;
      nfdfilteritem_t filterItem[1] = {{"Mesh", "obj"}};
      nfdresult_t result = NFD_OpenDialog(&outPath, filterItem, 1, FileUtil::cwd().c_str());

      if (result == NFD_OKAY) {
        strcpy(objFilePath, outPath);
      }
    }
    ImGui::InputFloat3("Offset (X, Y, Z)", offsetXYZ, FLOAT_FORMAT);
    ImGui::InputFloat3("Offset (X, Y, Z)", scaleXYZ, FLOAT_FORMAT);
    ImGui::InputFloat("Point size", &pointSize, 0.0f, 0.0f, FLOAT_FORMAT);
  }

  // ========================================================================================================================
  // Create GUI button
  // ========================================================================================================================
  ImGui::Separator();
  if (ImGui::Button("Cancel")) {
    ImGui::CloseCurrentPopup();  // close
    _message.clear();
    _errorMessage.clear();
  }
  ImGui::SameLine();
  if (ImGui::Button("Add")) {
    // ========================================================================================================================
    // Create objects
    // ========================================================================================================================
    Model::t_object newObject = nullptr;
    std::string strObjName(objName);
    std::string strObjFilePath(objFilePath);
    std::string strTexFilePath(textureFilePath);
    std::string strHeightMapFilePath(heightMapFilePath);
    std::string strNormalMapFilePath(normalMapFilePath);
    std::string strTextureFilePath(textureFilePath);

    try {
      _message = "Initializing ...";

      if (objectTypeID == 0) {
        // ====================================================================
        // Object
        // ====================================================================
        auto object = std::make_shared<Object>(strObjFilePath,
                                               offsetXYZ[0],
                                               offsetXYZ[1],
                                               offsetXYZ[2],
                                               scale);
        if (!strTexFilePath.empty()) {
          object->loadTexture(strTexFilePath);
        }

        if (!strNormalMapFilePath.empty()) {
          object->loadNormalMap(strNormalMapFilePath);
        }

        newObject = object;
      } else if (objectTypeID == 1) {
        // ====================================================================
        // Box
        // ====================================================================
        auto object = std::make_shared<Box>(offsetXYZ[0],
                                            offsetXYZ[1],
                                            offsetXYZ[2],
                                            scaleXYZ[0],
                                            scaleXYZ[1],
                                            scaleXYZ[2]);
        if (!strTexFilePath.empty()) {
          object->loadTexture(strTexFilePath);
        }

        newObject = object;
      } else if (objectTypeID == 2) {
        // ====================================================================
        // Terrain
        // ====================================================================
        newObject = std::make_shared<Terrain>(strHeightMapFilePath,
                                              offsetXYZ[0],
                                              offsetXYZ[1],
                                              offsetXYZ[2],
                                              scaleXYZ[0],
                                              scaleXYZ[1],
                                              scaleXYZ[2]);
      } else if (objectTypeID == 3) {
        // ====================================================================
        // Backgound
        // ====================================================================
        newObject = std::make_shared<Background>(strTextureFilePath);
      } else if (objectTypeID == 4) {
        // ====================================================================
        // Sphere
        // ====================================================================
        newObject = std::make_shared<Sphere>(nDivs,
                                             offsetXYZ[0],
                                             offsetXYZ[1],
                                             offsetXYZ[2],
                                             scaleXYZ[0],
                                             scaleXYZ[1],
                                             scaleXYZ[2],
                                             glm::vec3(color[0], color[1], color[2]));
      } else if (objectTypeID == 5) {
        // ====================================================================
        // Point cloud with polygon
        // ====================================================================
        newObject = std::make_shared<PointCloudPoly>(strObjFilePath,
                                                     offsetXYZ[0],
                                                     offsetXYZ[1],
                                                     offsetXYZ[2],
                                                     scale,
                                                     pointSize,
                                                     isDoubled);
      } else if (objectTypeID == 6) {
        // ====================================================================
        // Point cloud
        // ====================================================================
        newObject = std::make_shared<PointCloud>(strObjFilePath,
                                                 offsetXYZ[0],
                                                 offsetXYZ[1],
                                                 offsetXYZ[2],
                                                 scale,
                                                 pointSize);
      } else if (objectTypeID == 7) {
        // ====================================================================
        // Materialed object
        // ====================================================================
        newObject = std::make_shared<MaterialObject>(strObjFilePath,
                                                     glm::vec3(offsetXYZ[0], offsetXYZ[1], offsetXYZ[2]),
                                                     glm::vec3(scaleXYZ[0], scaleXYZ[1], scaleXYZ[2]));
      }

      if (newObject != nullptr) {
        newObject->setName(strObjName);

        if (objectTypeID == 3) {
          _model->addBackground(std::move(std::static_pointer_cast<Background>(newObject)));
        } else {
          _model->addObject(std::move(newObject));
        }

        _errorMessage.clear();
        _message = "Success";
      }

    } catch (const std::exception& error) {
      std::cerr << "[Error] See below messages." << std::endl;
      std::cerr << error.what() << std::endl;
      _errorMessage = "Failed to add a new object. Check attributes.";
    }
  }

  if (!_errorMessage.empty()) {
    ImGui::Separator();
    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), _errorMessage.c_str());
  } else if (!_message.empty()) {
    ImGui::Separator();
    ImGui::Text(_message.c_str());
  }
}

}  // namespace window
}  // namespace oglt