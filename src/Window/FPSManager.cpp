#include <SimView/Window/FPSManager.hpp>

namespace simview {
namespace window {

FPSManager::FPSManager()
    : _fps(DEFAULT_FPS),
      _invFps(1.0 / _fps),
      _previousTime(glfwGetTime()) {
}

FPSManager::~FPSManager() {
}

double FPSManager::getFPS() const {
  return _fps;
}

void FPSManager::setFPS(const double& fps) {
  _fps = fps;
  _invFps = 1.0 / _fps;
}

bool FPSManager::update() {
  bool toUpdate = false;
  const double currentTime = glfwGetTime();

  if ((currentTime - _previousTime) > _invFps) {
    toUpdate = true;
    _previousTime = currentTime;
  }

  return toUpdate;
}

}  // namespace window
}  // namespace simview