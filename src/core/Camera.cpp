#include "../../include/core/Camera.hpp"

#include <cmath>

Camera::Camera(int screenWidth, int screenHeight)
    : screenWidth(screenWidth), screenHeight(screenHeight) {};

void Camera::follow(float targetX, float targetY) {
  x = targetX - static_cast<float>(screenWidth) / 2.0f;
  y = targetY - static_cast<float>(screenHeight) / 2.0f;
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
