#pragma once

#include "../core/Camera.hpp"
#include <SDL2/SDL.h>

class Telephone {
public:
  void update(float playerX, float playerY);

  void render(SDL_Renderer *renderer, const Camera &camera) const;

  bool isNear() const;
  bool isRinging() const;
  SDL_Rect getBounds() const;

private:
  SDL_Rect bounds{650, 950, 80, 65};

  bool near = false;
  bool ringing = false;
};
