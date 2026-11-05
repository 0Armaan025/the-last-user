#include "../../include/core/Camera.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>

Camera::Camera(int screenWidth, int screenHeight)
    : screenWidth(screenWidth), screenHeight(screenHeight) {};

void Camera::follow(float targetX, float targetY, float deltaTime) {

  float targetCameraX = targetX - static_cast<float>(screenWidth) / 2.0f;
  float targetCameraY = targetY - static_cast<float>(screenHeight) / 2.0f;

  // smooth camera interpolation

  float interpolation = 1.0f - std::exp(-smoothing * deltaTime);

  x += (targetCameraX - x) * interpolation;
  y += (targetCameraY - y) * interpolation;

  x = std::clamp(x, 0.0f, static_cast<float>(worldWidth - screenWidth));

  y = std::clamp(y, 0.0f, static_cast<float>(worldHeight - screenHeight));
}

SDL_Rect Camera::apply(const SDL_Rect &worldObject) const {
  SDL_Rect screenObject = worldObject;
  // lround converts to the nearest integert
  screenObject.x =
      static_cast<int>(std::lround(static_cast<float>(worldObject.x) - x));

  screenObject.y =
      static_cast<int>(std::lround(static_cast<float>(worldObject.y) - y));

  return screenObject;
}
