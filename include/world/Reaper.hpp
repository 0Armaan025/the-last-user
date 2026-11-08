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

  // to stop appearing from one pos let's do this ig

  // last pos: leaves a fading shadow ig
  float afterImageX = 0.0f;

  float afterImageY = 0.0f;
  // man this really reminds me of shadow from eminence in shadow anime for
  // afterimages LOL

  float afterImageAlpha = 0.0f;

  float renderScale = 1.0f;

  float edgeCooldown = 7.0f;
  float edgeDuration = 0.0f;

  // alr let's make reaper smooth

  float animationTime = 0.0f;
  float floatOffset = 0.0f;

  float cloakSway = 0.0f;
  float breathing = 0.0f;
};
