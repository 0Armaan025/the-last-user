#pragma once

#include "../core/Camera.hpp"
#include "BatSwarm.hpp"

#include <SDL2/SDL.h>
#include <vector>

// i want a laer kinda waeapon shooting lasers
class EnergyBlaster {

public:
  void update(float playerX, float playerY, bool firing, float deltaTime,
              BatSwarm &swarm);

  void render(SDL_Renderer *renderer, const Camera &camera) const;

  bool isCollected() const;
  void reset();

private:
  struct Pulse {
    float x;
    float y;
    float vx;
    float vy;
    float life;
  };

  // iw ant ti to be at the top right
  SDL_Rect pickupBounds{2260, 100, 42, 24};

  bool collected = false;
  float fireCooldown = 0.0f;

  std::vector<Pulse> pulses;

  void fire(float playerX, float playerY, const BatSwarm &swarm);
};
