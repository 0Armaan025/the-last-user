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
    return;
  }

  Uint32 now = SDL_GetTicks();

  // first apperance

  if (!active) {
    active = true;
    appearNearPlayer(playerX, playerY);
  }

  // dis b/w player and reaper

  float dx = playerX - x;
  float dy = playerY - y;

  float distance = std::sqrt(dx * dx + dy * dy);

  if (visible && distance < 190.0f) {
    visible = false;
    opacity = 0.0f;
    vanishUntil = now + 2500;
  }

  // reappear somewhere else now

  if (!visible && now >= vanishUntil) {
    appearNearPlayer(playerX, playerY);
  }

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
