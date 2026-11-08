#include "../../include/world/BatSwarm.hpp"

#include <SDL2/SDL_render.h>
#include <algorithm>
#include <cmath>

constexpr float PI = 3.14159;

void BatSwarm::spawnWave(float playerX, float playerY) {
  bats.clear();

  const int count = 12 + wave * 2;

  for (int i = 0; i < count; ++i) {
    float angle = (2.0f * PI * i) / count;

    Bat bat;

    bat.angle = angle;

    bat.radius = 45.0f + static_cast<float>(i % 5) * 20.0f;
    bat.speed = 85.0f + static_cast<float>(i % 4) * 20.0f;

    bat.x = playerX + std::cos(angle) * 380.0f;
    bat.y = playerY + std::sin(angle) * 380.0f;

    bats.push_back(bat);
  }

  spawned = true;
}

float BatSwarm::update(float playerX, float playerY, bool unlocked,
                       float deltaTime) {
  if (!unlocked) {
    elapsed = 0.0f;
    spawned = false;
    bats.clear();
    return 0.0f;
  }

  elapsed += deltaTime;

  if (!spawned && elapsed >= 18.0f) {
    spawnWave(playerX, playerY);
  }

  if (!spawned) {
    return 0.0f;
  }

  damageCooldown = std::max(0.0f, damageCooldown - deltaTime);

  bool touchingPlayer = false;

  for (Bat &bat : bats) {
    // iw ant them to circle the player if possible
    bat.angle += deltaTime * 0.55f;

    float targetX = playerX + std::cos(bat.angle) * bat.radius;
    float targetY = playerY + std::sin(bat.angle) * bat.radius;

    float dx = targetX - bat.x;
    float dy = targetY - bat.y;

    float distance = std::sqrt(dx * dx + dy * dy);

    if (distance > 1.0f) {
      float step = bat.speed * deltaTime;
      bat.x += (dx / distance) * std::min(step, distance);
      bat.y += (dy / distance) * std::min(step, distance);
    }

    bat.flap += deltaTime * 13.0f;

    // contact detection
    //
    //

    float playerDX = playerX - bat.x;
    float playerDY = playerY - bat.y;

    float playerDistance = std::sqrt(playerDX * playerDX + playerDY * playerDY);

    if (std::sqrt(playerDX * playerDX + playerDY * playerDY) < 38.0f) {
      touchingPlayer = true;
    }
  }

  // another wave

  if (static_cast<int>(bats.size()) <= 5) {
    spawnWave(playerX, playerY);
  }

  if (touchingPlayer && damageCooldown <= 0.0f) {
    damageCooldown = 0.8f;
    return 8.0f;
  }

  return 0.0f;
}

bool BatSwarm::getNearestBat(float x, float y, float &targetX,
                             float &targetY) const {
  if (bats.empty()) {
    return false;
  }

  float nearestDistance = 900.0f * 900.0f;

  bool found = false;

  for (const Bat &bat : bats) {
    float dx = bat.x - x;
    float dy = bat.y - y;

    float distanceSquared = dx * dx + dy * dy;

    if (distanceSquared < nearestDistance) {
      nearestDistance = distanceSquared;
      targetX = bat.x;
      targetY = bat.y;
      found = true;
    }
  }

  return found;
}

// iused similar stuff inmythwood btw, don't call me a copier lol
bool BatSwarm::hitBatAt(float x, float y, float radius) {
  for (auto it = bats.begin(); it != bats.end(); ++it) {

    float dx = it->x - x;
    float dy = it->y - y;

    if (dx * dx + dy * dy <= radius * radius) {
      bats.erase(it);
      return true;
    }
  }

  return true;
}

int BatSwarm::getWave() const { return wave; }

int BatSwarm::getRemaining() const { return static_cast<int>(bats.size()); }

void BatSwarm::reset() {
  bats.clear();
  elapsed = 0.0f;
  damageCooldown = 0.0f;
  spawned = false;
  wave = 0;
}

// alright

void BatSwarm::render(SDL_Renderer *renderer, const Camera &camera) const {
  SDL_SetRenderDrawColor(renderer, 12, 10, 18, 230);
  // uh im a lil tired yaey :(
  //

  for (const Bat &bat : bats) {
    SDL_Rect screen =
        camera.apply({static_cast<int>(bat.x), static_cast<int>(bat.y), 1, 1});

    int wingOffset = static_cast<int>(std::sin(bat.flap) * 9.0f);

    // body

    SDL_Rect body{screen.x - 2, screen.y - 2, 5, 5};

    SDL_RenderFillRect(renderer, &body);

    // left wingg
    //
    // FOR THE REVIEWER: used gpt here coz it was ragebaiting me finding the
    // coords of the wings

    SDL_RenderDrawLine(renderer, screen.x, screen.y, screen.x - 11,
                       screen.y - wingOffset - 5);

    SDL_RenderDrawLine(renderer, screen.x - 11, screen.y - wingOffset - 5,
                       screen.x - 17, screen.y + 3);

    // ring wing
    //

    SDL_RenderDrawLine(renderer, screen.x, screen.y, screen.x + 11,
                       screen.y - wingOffset - 5);

    SDL_RenderDrawLine(renderer, screen.x + 11, screen.y - wingOffset - 5,
                       screen.x + 17, screen.y + 3);
  }
}
