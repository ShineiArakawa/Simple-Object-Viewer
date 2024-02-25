#include <pybind11/pybind11.h>

#include <App/ViewerGUIMain.hpp>
#include <App/ViewerMain.hpp>

PYBIND11_MODULE(ViewerPythonBinding, module) {
  module.doc() = "Python binding for the simple object viewer.";
  pybind11::class_<ViewerApp>(module, "ViewerApp")
      .def(pybind11::init<std::string>())
      .def("launch", &ViewerApp::launch, "Launch viewer window");
  pybind11::class_<ViewerGUIApp>(module, "ViewerGUIApp")
      .def(pybind11::init<>())
      .def("launch", &ViewerGUIApp::launch, "Launch viewer window");
}