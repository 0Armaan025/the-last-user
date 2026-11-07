#include "../../include/core/Game.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>
#include <algorithm>
#include <array>
#include <cmath>
#include <string>

Game::Game(int width, int height)
    : screenWidth(width), screenHeight(height), camera(width, height) {}

Game::~Game() {
  if (renderer) {

    SDL_DestroyRenderer(renderer);
    renderer = nullptr;
  }
  if (window) {
    SDL_DestroyWindow(window);
    window = nullptr;
  }
  if (font) {
    TTF_CloseFont(font);
    font = nullptr;
  }
  TTF_Quit();
  // should work ig
  SDL_Quit();
}

bool Game::init() {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {

    std::cout << "SDL ERROR: " << SDL_GetError() << std::endl;

    return false;
  }

  SDL_Window *sdl_window = SDL_CreateWindow(
      "The Last User", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
      screenWidth, screenHeight, SDL_WINDOW_SHOWN);

  if (!sdl_window) {
    std::cout << "could not make a window" << std::endl;
    return false;
  }
  window = sdl_window;

  SDL_Renderer *sdl_renderer = SDL_CreateRenderer(
      window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

  if (!sdl_renderer) {
    std::cout << "renderer error" << SDL_GetError() << std::endl;
    return false;
  }

  renderer = sdl_renderer;

  // otherwise, return treu
  //
  //
  // font time now

  // iuse arch btw

  if (TTF_Init() == -1) {
    std::cout << "ttf error: " << TTF_GetError() << "\n";
    return false;
  }
  font = TTF_OpenFont("/usr/share/fonts/TTF/DejaVuSerif.ttf", 18);

  if (!font) {
    std::cout << "Font loading faileeed: " << TTF_GetError() << std::endl;
    return false;
  }
  running = true;
  return true;
}

bool Game::isRunning() const { return running; }

void Game::handleEvents() {

  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_QUIT) {
      running = false;
    }

    if (e.type == SDL_KEYDOWN && !e.key.repeat) {
      if (e.key.keysym.scancode == SDL_SCANCODE_E) {
        if (nearComputer) {
          computerInteracted = true;
        }
      }
    }
  }
}

void Game::update(float deltaTime) {
  // handle Input
  // and update player or something

  player.handleInput();
  std::vector<SDL_Rect> obstacles = world.getSkeletonBounds();
  obstacles.push_back(world.getComputerBounds());
  player.update(deltaTime, obstacles);
  player.constrainToWorld(2400, 1600);

  SDL_Rect computer = world.getComputerBounds();

  float playerX = player.getCenterX();
  float playerY = player.getCenterY();
  // nearest point on the computer rect
  float nearestX = std::clamp(playerX, static_cast<float>(computer.x),

                              static_cast<float>(computer.x + computer.w));

  float nearestY = std::clamp(playerY, static_cast<float>(computer.y),

                              static_cast<float>(computer.y + computer.h));

  float dx = playerX - nearestX;
  float dy = playerY - nearestY;

  float distance = std::sqrt(dx * dx + dy * dy);

  constexpr float INTERACTION_DISTANCE = 120.0f;

  nearComputer = distance <= INTERACTION_DISTANCE;

  // testing

  // if (nearComputer) {
  //   std::cout << "Near computer" << std::endl;
  // }
  //
  // a lil optimization as well
  //

  camera.follow(playerX, playerY, deltaTime);
}
void Game::draw() {
  SDL_SetRenderDrawColor(renderer, 24, 35, 30, 255);
  SDL_RenderClear(renderer);
  // drawGrid();
  // drawing not the actual player but the player with the new camera coords
  // let us draw the world before the player

  world.draw(renderer, camera);
  SDL_Rect screenPlayer = camera.apply(player.getBounds());
  drawInteractionPrompt();
  SDL_SetRenderDrawColor(renderer, 180, 100, 50, 255);
  SDL_RenderFillRect(renderer, &screenPlayer);
  SDL_RenderPresent(renderer);
}

void Game::run() {
  if (!init()) {
    return;
  }

  Uint64 previousTime = SDL_GetPerformanceCounter();
  while (running) {
    Uint64 currentTime = SDL_GetPerformanceCounter();

    float deltaTime =
        static_cast<float>(static_cast<double>(currentTime - previousTime) /
                           static_cast<double>(SDL_GetPerformanceFrequency()));

    previousTime = currentTime;
    handleEvents();
    update(deltaTime);
    draw();
  }
}

// debuggin

void Game::drawGrid() {
  // SDL_SetRenderDrawColor(renderer, 55, 65, 60, 255);
  // constexpr int GRID_SIZE = 100;
  // constexpr int WORLD_WIDTH = 3000;
  // constexpr int WORLD_HEIGHT = 2000;
  //
  // for (int x = 0; x <= WORLD_WIDTH; x += GRID_SIZE) {
  //   SDL_Rect line = camera.apply({x, 0, 1, WORLD_HEIGHT});
  //   SDL_RenderFillRect(renderer, &line);
  // }
  //
  // for (int y = 0; y <= WORLD_HEIGHT; y += GRID_SIZE) {
  //   SDL_Rect line = camera.apply({0, y, WORLD_WIDTH, 1});
  //   SDL_RenderFillRect(renderer, &line);
  // }
}

void Game::drawInteractionPrompt() {
  if (!nearComputer) {
    return;
  }

  // i want it less than half
  // and a lil less thant he  height
  SDL_Rect promptPanel{screenWidth / 2 - 160, screenHeight - 75, 320, 45};

  // bgg

  SDL_SetRenderDrawColor(renderer, 12, 19, 16, 235);
  SDL_RenderFillRect(renderer, &promptPanel);

  // i wanna mkae it look a lil better, so i'll add a cool border

  SDL_SetRenderDrawColor(renderer, 110, 135, 112, 225);
  // LESSON 04: rememgber armaan, we ain't using SDL_RenderFillRect her, we will
  // use RenderDrawRect for the bordr

  SDL_RenderDrawRect(renderer, &promptPanel);
  ;

  const char *message =
      computerInteracted ? "COMPUTER READY" : "Press [E] to interact";

  SDL_Color textColor{210, 220, 205, 255};
  SDL_Surface *surface = TTF_RenderUTF8_Blended(font, message, textColor);
  // also future m, u could use utf8_solid but yeah this works too

  if (!surface) {
    std::cout << "Text rendering failed: " << TTF_GetError() << std::endl;
    return;
  }

  // we need surface and textures for our beloved gpu

  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

  if (!texture) {
    SDL_FreeSurface(surface);
    // some error
    std::cout << "some error came: " << SDL_GetError() << "\n";
  }

  // btw i used chatgpt for this thing here coz i couldnt' figure out the
  // correct coords
  SDL_Rect textRect{screenWidth / 2 - surface->w / 2,
                    promptPanel.y + promptPanel.h / 2 - surface->h / 2,
                    surface->w, surface->h};

  SDL_RenderCopy(renderer, texture, nullptr, &textRect);
  SDL_DestroyTexture(texture);
  SDL_FreeSurface(surface);
}
