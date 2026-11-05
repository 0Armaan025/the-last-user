#pragma once
#include <SDL2/SDL.h>

class Camera {
public:
  Camera(int screenWidth, int screenHeight);
  void follow(float targetX, float targetY);

  SDL_Rect apply(const SDL_Rect &worldObject) const;

private:
  float x = 0.0f;
  float y = 0.0f;

  int screenWidth;
  int screenHeight;
};
