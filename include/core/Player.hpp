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
  float x = 50.0f;
  float y = 50.0f;

  float velocityX = 0.0f;
  float velocityY = 0.0f;
  float speed = 200.0f;

  SDL_Rect bounds{50, 50, 50, 50};
};
