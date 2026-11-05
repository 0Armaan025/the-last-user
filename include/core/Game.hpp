#pragma once

#include "../../include/core/Camera.hpp"
#include "../../include/core/Player.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <iostream>

class Game {
public:
  Game(int width, int height);
  ~Game();

  Game(const Game &) = delete;
  Game &operator=(const Game &) = delete;

  bool init();
  Player player;

  void handleEvents();
  void update(float deltaTime);
  void draw();

  void run();
  void quit();

  SDL_Renderer *getRenderer() const;
  SDL_Window *getWindow() const;

  bool isRunning() const;

private:
  int screenWidth, screenHeight;

  Camera camera;
  bool running = false;

  SDL_Renderer *renderer = nullptr;
  SDL_Window *window = nullptr;
};
