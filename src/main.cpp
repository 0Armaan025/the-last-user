#include "../src/core/Game.cpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <iostream>

int main() {

  const int SCREEN_WIDTH = 900;
  const int SCREEN_HEIGHT = 1200;
  Game game(SCREEN_WIDTH, SCREEN_HEIGHT);

  game.run();

  return 0;
}
