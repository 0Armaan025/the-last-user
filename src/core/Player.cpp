#include "../../include/core/Player.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_render.h>
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

  // update pos

  x += velocityX * speed * deltaTime;
  y += velocityY * speed * deltaTime;
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
    velocityX += 10;
  }
}
