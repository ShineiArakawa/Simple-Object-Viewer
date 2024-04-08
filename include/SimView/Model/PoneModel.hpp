#pragma once

#include <SimView/Model/Background.hpp>
#include <SimView/Model/Model.hpp>
#include <SimView/Model/Object.hpp>
#include <SimView/Model/Primitives.hpp>
#include <SimView/OpenGL.hpp>
#include <array>
#include <iostream>
#include <map>
#include <memory>
#include <random>
#include <string>
#include <vector>

namespace simview {
namespace model {

class PoneModel : public Model {
 public:
  enum class GamePhase { START,
                         PLAYING,
                         GAME_OVER };
  inline static const float DELTA_T = 0.3f;
  inline static const float DELTA_Z = 0.7f;
  inline static const float BALL_RADIUS = 0.3f;
  inline static const float PADDLE_OFFSET_Y = -8.0f;
  inline static const float PADDLE_HALF_WIDTH = 3.0f / 2.0f;
  inline static const float PADDLE_HALF_HEIGHT = 0.5f / 2.0f;
  inline static const glm::vec2 RANGE_Z = glm::vec2(-9.5f, 9.5f);
  inline static const glm::vec2 RANGE_Y = glm::vec2(-9.5f, 9.5f);

 private:
  Primitive_t _sphere = nullptr;
  Primitive_t _paddle = nullptr;
  Primitive_t _backgroundStart = nullptr;
  Primitive_t _backgroundGameOver = nullptr;
  Model::Objects_t _walls = std::make_shared<std::vector<Primitive_t>>();
  GamePhase _phase = GamePhase::START;

  // clang-format off
  inline static const glm::vec3 wallNormals[4] = {
    glm::vec3(0.0f, 1.0f, 0.0f),
    glm::vec3(0.0f, 0.0f, -1.0f),
    glm::vec3(0.0f, -1.0f, 0.0f),
    glm::vec3(0.0f, 1.0f,  0.0f)
  };
  // clang-format on

 private:
  void rebound(const glm::vec3 &);

 public:
  PoneModel();
  ~PoneModel();

  void initVAO() override;
  void paintGL(
      const TransformationContext &transCtx,  // transCtx
      const GLuint &depthMapId                // depthMapId
      ) override;
  void tick(float time) override;

  void setPhase(GamePhase phase) { _phase = phase; };
  GamePhase getPhase() { return _phase; };
  void reset();
  void paddleMoveLeft();
  void paddleMoveRight();
};

using PoneModel_t = std::shared_ptr<PoneModel>;

}  // namespace model
}  // namespace simview