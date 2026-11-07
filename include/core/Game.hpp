#pragma once

#include "../../include/core/Camera.hpp"
#include "../../include/core/Player.hpp"
#include "../../include/world/World.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>
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
  // finally teh wrold
  World world;

  void run();
  void quit();

  SDL_Renderer *getRenderer() const;
  SDL_Window *getWindow() const;

  void drawGrid();

  bool isRunning() const;
  bool nearComputer = false;
  bool computerInteracted = false;

private:
  int screenWidth, screenHeight;

  Camera camera;
  bool running = false;

  SDL_Renderer *renderer = nullptr;
  SDL_Window *window = nullptr;
  // finally text work

  TTF_Font *font = nullptr;
  // alright let's begin this now

  void drawInteractionPrompt();
  // alright

  // xp time

  bool xpDesktopActive = false;

  bool myComputerOpen = false;

  bool fileRead[3]{false, false, false};
  bool driveOpen = false;

  int selectedFolder = -1;
  int openedFile = -1;

  bool startMenuOpen = false;

  void drawFolderWindow();
  void drawTextFile();
  void drawMyComputerWindow();

  void drawDriveWindow();
  void drawWindowsXP();

  // atmopshere

  void drawAtmosphere();
};
