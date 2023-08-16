#pragma once
#ifndef _PONE_MODEL_H_
#define _PONE_MODEL_H_

#include <Model/Background.hpp>
#include <Model/Model.hpp>
#include <Model/Object.hpp>
#include <Model/Primitives.hpp>
#include <OpenGL.hpp>
#include <array>
#include <iostream>
#include <map>
#include <memory>
#include <random>
#include <string>
#include <vector>

class PoneModel : public Model {
 public:
  enum class GamePhase { START, PLAYING, GAME_OVER };
  inline static const float DELTA_T = 0.3f;
  inline static const float DELTA_Z = 0.7f;
  inline static const float BALL_RADIUS = 0.3f;
  inline static const float PADDLE_OFFSET_Y = -8.0f;
  inline static const float PADDLE_HALF_WIDTH = 3.0f / 2.0f;
  inline static const float PADDLE_HALF_HEIGHT = 0.5f / 2.0f;
  inline static const glm::vec2 RANGE_Z = glm::vec2(-9.5f, 9.5f);
  inline static const glm::vec2 RANGE_Y = glm::vec2(-9.5f, 9.5f);

 private:
  Model::t_object _sphere = nullptr;
  Model::t_object _paddle = nullptr;
  Model::t_object _backgroundStart = nullptr;
  Model::t_object _backgroundGameOver = nullptr;
  Model::t_objects _walls = std::make_shared<std::vector<Model::t_object>>();
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
  void rebound(const glm::vec3&);

 public:
  PoneModel();
  ~PoneModel();

  void initVAO() override;
  void paintGL(const glm::mat4& mvpMat) override;
  void tick(float time) override;

  void setPhase(GamePhase phase) { _phase = phase; };
  GamePhase getPhase() { return _phase; };
  void reset();
  void paddleMoveLeft();
  void paddleMoveRight();
};
#endif