#pragma once

#include "../core/Camera.cpp"
#include <SDL2/SDL.h>
#include <vector>

class BatSwarm {
public:
  float update(float playerX, float playerY, bool unlocked, float deltaTime);
};
