#include "../../include/core/Game.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
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
      if (xpDesktopActive) {
        if (e.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
          if (myComputerOpen) {
            myComputerOpen = false;
          } else {
            xpDesktopActive = false;
          }
        }
      }

      continue;
    }

    if (e.key.keysym.scancode == SDL_SCANCODE_E && nearComputer) {
      xpDesktopActive = true;
    }
  }

  if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT &&
      xpDesktopActive) {
    int mouseX = e.button.x;
    int mouseY = e.button.y;

    SDL_Point mousePosition{mouseX, mouseY};

    if (myComputerOpen) {
      SDL_Rect closeButton{screenWidth - 65, 55, 25, 25};

      // checks if a point is inside a rect
      if (SDL_PointInRect(&mousePosition, &closeButton)) {
        myComputerOpen = false;
      }
    }

    else {
      SDL_Rect myComputerIcon{25, 25, 60, 60};

      if (SDL_PointInRect(&mousePosition, &myComputerIcon)) {
        myComputerOpen = true;
      }
    }
  }
}

void Game::update(float deltaTime) {
  // handle Input
  // and update player or something

  if (xpDesktopActive) {
    return;
  }

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

  if (xpDesktopActive) {
    drawWindowsXP();
    SDL_RenderPresent(renderer);
    return;
  }
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

  // alright
  // btw i used chatgpt for this thing here coz i couldnt' figure out the
  // correct coords
  SDL_Rect textRect{screenWidth / 2 - surface->w / 2,
                    promptPanel.y + promptPanel.h / 2 - surface->h / 2,
                    surface->w, surface->h};

  SDL_RenderCopy(renderer, texture, nullptr, &textRect);
  SDL_DestroyTexture(texture);
  SDL_FreeSurface(surface);
}

void Game::drawWindowsXP() {
  // ALR THIS NEEDS TO BE PRETTY COOL LOOKING, IM GONNA GO AHEAD AND FIRE UP
  // EXCALIDRAW

  SDL_SetRenderDrawColor(renderer, 58, 110, 165, 255);

  SDL_RenderClear(renderer);

  // labmda functinos are handy
  auto drawText = [&](const char *message, int x, int y, SDL_Color color) {
    SDL_Surface *surface = TTF_RenderUTF8_Blended(font, message, color);

    if (!surface) {
      return;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture) {
      SDL_Rect destination{x, y, surface->w, surface->h};

      SDL_RenderCopy(renderer, texture, nullptr, &destination);

      SDL_DestroyTexture(texture);
    }

    SDL_FreeSurface(surface);
  };

  // icons

  struct DesktopIcon {
    const char *name;
    int x;
    int y;
    SDL_Color color;
  };

  // i need something that would look good for windows xp uhhh
  const DesktopIcon icons[] = {
      {"My computer", 25, 25, {120, 160, 190, 255}},
      {"Recycle bin", 25, 105, {180, 190, 195, 255}},
      {"Internet explorer", 25, 185, {40, 150, 220, 255}},
      {"VLC (THE OG)", 25, 265, {50, 200, 100, 255}},

      {"Documents", 25, 345, {220, 190, 100, 255}}};

  // iteration

  for (const DesktopIcon &icon : icons) {
    SDL_Rect iconShape{icon.x + 12, icon.y, 32, 32};

    SDL_SetRenderDrawColor(renderer, icon.color.r, icon.color.g, icon.color.b,
                           icon.color.a);

    SDL_RenderFillRect(renderer, &iconShape);

    SDL_SetRenderDrawColor(renderer, 235, 240, 245, 255);
    SDL_RenderDrawRect(renderer, &iconShape);

    drawText(icon.name, icon.x, icon.y + 38, {255, 255, 255, 255});
    // seperator

    SDL_SetRenderDrawColor(renderer, 120, 165, 210, 255);

    SDL_RenderDrawLine(renderer, 106, screenHeight - 36, 106, screenHeight - 4);
    // alright let's move ahead now

    drawText("2:00 AM", screenWidth - 95, screenHeight - 28,
             {255, 255, 255, 255});

    SDL_SetRenderDrawColor(renderer, 120, 165, 210, 255);
  }

  // so yeah

  if (myComputerOpen) {
    drawMyComputerWindow();
  }
}

// now time to draw my computer window

void Game::drawMyComputerWindow() {
  // firfst we ned to have a main window
  //

  SDL_Rect windowRect{100, 55, screenWidth - 200, screenHeight - 110};

  // body

  SDL_SetRenderDrawColor(renderer, 230, 230, 220, 255);
  SDL_RenderFillRect(renderer, &windowRect);

  SDL_Rect titleBar{windowRect.x, windowRect.y, windowRect.w, 32};
  SDL_SetRenderDrawColor(renderer, 20, 75, 155, 255);

  SDL_RenderFillRect(renderer, &titleBar);

  // title

  SDL_Surface *titleSurface =
      TTF_RenderUTF8_Blended(font, "My Computer", {255, 255, 255, 255});

  if (titleSurface) {
    SDL_Texture *titleTexture =
        SDL_CreateTextureFromSurface(renderer, titleSurface);

    if (titleTexture) {
      SDL_Rect destination{
          titleBar.x + 10,
          titleBar.y + 7,
          titleSurface->w,
          titleSurface->h,
      };

      SDL_RenderCopy(renderer, titleTexture, nullptr, &destination);
      SDL_DestroyTexture(titleTexture);
    }

    SDL_FreeSurface(titleSurface);
  }
  // now the close button on teh topright ig

  SDL_Rect closeBtn{screenWidth - 65, 55, 25, 25};

  SDL_SetRenderDrawColor(renderer, 190, 55, 45, 255);
  SDL_RenderFillRect(renderer, &closeBtn);

  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

  SDL_RenderDrawLine(renderer, closeBtn.x + 7, closeBtn.y + 7, closeBtn.x + 18,
                     closeBtn.y + 18);

  SDL_RenderDrawLine(renderer, closeBtn.x + 18, closeBtn.y + 7, closeBtn.x + 7,
                     closeBtn.y + 18);

  // content

  SDL_Rect contentArea = {windowRect.x + 15, windowRect.y + 45,
                          windowRect.w - 30, windowRect.h - 60};

  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_RenderFillRect(renderer, &contentArea);
  // uhm so uhh after this we just need a text and convert it into texture and
  // stuff, we need C: first

  SDL_Color textColor{30, 30, 30, 255};

  SDL_Surface *contentSurface =
      TTF_RenderUTF8_Blended(font, "Local Disk (C:)", textColor);

  if (contentSurface) {
    SDL_Texture *contentTexture =
        SDL_CreateTextureFromSurface(renderer, contentSurface);

    if (contentTexture) {
      SDL_Rect destination{contentArea.x + 20, contentArea.y + 20,
                           contentSurface->w, contentSurface->h};

      SDL_RenderCopy(renderer, contentTexture, nullptr, &destination);

      SDL_DestroyTexture(contentTexture);
    }

    SDL_FreeSurface(contentSurface);
  }
}
