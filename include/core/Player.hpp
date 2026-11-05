#pragma once

#include <SDL2/SDL.h>

#include <iostream>

class Player {

public:
  void handleInput();
  Player();
  SDL_Rect player = {};
  void draw(SDL_Renderer *renderer);
};
