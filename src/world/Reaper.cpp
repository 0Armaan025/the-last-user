// ALRIGHT LISTEN FUTURE ARMAAN, YOU CAN TAKE INSPIRATINO FROM HERE COZ THE
// REAPER IM GONNA CREATE IS GONNA BE SUPER SUPER SUPER DETAILED OKIE!?

#include "../../include/world/Reaper.hpp"
#include <SDL2/SDL_blendmode.h>
#include <algorithm>
#include <cmath>

void Reaper::appearNearPlayer(float playerX, float playerY) {
  // diff pos around the player should work

  const SDL_Point offsets[] = {
      {360, -150}, {-360, -120}, {300, 180}, {-340, 160}, {0, -350}};

  const SDL_Point offset = offsets[appearance % 5];

  x = std::clamp(playerX + static_cast<float>(offset.x), 80.0f, 2320.0f);
  y = std::clamp(playerY + static_cast<float>(offset.y), 80.0f, 1520.0f);

  ++appearance;

  visible = true;
}

void Reaper::update(float playerX, float playerY, bool unlocked,
                    float deltaTime) {
  if (!unlocked) {
    active = false;

    visible = false;
    opacity = 0.0f;
    moving = false;
    return;
  }

  Uint32 now = SDL_GetTicks();

  // first apperance

  if (!active) {
    active = true;
    x = playerX + 650.0f;
    y = playerY;
    chooseStalkingPosition(playerX, playerY);
    visible = false;
    vanishUntil = now + 1500;
  }

  // dis b/w player and reaper

  float dx = playerX - x;
  float dy = playerY - y;

  facingAngle = std::atan2(dy, dx);
  float distance = std::sqrt(dx * dx + dy * dy);

  if (visible && distance < 190.0f) {
    visible = false;
    opacity = 0.0f;
    waitTimer = 1.5f;
    moving = false;
    vanishUntil = now + 2500;
  }

  // now move when hidden

  if (!visible && moving) {
    float moveX = targetX - x;
    float moveY = targetY - y;

    float targetDistance = std::sqrt(moveX * moveX + moveY * moveY);
    if (targetDistance > 5.0f) {
      float step = moveSpeed * deltaTime;

      x += (moveX / targetDistance) * std::min(step, targetDistance);
      y += (moveY / targetDistance) * std::min(step, targetDistance);
    } else {
      moving = false;
      waitTimer = 1.5f;
    }
  }

  // reappear somewhere else now

  if (!visible && !moving && now >= vanishUntil) {
    if (waitTimer > 0.0f) {
      waitTimer -= deltaTime;
    } else {
      visible = true;
    }
  }
  // face player

  dx = playerX - x;
  dy = playerY - y;

  facingAngle = std::atan2(dy, dx);
  // smooth fading

  if (visible) {
    opacity = std::min(180.0f, opacity + 120.0f * deltaTime);
  } else {
    opacity = std::max(0.0f, opacity - 180.0f * deltaTime);
  }
}

void Reaper::render(SDL_Renderer *renderer, const Camera &camera) const {
  if (!visible || opacity <= 0.0f) {
    return;
  }

  Uint8 alpha = static_cast<Uint8>(opacity);

  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

  // now i want a good looking cloak for my reaper

  const SDL_Rect cloakParts[] = {
      {static_cast<int>(x - 9), static_cast<int>(y + 25), 18, 16},
      {static_cast<int>(x - 15), static_cast<int>(y + 41), 30, 16},
      {static_cast<int>(x - 23), static_cast<int>(y + 57), 46, 16},
      {static_cast<int>(x - 30), static_cast<int>(y + 73), 60, 20},
  };

  SDL_SetRenderDrawColor(renderer, 8, 9, 12, alpha);

  for (const SDL_Rect &part : cloakParts) {
    SDL_Rect screenPart = camera.apply(part);
    SDL_RenderFillRect(renderer, &screenPart);
  }

  // hood

  SDL_Rect hood = camera.apply(
      {static_cast<int>(x - 17), static_cast<int>(y - 10), 34, 42});

  SDL_SetRenderDrawColor(renderer, 5, 5, 8, alpha);
  SDL_RenderFillRect(renderer, &hood);

  SDL_Rect face =
      camera.apply({static_cast<int>(x - 10), static_cast<int>(y + 3), 20, 23});

  SDL_SetRenderDrawColor(renderer, 1, 2, 4, alpha);
  SDL_RenderFillRect(renderer, &face);

  // eyes

  Uint8 eyeAlpha = static_cast<Uint8>(alpha / 2);

  SDL_Rect leftEye =
      camera.apply({static_cast<int>(x - 7), static_cast<int>(y + 13), 4, 2});

  SDL_Rect rightEye =
      camera.apply({static_cast<int>(x + 3), static_cast<int>(y + 13), 4, 2});

  SDL_SetRenderDrawColor(renderer, 145, 35, 35, eyeAlpha);

  SDL_RenderFillRect(renderer, &leftEye);
  SDL_RenderFillRect(renderer, &rightEye);

  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

void Reaper::chooseStalkingPosition(float playerX, float playerY) {
  const float angles[] = {

      0.0f, 0.8f, 1.6f, 2.4f, 3.2f, 4.0f, 4.8f, 5.6f,
  };

  const int index = appearance % 8;
  const float angle = angles[index];
  const float distance = 650.0f;

  targetX = std::clamp(playerX + std::cos(angle) * distance, 80.0f, 2320.0f);

  targetY = std::clamp(playerY + std::sin(angle) * distance, 80.0f, 1520.0f);

  ++appearance;

  moving = true;
}
