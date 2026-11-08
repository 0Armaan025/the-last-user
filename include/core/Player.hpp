#pragma once

#include <SDL2/SDL.h>
#include <vector>

#include <iostream>

class Player {

public:
  Player();
  void handleInput();
  void draw(SDL_Renderer *renderer);

  // rn it'st just one obstacle, let's add more
  void update(float deltaTime, const std::vector<SDL_Rect> &obstacles);

  float getCenterX() const;
  float getCenterY() const;

  void constrainToWorld(int worldWidth, int worldHeight);
  SDL_Rect getBounds() const;

  void reset();

private:
  // updating position to be closer to the computer idk why i put it there :sob:
  float x = 1175.0f;
  float y = 1350.0f;

  float velocityX = 0.0f;
  float velocityY = 0.0f;
  float speed = 200.0f;

  SDL_Rect bounds{1175, 1350, 50, 50};
};
