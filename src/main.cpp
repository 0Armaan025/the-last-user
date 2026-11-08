#include "../include/core/Game.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <iostream>

int main() {

  const int SCREEN_WIDTH = 900;
  const int SCREEN_HEIGHT = 480;
  Game game(SCREEN_WIDTH, SCREEN_HEIGHT);

  game.run();

  // let's continue
  return 0;
}
