#pragma once

#include "../core/Camera.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <vector>

class BatSwarm {
public:
  float update(float playerX, float playerY, bool unlocked, float deltaTime);

  void render(SDL_Renderer *renderer, const Camera &camera) const;

private:
  struct Bat {

    float x = 0.0f;
    float y = 0.0f;
    float angle = 0.0f;
    float radius = 70.0f;
    float speed = 100.0f;
    float flap = 0.0f;
  };

  std::vector<Bat> bats;

  float elapsed = 0.0f;
  float damageCooldown = 0.0f;

  bool spawned = false;

  void spawn(float playerX, float playerY);
};
