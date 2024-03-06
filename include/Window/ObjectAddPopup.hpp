#pragma once

#include <nfd.h>

#include <ImGui.hpp>
#include <Model/Background.hpp>
#include <Model/Box.hpp>
#include <Model/MaterialObject.hpp>
#include <Model/Object.hpp>
#include <Model/PointCloud.hpp>
#include <Model/PointCloudPoly.hpp>
#include <Model/Sphere.hpp>
#include <Model/Terrain.hpp>
#include <Model/ViewerModel.hpp>
#include <OpenGL.hpp>
#include <Window/ImGuiSceneView.hpp>
#include <map>
#include <memory>
#include <string>
#include <variant>

namespace simview {
namespace window {
using namespace std::string_literals;
class ObjectAddFileDialog {
 private:
  inline static const char* FLOAT_FORMAT = "%.6f";

  using Attrib_t = std::variant<int, float, std::string>;

  inline static const std::string TITLE = "Add Object";

  std::string _objectTypes;
  std::shared_ptr<model::ViewerModel> _model;
  std::map<std::string, Attrib_t> _attributeBuffer;
  std::string _message = "";
  std::string _errorMessage = "";

 public:
  ObjectAddFileDialog(std::shared_ptr<model::ViewerModel>);
  ~ObjectAddFileDialog();

  void paint();
};
}  // namespace window
}  // namespace simview