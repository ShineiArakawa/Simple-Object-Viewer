#include <SimView/Model/PoneModel.hpp>

namespace simview {
namespace model {

using namespace util;
using namespace shader;

PoneModel::PoneModel(/* args */)
    : Model() {
}

PoneModel::~PoneModel() {}

void PoneModel::initVAO() {
  int nBackgrounds = getNumBackgrounds();
  for (int iBackground = 0; iBackground < nBackgrounds; iBackground++) {
    Background_t background = getBackground(iBackground);

    if (background->getName() == "Start") {
      _backgroundStart = background;
    } else if (background->getName() == "GameOver") {
      _backgroundGameOver = background;
    }
  }

  int nModels = getNumObjects();
  for (int iModel = 0; iModel < nModels; iModel++) {
    Primitive_t model = getObject(iModel);

    if (model->getName() == "Sphere") {
      _sphere = model;
    } else if (model->getName() == "Paddle") {
      _paddle = model;
    } else {
      _walls->push_back(std::move(model));
    }
  }

  reset();
}

void PoneModel::paintGL(
    const TransformationContext &transCtx,  // transCtx
    const GLuint &depthMapId                // depthMapId
) {
  const glm::vec3 &lightPosition = _lightPosition.xyz();

  const model::LightingContext lightingCtx(
      lightPosition,     // lightPos
      _shininess,        // shininess
      _ambientIntensity  // ambientIntensity
  );

  const model::RenderingContext renderingCtx(
      _wireFrameColor,  // wireFrameColor
      _wireFrameWidth,  // wireFrameWidth
      depthMapId        // depthTextureId
  );

  if (_phase == GamePhase::START) {
    _backgroundStart->paintGL(transCtx, lightingCtx, renderingCtx);
  } else if (_phase == GamePhase::PLAYING) {
    for (int iWall = 0; iWall < (int)_walls->size(); iWall++) {
      (*_walls)[iWall]->update();
      (*_walls)[iWall]->paintGL(transCtx, lightingCtx, renderingCtx);
    }

    _sphere->update();
    _sphere->paintGL(transCtx, lightingCtx, renderingCtx);

    _paddle->update();
    _paddle->paintGL(transCtx, lightingCtx, renderingCtx);

  } else if (_phase == GamePhase::GAME_OVER) {
    _backgroundGameOver->paintGL(transCtx, lightingCtx, renderingCtx);
  }
}

void PoneModel::tick(float time) {
  _time += time;

  if (_phase == GamePhase::PLAYING) {
    _sphere->forward(DELTA_T);
  }

  glm::vec3 centerCoord = _sphere->getPosition();
  glm::vec3 paddleCenterCoord = _paddle->getPosition();
  const float paddleMinZ = paddleCenterCoord.z - PADDLE_HALF_WIDTH;
  const float paddleMaxZ = paddleCenterCoord.z + PADDLE_HALF_WIDTH;
  const float paddleMinY = PADDLE_OFFSET_Y + paddleCenterCoord.y - PADDLE_HALF_HEIGHT;
  const float paddleMaxY = PADDLE_OFFSET_Y + paddleCenterCoord.y + PADDLE_HALF_HEIGHT;
  const float ballMinZ = centerCoord.z - BALL_RADIUS;
  const float ballMaxZ = centerCoord.z + BALL_RADIUS;
  const float ballMinY = centerCoord.y - BALL_RADIUS;
  const float ballMaxY = centerCoord.y + BALL_RADIUS;

  if (centerCoord.z - BALL_RADIUS < RANGE_Z.x) {
    // Right Wall
    rebound(wallNormals[0]);
  } else if (ballMaxY > RANGE_Y.y) {
    // Upper Wall
    rebound(wallNormals[1]);
  } else if (ballMaxZ > RANGE_Z.y) {
    // Left Wall
    rebound(wallNormals[2]);
  } else if (ballMinY < RANGE_Y.x) {
    // Bottom Wall
    _phase = GamePhase::GAME_OVER;
  } else if (paddleMinZ < centerCoord.z && centerCoord.z < paddleMaxZ && ballMinY < PADDLE_OFFSET_Y) {
    // Paddle Upper
    rebound(wallNormals[1]);
  } else if (paddleMinY < centerCoord.y && centerCoord.y < paddleMaxY && ballMinZ < paddleMaxZ && ballMaxZ > paddleMaxZ) {
    // Paddle Left
    rebound(wallNormals[2]);
  } else if (paddleMinY < centerCoord.y && centerCoord.y < paddleMaxY && ballMaxZ > paddleMinZ && ballMinZ < paddleMinZ) {
    // Paddle Right
    rebound(wallNormals[0]);
  }
}

void PoneModel::reset() {
  {
    std::random_device rnd;
    std::mt19937_64 mt64(rnd());
    std::uniform_real_distribution<double> uniform(0.0, 1.0);

    glm::vec3 initPosition(0.0f);
    glm::vec3 initVelocity(0.0f);
    for (int i = 0; i < 2; i++) {
      initVelocity[i + 1] = (float)uniform(mt64);
    }
    _sphere->setPosition(initPosition);
    _sphere->setVecocity(glm::normalize(initVelocity));
  }

  {
    glm::vec3 initPosition(0.0f);
    _paddle->setPosition(initPosition);
  }
}

void PoneModel::rebound(const glm::vec3 &normal) {
  glm::vec3 velocity = _sphere->getVecocity();
  const float lenNormal = glm::length(normal);
  const float len1 = glm::dot(velocity, normal) / lenNormal;

  velocity.x = -velocity.x + 2.0f * len1 * normal.x / lenNormal;
  velocity.y = -velocity.y + 2.0f * len1 * normal.y / lenNormal;
  velocity.z = -velocity.z + 2.0f * len1 * normal.z / lenNormal;

  _sphere->setVecocity(velocity);
}

void PoneModel::paddleMoveLeft() {
  if (_phase == GamePhase::PLAYING) {
    auto paddleCenterCoord = _paddle->getPosition();
    paddleCenterCoord.z += DELTA_Z;

    if (paddleCenterCoord.z + PADDLE_HALF_WIDTH < RANGE_Z.y) {
      _paddle->setPosition(paddleCenterCoord);
    }
  }
}

void PoneModel::paddleMoveRight() {
  if (_phase == GamePhase::PLAYING) {
    auto paddleCenterCoord = _paddle->getPosition();
    paddleCenterCoord.z -= DELTA_Z;

    if (paddleCenterCoord.z - PADDLE_HALF_WIDTH > RANGE_Z.x) {
      _paddle->setPosition(paddleCenterCoord);
    }
  }
}

}  // namespace model
}  // namespace simview