#pragma once

#include <SimView/OpenGL.hpp>
#include <SimView/Util/Logging.hpp>

namespace simview {
namespace window {

class FPSManager {
 private:
  inline static const double DEFAULT_FPS = 240.0;

  double _previousTime;
  double _fps;
  double _invFps;

 public:
  FPSManager();
  ~FPSManager();

  double getFPS() const;
  void setFPS(const double& fps);
  bool update();
};

}  // namespace window
}  // namespace simview