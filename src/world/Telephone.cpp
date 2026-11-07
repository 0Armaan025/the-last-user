#include "../../include/world/Telephone.hpp";
#include <SDL2/SDL_mixer.h>
#include <iostream>

#include <cmath>

bool Telephone::loadRingSound(const char *path) {
  ringSound = Mix_LoadWAV(path);

  if (!ringSound) {
    std::cout << "Telephone audo loadign failed: " << Mix_GetError() << "\n";

    return false;
  }

  return true;
}

void Telephone::cleanup() {
  if (ringChannel != -1) {
    Mix_HaltChannel(ringChannel);
    ringChannel = -1;
  }

  if (ringSound) {
    Mix_FreeChunk(ringSound);
    ringSound = nullptr;
  }
}

void Telephone::update(float playerX, float playerY) {
  float centerX = bounds.x + bounds.w / 2.0f;

  float centerY = bounds.y + bounds.h / 2.0f;

  float dx = playerX - centerX;
  float dy = playerY - centerY;

  float distance = std::sqrt(dx * dx + dy * dy);

  near = distance <= 110.0f;

  // rings when approaching ig

  ringing = distance <= 260.0f && !near;

  // start ringing

  if (ringing && ringSound && ringChannel == -1) {
    ringChannel = Mix_PlayChannel(-1, ringSound, -1);
    if (ringChannel == -1) {
      std::cout << "Telephone playback failed: " << Mix_GetError() << "\n";
    }
  }

  if (!ringing && ringChannel != -1) {
    Mix_HaltChannel(ringChannel);
    ringChannel = -1;
  }
}

void Telephone::render(SDL_Renderer *renderer, const Camera &camera) const {

  // base

  SDL_Rect base = camera.apply(bounds);
  SDL_SetRenderDrawColor(renderer, 45, 35, 30, 255);
  SDL_RenderFillRect(renderer, &base);

  SDL_Rect body = camera.apply({bounds.x + 8, bounds.y + 10, 64, 45});

  SDL_SetRenderDrawColor(renderer, 85, 65, 48, 255);
  SDL_RenderFillRect(renderer, &body);

  // dial

  SDL_Rect dial = camera.apply({bounds.x + 24, bounds.y + 20, 32, 27});

  SDL_SetRenderDrawColor(renderer, 160, 145, 115, 255);
  SDL_RenderFillRect(renderer, &dial);

  SDL_SetRenderDrawColor(renderer, 45, 35, 30, 255);
  SDL_RenderDrawRect(renderer, &dial);

  // dial holes
  //

  for (int i = 0; i < 6; ++i) {
    SDL_Rect hole = camera.apply(
        {bounds.x + 28 + (i % 3) * 9, bounds.y + 23 + (i / 3) * 9, 4, 4});

    SDL_SetRenderDrawColor(renderer, 45, 35, 30, 255);
    SDL_RenderFillRect(renderer, &hole);
  }

  // handset

  SDL_Rect handset = camera.apply({bounds.x + 5, bounds.y + 3, 70, 9});

  SDL_SetRenderDrawColor(renderer, 30, 25, 23, 255);
  SDL_RenderFillRect(renderer, &handset);

  // small handset ends

  SDL_Rect leftEnd = camera.apply({bounds.x + 3, bounds.y, 13, 15});

  SDL_Rect rightEnd = camera.apply({bounds.x + 64, bounds.y, 13, 15});

  SDL_RenderFillRect(renderer, &leftEnd);
  SDL_RenderFillRect(renderer, &rightEnd);

  // a lil ringing visual

  if (ringing) {
    Uint32 time = SDL_GetTicks();

    if ((time / 350) % 2 == 0) {
      SDL_Rect indicator = camera.apply({bounds.x + 35, bounds.y - 15, 10, 7});

      SDL_SetRenderDrawColor(renderer, 190, 55, 45, 255);

      SDL_RenderFillRect(renderer, &indicator);
    }
  }
}
// alr

bool Telephone::isNear() const { return near; }

bool Telephone::isRinging() const { return ringing; }

SDL_Rect Telephone::getBounds() const { return bounds; }
