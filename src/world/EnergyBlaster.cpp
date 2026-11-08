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

  float dx = targetX - playerX;
  float dy = targetY - playerY;

  float distance = std::sqrt(dx * dx + dy * dy);

  if (distance <= 0.001f) {
    return;
  }

  constexpr float PULSE_SPEED = 520.0f;

  Pulse pulse;

  pulse.x = playerX;
  pulse.y = playerY;

  pulse.vx = (dx / distance) * PULSE_SPEED;
  pulse.vy = (dy / distance) * PULSE_SPEED;

  pulse.life = 1.5f;

  pulses.push_back(pulse);
}

void EnergyBlaster::update(float playerX, float playerY, bool firing,
                           float deltaTime, BatSwarm &swarm) {
  if (!collected) {
    float dx = playerX - (pickupBounds.x + pickupBounds.w / 2.0f);
    float dy = playerY - (pickupBounds.y + pickupBounds.h / 2.0f);

    if (dx * dx + dy * dy < 55.0f * 55.0f) {
      collected = true;
    }

    fireCooldown = std::max(0.0f, fireCooldown - deltaTime);

    if (collected && firing && fireCooldown <= 0.0f) {
      fire(playerX, playerY, swarm);
      fireCooldown = 0.35f;
    }

    for (auto it = pulses.begin(); it != pulses.end();) {
      it->x += it->vx * deltaTime;
      it->y += it->vy * deltaTime;
      it->life -= deltaTime;

      bool hit = swarm.hitBatAt(it->x, it->y, 20.0f);

      if (hit || it->life <= 0.0f) {
        it = pulses.erase(it);
      } else {
        ++it;
      }
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
    SDL_Rect screen = camera.apply(
        {static_cast<int>(pulse.x) - 5, static_cast<int>(pulse.y) - 5, 10, 10});

    SDL_SetRenderDrawColor(renderer, 40, 190, 255, 100);
    SDL_Rect glow{screen.x - 5, screen.y - 5, screen.w + 10, screen.h + 10};
    SDL_RenderFillRect(renderer, &glow);
    SDL_SetRenderDrawColor(renderer, 130, 235, 255, 255);
    SDL_RenderFillRect(renderer, &screen);
  }
}

bool EnergyBlaster::isCollected() const { return collected; }

void EnergyBlaster::reset() {
  collected = false;
  fireCooldown = 0.0f;
  pulses.clear();
}
