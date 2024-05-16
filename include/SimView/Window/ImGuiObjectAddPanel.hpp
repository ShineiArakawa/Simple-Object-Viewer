#pragma once

#include <nfd.h>

#include <SimView/ImGui.hpp>
#include <SimView/Model/Background.hpp>
#include <SimView/Model/Box.hpp>
#include <SimView/Model/MaterialObject.hpp>
#include <SimView/Model/Object.hpp>
#include <SimView/Model/PointCloud.hpp>
#include <SimView/Model/PointCloudPoly.hpp>
#include <SimView/Model/Sphere.hpp>
#include <SimView/Model/Terrain.hpp>
#include <SimView/Model/TextBox.hpp>
#include <SimView/Model/ViewerModel.hpp>
#include <SimView/OpenGL.hpp>
#include <SimView/Util/ObjectLoader.hpp>
#include <SimView/Util/StreamExecutor.hpp>
#include <SimView/Window/ImGuiSceneView.hpp>
#include <map>
#include <memory>
#include <string>
#include <thread>

namespace simview {
namespace window {

using namespace std::string_literals;

class ImGuiObjectAddPanel {
 private:
  inline static const size_t CHAR_BUFFER_SIZE = 512;
  inline static const char* FLOAT_FORMAT = "%.6f";

  inline static const std::string TITLE = "Add Object";

  std::string _objectTypes;
  model::ViewerModel_t _model;
  std::string _message = "";
  std::string _errorMessage = "";
  std::string _readableExtensions;

  std::shared_ptr<util::StreamExecutor> _streamExecutor;

 public:
  // Nothing

 private:
  void openFileDialog(char*, const nfdfilteritem_t*) const;

 public:
  ImGuiObjectAddPanel(model::ViewerModel_t);
  ~ImGuiObjectAddPanel();

  void paint();
};

using ObjectAddFileDialog_t = std::shared_ptr<ImGuiObjectAddPanel>;

}  // namespace window
}  // namespace simview