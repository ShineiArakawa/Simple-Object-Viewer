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
#include <Util/ObjectLoader.hpp>
#include <Window/ImGuiSceneView.hpp>
#include <map>
#include <memory>
#include <string>
#include <thread>

namespace simview {
namespace window {

using namespace std::string_literals;

class ImGuiObjectAddPanel {
 private:
  inline static const char* FLOAT_FORMAT = "%.6f";

  inline static const std::string TITLE = "Add Object";

  std::string _objectTypes;
  model::ViewerModel_t _model;
  std::string _message = "";
  std::string _errorMessage = "";
  std::string _readableExtensions;

 public:
  ImGuiObjectAddPanel(model::ViewerModel_t);
  ~ImGuiObjectAddPanel();

  void paint();
};

using ObjectAddFileDialog_t = std::shared_ptr<ImGuiObjectAddPanel>;

}  // namespace window
}  // namespace simview