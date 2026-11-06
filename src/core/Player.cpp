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

void Player::update(float deltaTime) {
  // let us allow the playre to move

  float direction_magnitude =
      sqrt((velocityX * velocityX) + (velocityY * velocityY));

  if (direction_magnitude > 0) {
    // normalize

    velocityX = velocityX / direction_magnitude;
    velocityY = velocityY / direction_magnitude;
  }

  x += velocityX * speed * deltaTime;
  y += velocityY * speed * deltaTime;

  // syncing rectangel with our pos

  bounds.x = static_cast<int>(x);
  bounds.y = static_cast<int>(y);
  std::cout << "Player: " << x << ", " << y << '\n';
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
