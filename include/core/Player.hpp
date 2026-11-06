#pragma once

#include <SDL2/SDL.h>

#include <iostream>

class Player {

public:
  Player();
  void handleInput();
  void draw(SDL_Renderer *renderer);

  void update(float deltaTime);

  float getCenterX() const;
  float getCenterY() const;

  SDL_Rect getBounds() const;

private:
  // updating position to be closer to the computer idk why i put it there :sob:
  float x = 1175.0f;
  float y = 1350.0f;

  float velocityX = 0.0f;
  float velocityY = 0.0f;
  float speed = 200.0f;

  SDL_Rect bounds{1775, 1150, 50, 50};
};
