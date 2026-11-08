// ALRIGHT LISTEN FUTURE ARMAAN, YOU CAN TAKE INSPIRATINO FROM HERE COZ THE
// REAPER IM GONNA CREATE IS GONNA BE SUPER SUPER SUPER DETAILED OKIE!?

#include "../../include/world/Reaper.hpp"
#include <SDL2/SDL_blendmode.h>
#include <SDL2/SDL_render.h>
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

  // alirght

  animationTime += deltaTime;

  // slwo floating

  floatOffset = std::sin(animationTime * 1.8f) * 7.0f;

  // side to side movement

  cloakSway = std::sin(animationTime * 1.3f) * 5.0f;

  breathing = std::sin(animationTime * 2.0f) * 1.5f;
  //  first apperance

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

  renderScale = std::clamp(distance / 650.0f, 0.55f, 1.0f);

  afterImageAlpha = std::max(0.0f, afterImageAlpha - 90.0f * deltaTime);

  edgeCooldown -= deltaTime;

  if (visible && edgeCooldown <= 0.0f) {
    edgeCooldown = 0.8f;
    edgeCooldown = 8.0f;
  }

  edgeDuration = std::max(0.0f, edgeDuration - deltaTime);

  if (visible && distance < 190.0f) {
    afterImageX = x;
    afterImageY = y;
    afterImageAlpha = 110.0f;
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

  // reaper stalks now, let's go
  // tiem to fix the reaper , let's do it

  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

  // fading shadow afterimage

  if (afterImageAlpha > 0.0f) {
    SDL_Rect shadow =
        camera.apply({static_cast<int>(afterImageX - 35),
                      static_cast<int>(afterImageY + 80), 70, 12});

    SDL_SetRenderDrawColor(renderer, 5, 5, 8,
                           static_cast<Uint8>(afterImageAlpha));

    SDL_RenderFillRect(renderer, &shadow);
  }
  if (!visible || opacity <= 0.0f) {
    return;
  }

  Uint8 alpha = static_cast<Uint8>(opacity);

  // distance based silhouette let's go

  auto scaledRect = [&](float offsetX, float offsetY, float width,
                        float height) {
    float sway = 0.0f;

    if (offsetY > 20.0f) {
      sway = cloakSway * (offsetY / 90.0f);
    }

    float adjustedY = offsetY + floatOffset;

    float adjustedWidth = width;
    float adjustedHeight = height;

    if (offsetY < 20.0f) {
      adjustedWidth += breathing;

      adjustedHeight += breathing;
    }
    SDL_Rect worldRect{static_cast<int>(x + (offsetX + sway) * renderScale),
                       static_cast<int>(y + adjustedY * renderScale),
                       static_cast<int>(adjustedWidth * renderScale),
                       static_cast<int>(adjustedHeight * renderScale)};

    return camera.apply(worldRect);
  };

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

  // alirght
  SDL_Rect hood = camera.apply(
      {static_cast<int>(x - 17), static_cast<int>(y - 10), 34, 42});

  SDL_SetRenderDrawColor(renderer, 5, 5, 8, alpha);
  SDL_RenderFillRect(renderer, &hood);

  SDL_Rect aura = scaledRect(-23, -17, 46, 52);
  SDL_SetRenderDrawColor(renderer, 35, 8, 12, static_cast<Uint8>(alpha / 9));

  SDL_RenderDrawRect(renderer, &aura);

  // now we need enemies
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

  SDL_SetRenderDrawColor(renderer, 3, 4, 7, alpha);

  SDL_Rect raggedEdges[] = {
      scaledRect(-34, 87, 9, 12), scaledRect(-18, 91, 8, 10),
      scaledRect(8, 90, 9, 13), scaledRect(25, 87, 8, 11)};

  for (const SDL_Rect &edge : raggedEdges) {
    SDL_RenderFillRect(renderer, &edge);
  }

  // ALR NOW WE NEED SOME RANDOM OCCASSIONAL SIGHTINGS (screen eddge ones)

  if (edgeDuration > 0.0f) {
    int screenWidth;
    int screenHeight;

    SDL_GetRendererOutputSize(renderer, &screenWidth, &screenHeight);

    SDL_Rect worldPosition = camera.apply({
        static_cast<int>(x),
        static_cast<int>(y),
        1,
        1,
    });

    bool outside = worldPosition.x < 0 || worldPosition.x >= screenWidth ||
                   worldPosition.y < 0 || worldPosition.y >= screenHeight;

    if (outside) {
      int edgeX = std::clamp(worldPosition.x, 24, screenWidth - 24);

      int edgeY = std::clamp(worldPosition.y, 24, screenHeight - 24);

      Uint8 edgeAlpha =
          static_cast<Uint8>(150.0f * std::min(1.0f, edgeDuration / 0.8f));

      SDL_Rect edgeHood{edgeX - 8, edgeY - 12, 16, 20};

      SDL_Rect edgeCloak{edgeX - 13, edgeY + 8, 26, 24};

      SDL_SetRenderDrawColor(renderer, 5, 5, 8, edgeAlpha);

      SDL_RenderFillRect(renderer, &edgeCloak);
      SDL_RenderFillRect(renderer, &edgeHood);

      SDL_Rect edgeEyes{edgeX - 4, edgeY - 3, 8, 2};

      SDL_SetRenderDrawColor(renderer, 145, 35, 35, edgeAlpha);
      SDL_RenderFillRect(renderer, &edgeEyes);
    }
  }
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
  // our reaper sucks
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
