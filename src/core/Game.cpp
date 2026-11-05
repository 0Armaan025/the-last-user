#include "../../include/core/Game.hpp"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_timer.h>

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
  SDL_Quit();
}

bool Game::init() {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {

    std::cout << "SDL ERROR: " << SDL_GetError() << std::endl;

    return false;
  }

  SDL_Window *sdl_window = SDL_CreateWindow(
      "The Last User", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
      screenWidth, screenHeight, SDL_WINDOW_RESIZABLE);

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
  }
}

void Game::update(float deltaTime) {
  // handle Input
  // and update player or something

  player.handleInput();
  player.update(deltaTime);

  camera.follow(player.getCenterX(), player.getCenterY());
}
void Game::draw() {
  SDL_SetRenderDrawColor(renderer, 24, 35, 30, 255);
  SDL_RenderClear(renderer);
  // drawing not the actual player but the player with the new camera coords
  SDL_Rect screenPlayer = camera.apply(player.getBounds());
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
