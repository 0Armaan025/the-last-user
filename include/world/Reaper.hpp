#pragma once

#include "../core/Camera.hpp"
#include <SDL2/SDL.h>

class Reaper {
public:
  void update(float playerX, float playerY, bool unlocked, float deltaTime);

  void render(SDL_Renderer *renderer, const Camera &camera) const;
  float facingAngle = 0.0f;

private:
  float x = 0.0f;
  float y = 0.0f;
  float opacity = 0.0f;

  bool active = false;

  bool visible = false;

  Uint32 vanishUntil = 0;

  int appearance = 0;

  void appearNearPlayer(float playerX, float playerY);
  // alright

  float targetX = 0.0f;
  float targetY = 0.0f;
  float moveSpeed = 100.0f;
  float waitTimer = 0.0f;

  bool moving = false;

  void chooseStalkingPosition(float playerX, float playerY);
};
