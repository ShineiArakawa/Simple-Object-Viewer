#include <SimView/Window/ImGuiObjectAddPanel.hpp>

namespace simview {
namespace window {

using namespace model;
using namespace util;

ImGuiObjectAddPanel::ImGuiObjectAddPanel(ViewerModel_t model)
    : _model(model),
      _message(),
      _errorMessage(),
      _readableExtensions() {
  {
    // Update loadable primitive types
    _objectTypes = ""s;
    _objectTypes += Object::KEY_MODEL_OBJECT + "\0"s;
    _objectTypes += Box::KEY_MODEL_BOX + "\0"s;
    _objectTypes += Terrain::KEY_MODEL_TERRAIN + "\0"s;
    _objectTypes += Background::KEY_MODEL_BACKGROUND + "\0"s;
    _objectTypes += Sphere::KEY_MODEL_SPHERE + "\0"s;
    _objectTypes += PointCloudPoly::KEY_MODEL_POINT_CLOUD_POLY + "\0"s;
    _objectTypes += PointCloud::KEY_MODEL_POINT_CLOUD + "\0"s;
    _objectTypes += MaterialObject::KEY_MODEL_MATERIAL_OBJECT + "\0"s;
    _objectTypes += TextBox::KEY_MODEL_TEXT_BOX + "\0"s;
  }

  {
    // Update loadable object file extensions
    const auto extensionList = ObjectLoader::getReadableExtensionList();
    for (int iExtension = 0; iExtension < extensionList.size(); ++iExtension) {
      _readableExtensions += extensionList[iExtension];
      if (iExtension != extensionList.size() - 1) {
        _readableExtensions += ",";
      }
    }
  }
}

ImGuiObjectAddPanel::~ImGuiObjectAddPanel() {}

void ImGuiObjectAddPanel::paint() {
  static int objectTypeID = 0;
  static char objName[CHAR_BUFFER_SIZE];
  static char objFilePath[CHAR_BUFFER_SIZE];
  static char textureFilePath[CHAR_BUFFER_SIZE];
  static char heightMapFilePath[CHAR_BUFFER_SIZE];
  static char normalMapFilePath[CHAR_BUFFER_SIZE];
  static float offsetXYZ[3] = {0.0f, 0.0f, 0.0f};
  static float scale = 1.0f;
  static float scaleXYZ[3] = {1.0f, 1.0f, 1.0f};
  static float color[3] = {1.0f, 1.0f, 1.0f};
  static int nDivs = 100;
  static float pointSize = 0.01f;
  static bool isDoubled = true;
  static char text[CHAR_BUFFER_SIZE] = "Hello, world!";
  static int fontPixelSize = 64;
  static int fontPadding = 16;
  static float screenSpacePos[2] = {0.0f, 0.0f};
  static float textBoxMag = 1.0f;

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
    ImGui::InputText("Obj name", objName, CHAR_BUFFER_SIZE);

    ImGui::InputText("Obj file path", objFilePath, CHAR_BUFFER_SIZE);
    ImGui::SameLine();
    if (ImGui::Button("Browse obj")) {
      nfdchar_t* outPath;
      nfdfilteritem_t filterItem[1] = {{"Mesh", _readableExtensions.c_str()}};
      nfdresult_t result = NFD_OpenDialog(&outPath, filterItem, 1, FileUtil::cwd().c_str());

      if (result == NFD_OKAY) {
        strcpy(objFilePath, outPath);
        NFD_FreePath(outPath);
      }
    }

    ImGui::InputText("Texture file path", textureFilePath, CHAR_BUFFER_SIZE);
    ImGui::SameLine();
    if (ImGui::Button("Browse texture")) {
      nfdchar_t* outPath;
      nfdfilteritem_t filterItem[1] = {{"Texture", "png,jpg"}};
      nfdresult_t result = NFD_OpenDialog(&outPath, filterItem, 1, FileUtil::cwd().c_str());

      if (result == NFD_OKAY) {
        strcpy(textureFilePath, outPath);
        NFD_FreePath(outPath);
      }
    }

    ImGui::InputText("Normal map file path", normalMapFilePath, CHAR_BUFFER_SIZE);
    ImGui::SameLine();
    if (ImGui::Button("Browse normal map")) {
      nfdchar_t* outPath;
      nfdfilteritem_t filterItem[1] = {{"Normal map", "png,jpg"}};
      nfdresult_t result = NFD_OpenDialog(&outPath, filterItem, 1, FileUtil::cwd().c_str());

      if (result == NFD_OKAY) {
        strcpy(normalMapFilePath, outPath);
        NFD_FreePath(outPath);
      }
    }

    ImGui::InputFloat3("Offset (X, Y, Z)", offsetXYZ, FLOAT_FORMAT);

    ImGui::InputFloat("Scale", &scale, 0.0f, 0.0f, FLOAT_FORMAT);
  } else if (objectTypeID == 1) {
    // ====================================================================
    // Box
    // ====================================================================
    ImGui::InputText("Obj name", objName, CHAR_BUFFER_SIZE);
    ImGui::InputFloat3("Offset (X, Y, Z)", offsetXYZ, FLOAT_FORMAT);
    ImGui::InputFloat3("Scale (X, Y, Z)", scaleXYZ, FLOAT_FORMAT);

    ImGui::InputText("Texture file path", textureFilePath, CHAR_BUFFER_SIZE);
    ImGui::SameLine();
    if (ImGui::Button("Browse texture")) {
      nfdchar_t* outPath;
      nfdfilteritem_t filterItem[1] = {{"Texture", "png,jpg"}};
      nfdresult_t result = NFD_OpenDialog(&outPath, filterItem, 1, FileUtil::cwd().c_str());

      if (result == NFD_OKAY) {
        strcpy(textureFilePath, outPath);
        NFD_FreePath(outPath);
      }
    }
  } else if (objectTypeID == 2) {
    // ====================================================================
    // Terrain
    // ====================================================================
    ImGui::InputText("Obj name", objName, CHAR_BUFFER_SIZE);
    ImGui::InputText("Height map file path", heightMapFilePath, CHAR_BUFFER_SIZE);
    ImGui::SameLine();
    if (ImGui::Button("Browse obj")) {
      nfdchar_t* outPath;
      nfdfilteritem_t filterItem[1] = {{"Height map", "png,jpg"}};
      nfdresult_t result = NFD_OpenDialog(&outPath, filterItem, 1, FileUtil::cwd().c_str());

      if (result == NFD_OKAY) {
        strcpy(heightMapFilePath, outPath);
        NFD_FreePath(outPath);
      }
    }
    ImGui::InputFloat3("Offset (X, Y, Z)", offsetXYZ, FLOAT_FORMAT);
    ImGui::InputFloat3("Scale (X, Y, H)", scaleXYZ, FLOAT_FORMAT);
  } else if (objectTypeID == 3) {
    // ====================================================================
    // Background
    // ====================================================================
    ImGui::InputText("Obj name", objName, CHAR_BUFFER_SIZE);
    ImGui::InputText("Texture file path", textureFilePath, CHAR_BUFFER_SIZE);
    ImGui::SameLine();
    if (ImGui::Button("Browse texture")) {
      nfdchar_t* outPath;
      nfdfilteritem_t filterItem[1] = {{"Texture", "png,jpg"}};
      nfdresult_t result = NFD_OpenDialog(&outPath, filterItem, 1, FileUtil::cwd().c_str());

      if (result == NFD_OKAY) {
        strcpy(textureFilePath, outPath);
        NFD_FreePath(outPath);
      }
    }
  } else if (objectTypeID == 4) {
    // ====================================================================
    // Sphere
    // ====================================================================
    ImGui::InputText("Obj name", objName, CHAR_BUFFER_SIZE);
    ImGui::InputInt("Num divisions", &nDivs);
    ImGui::InputFloat3("Offset (X, Y, Z)", offsetXYZ, FLOAT_FORMAT);
    ImGui::InputFloat3("Scale (X, Y, Z)", scaleXYZ, FLOAT_FORMAT);
    ImGui::ColorEdit3("Color", color);
  } else if (objectTypeID == 5) {
    // ====================================================================
    // Point cloud with polygon
    // ====================================================================
    ImGui::InputText("Obj name", objName, CHAR_BUFFER_SIZE);
    ImGui::InputText("Point cloud file path", objFilePath, CHAR_BUFFER_SIZE);
    ImGui::SameLine();
    if (ImGui::Button("Browse object file")) {
      nfdchar_t* outPath;
      nfdfilteritem_t filterItem[1] = {{"Mesh", "obj"}};
      nfdresult_t result = NFD_OpenDialog(&outPath, filterItem, 1, FileUtil::cwd().c_str());

      if (result == NFD_OKAY) {
        strcpy(objFilePath, outPath);
        NFD_FreePath(outPath);
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
    ImGui::InputText("Obj name", objName, CHAR_BUFFER_SIZE);
    ImGui::InputText("Point cloud file path", objFilePath, CHAR_BUFFER_SIZE);
    ImGui::SameLine();
    if (ImGui::Button("Browse object file")) {
      nfdchar_t* outPath;
      nfdfilteritem_t filterItem[1] = {{"Mesh", "obj"}};
      nfdresult_t result = NFD_OpenDialog(&outPath, filterItem, 1, FileUtil::cwd().c_str());

      if (result == NFD_OKAY) {
        strcpy(objFilePath, outPath);
        NFD_FreePath(outPath);
      }
    }
    ImGui::InputFloat3("Offset (X, Y, Z)", offsetXYZ, FLOAT_FORMAT);
    ImGui::InputFloat("Scale", &scale);
    ImGui::InputFloat("Point size", &pointSize, 0.0f, 0.0f, FLOAT_FORMAT);
  } else if (objectTypeID == 7) {
    // ====================================================================
    // Materialed object
    // ====================================================================
    ImGui::InputText("Obj name", objName, CHAR_BUFFER_SIZE);
    ImGui::InputText("Obj file path", objFilePath, CHAR_BUFFER_SIZE);
    ImGui::SameLine();
    if (ImGui::Button("Browse Obj")) {
      nfdchar_t* outPath;
      nfdfilteritem_t filterItem[1] = {{"Mesh", "obj,stl"}};
      nfdresult_t result = NFD_OpenDialog(&outPath, filterItem, 1, FileUtil::cwd().c_str());

      if (result == NFD_OKAY) {
        strcpy(objFilePath, outPath);
        NFD_FreePath(outPath);
      }
    }
    ImGui::InputFloat3("Offset (X, Y, Z)", offsetXYZ, FLOAT_FORMAT);
    ImGui::InputFloat3("Scale (X, Y, Z)", scaleXYZ, FLOAT_FORMAT);
    ImGui::InputFloat("Scale", &scale);
  } else if (objectTypeID == 8) {
    // ====================================================================
    // Text box
    // ====================================================================
    ImGui::InputText("Obj name", objName, CHAR_BUFFER_SIZE);
    ImGui::InputTextMultiline("Text", text, CHAR_BUFFER_SIZE);
    ImGui::InputFloat2("Position (-1.0 < x,y < 1.0)", screenSpacePos, FLOAT_FORMAT);
    ImGui::DragFloat("Size mag", &textBoxMag, 0.001f, 0.0f, 4.0f, FLOAT_FORMAT);
    ImGui::InputInt("Font pixel size", &fontPixelSize);
    ImGui::InputInt("Font padding", &fontPadding);
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
    Primitive_t newObject = nullptr;
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
                                                     glm::vec3(scaleXYZ[0] * scale, scaleXYZ[1] * scale, scaleXYZ[2] * scale));
      } else if (objectTypeID == 8) {
        // ====================================================================
        // Text box
        // ====================================================================
        newObject = std::make_shared<TextBox>(text,
                                              glm::vec2(screenSpacePos[0], screenSpacePos[1]),
                                              textBoxMag,
                                              fontPixelSize,
                                              fontPadding);
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
    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "%s", _errorMessage.c_str());
  } else if (!_message.empty()) {
    ImGui::Separator();
    ImGui::Text("%s", _message.c_str());
  }
}

}  // namespace window
}  // namespace simview