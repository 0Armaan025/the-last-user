#include "../../include/core/Player.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_render.h>
#include <algorithm>
#include <cmath>
#include <iostream>

Player::Player() = default;

void Player::draw(SDL_Renderer *renderer) {
  SDL_SetRenderDrawColor(renderer, 180, 100, 50, 255);

  SDL_RenderFillRect(renderer, &bounds);
}

void Player::update(float deltaTime, const SDL_Rect &obstacle) {
  // this logic is messed up for collision, so gonna rewrite it
  float magnitude =
      std::sqrt((velocityX * velocityX) + (velocityY * velocityY));
  // doing this to prevent diagonal movement faster :((
  //
  //

  float moveX = velocityX;
  float moveY = velocityY;

  if (magnitude > 0.0f) {
    moveX = moveX / magnitude;
    moveY = moveY / magnitude;
    // rn im gonna normalize THESE HEHE AGAIN
  }

  float dx = moveX * speed * deltaTime;
  float dy = moveY * speed * deltaTime;

  float nextX = x + dx;

  SDL_Rect nextBounds = bounds;
  nextBounds.x = static_cast<int>(nextX);
  // for future me: use this to detect intersection

  if (!SDL_HasIntersection(&nextBounds, &obstacle)) {
    x = nextX;
    bounds.x = static_cast<int>(x);
  }

  // for y axix now
  // hehe, don't underestimate me, im a jee student

  float nextY = y + dy;

  nextBounds = bounds;
  nextBounds.y = static_cast<int>(nextY);

  if (!SDL_HasIntersection(&nextBounds, &obstacle)) {
    y = nextY;
    bounds.y = static_cast<int>(y);
  }
}

void Player::handleInput() {
  const Uint8 *keyboard = SDL_GetKeyboardState(nullptr);

  velocityX = 0.0f;
  velocityY = 0.0f;

  if (keyboard[SDL_SCANCODE_W]) {

    velocityY -= 1.0f;
  }
  if (keyboard[SDL_SCANCODE_S]) {

    velocityY += 1.0f;
  }
  if (keyboard[SDL_SCANCODE_A]) {

    velocityX -= 1.0f;
  }
  if (keyboard[SDL_SCANCODE_D]) {
    velocityX += 1.0f;
  }
}

// center is like uhh we get the x coordinate of the player and 25 to it coz the
// width is 50, so we get (25,0)
float Player::getCenterX() const { return x + 25.0f; }
// and for this we get (0,25)

float Player::getCenterY() const { return y + 25.0f; }

SDL_Rect Player::getBounds() const { return bounds; }

void Player::constrainToWorld(int worldWidth, int worldHeight) {
  x = std::clamp(x, 0.0f, static_cast<float>(worldWidth - bounds.w));

  // LESSON 02: so what's gonna happen here is let's say the width is 10, and
  // player width is 2, 10-2 = 8 so it'sg onna stop when player reaches x, other
  // rest will be covered by the width of the player
  //
  //
  // NOW WHY AM I WRITING ALL THIS? COZ IM DUMB AND I'LL FORGET BASIC STUFF
  // MAKING THE NEXT GAME

  y = std::clamp(y, 0.0f, static_cast<float>(worldHeight - bounds.h));

  bounds.x = static_cast<int>(x);
  bounds.y = static_cast<int>(y);
}
