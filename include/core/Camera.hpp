#pragma once
#include <SDL2/SDL.h>

class Camera {
public:
  Camera(int screenWidth, int screenHeight);
  void follow(float targetX, float targetY, float deltaTime);
  // now let's try adding a smoothing factor

  void snapTo(float targetX, float targetY);

  SDL_Rect apply(const SDL_Rect &worldObject) const;

private:
  float x = 0.0f;
  float y = 0.0f;

  int screenWidth;

  float smoothing = 5.0f;
  int screenHeight;

  int worldWidth = 2400;
  int worldHeight = 1600;
};
