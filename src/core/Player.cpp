#include "../../include/core/Player.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <iostream>

Player::Player() { player = {50, 50, 50, 50}; }

void Player::draw(SDL_Renderer *renderer) {
  SDL_SetRenderDrawColor(renderer, 180, 100, 50, 255);

  SDL_RenderFillRect(renderer, &player);
}

void Player::handleInput() {}
