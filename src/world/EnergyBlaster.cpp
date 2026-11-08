#include "../../include/world/EnergyBlaster.hpp"

#include <SDL2/SDL_render.h>
#include <algorithm>
#include <array>
#include <cmath>

void EnergyBlaster::fire(float playerX, float playerY, const BatSwarm &swarm) {
  float targetX = 0.0f;
  float targetY = 0.0f;

  if (!swarm.getNearestBat(playerX, playerY, targetX, targetY)) {
    return;
  }

  const float dx = targetX - playerX;
  const float dy = targetY - playerY;
  const float distance = std::sqrt(dx * dx + dy * dy);

  if (distance <= 0.001f) {
    return;
  }

  constexpr float PULSE_SPEED = 850.0f;
  pulses.push_back({playerX, playerY, (dx / distance) * PULSE_SPEED,
                    (dy / distance) * PULSE_SPEED, 1.2f});
}

void EnergyBlaster::update(float playerX, float playerY, bool firing,
                           float deltaTime, BatSwarm &swarm) {

  // Pickup detection
  if (!collected) {
    const float pickupX = pickupBounds.x + pickupBounds.w / 2.0f;

    const float pickupY = pickupBounds.y + pickupBounds.h / 2.0f;

    const float dx = playerX - pickupX;
    const float dy = playerY - pickupY;

    if (dx * dx + dy * dy < 55.0f * 55.0f) {
      collected = true;
    }
  }

  // No firing before collecting the gun
  if (!collected) {
    return;
  }

  // Firing cooldown
  fireCooldown = std::max(0.0f, fireCooldown - deltaTime);

  if (firing && fireCooldown <= 0.0f) {
    fire(playerX, playerY, swarm);
    fireCooldown = 0.18f;
  }

  // Update existing energy pulses
  for (auto it = pulses.begin(); it != pulses.end();) {
    it->x += it->vx * deltaTime;
    it->y += it->vy * deltaTime;
    it->life -= deltaTime;

    const bool hit = swarm.hitBatAt(it->x, it->y, 24.0f);

    if (hit || it->life <= 0.0f) {
      it = pulses.erase(it);
    } else {
      ++it;
    }
  }
}
void EnergyBlaster::render(SDL_Renderer *renderer, const Camera &camera) const {
  if (!collected) {
    SDL_Rect pickup = camera.apply(pickupBounds);

    SDL_SetRenderDrawColor(renderer, 30, 80, 100, 255);
    SDL_RenderFillRect(renderer, &pickup);

    SDL_SetRenderDrawColor(renderer, 80, 220, 255, 255);
    SDL_RenderDrawRect(renderer, &pickup);

    SDL_Rect core{pickup.x + 8, pickup.y + 7, pickup.w - 16, pickup.h - 14};

    SDL_RenderFillRect(renderer, &core);
  }

  for (const Pulse &pulse : pulses) {
    const int x = static_cast<int>(pulse.x);
    const int y = static_cast<int>(pulse.y);

    const int tailX = static_cast<int>(pulse.x - pulse.vx * 0.035f);

    const int tailY = static_cast<int>(pulse.y - pulse.vy * 0.035f);

    SDL_Rect current = camera.apply({x - 4, y - 4, 8, 8});
    SDL_Rect tail = camera.apply({tailX, tailY, 1, 1});

    // outer energy glow

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 20, 150, 255, 65);

    SDL_Rect glow{current.x - 9, current.y - 9, current.w + 18, current.h + 18};

    SDL_RenderFillRect(renderer, &glow);

    // nergy tail (trail)

    SDL_SetRenderDrawColor(renderer, 40, 190, 255, 180);
    SDL_RenderDrawLine(renderer, current.x + 4, current.y + 4, tail.x, tail.y);

    // bright laser core

    SDL_SetRenderDrawColor(renderer, 170, 245, 255, 255);
    SDL_RenderFillRect(renderer, &current);

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
  }
}

bool EnergyBlaster::isCollected() const { return collected; }

void EnergyBlaster::reset() {
  collected = false;
  fireCooldown = 0.0f;
  pulses.clear();
}
