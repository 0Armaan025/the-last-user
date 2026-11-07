#pragma once

#include "../core/Camera.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

class Telephone {
public:
  bool loadRingSound(const char *path);
  void update(float playerX, float playerY);

  void render(SDL_Renderer *renderer, const Camera &camera) const;

  bool isNear() const;
  bool isRinging() const;
  SDL_Rect getBounds() const;
  void cleanup();

private:
  SDL_Rect bounds{650, 950, 80, 65};

  bool near = false;
  bool ringing = false;

  Mix_Chunk *ringSound = nullptr;
  int ringChannel = -1;
};
