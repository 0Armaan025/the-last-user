#include "../../include/core/Game.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>
#include <algorithm>
#include <array>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>

#ifdef __EMSCRIPTEN__
  #define SDL_PIXELFORMAT_RGBA32 SDL_PIXELFORMAT_ARGB8888
#endif

struct StoryFile {
  const char *name;
  const char *content;
};
// going ahead

static std::string formatSurvivalTime(float seconds) {
  const int totalSeconds =
      static_cast<int>(std::floor(std::max(0.0f, seconds)));

  const int minutes = totalSeconds / 60;
  const int remainingSeconds = totalSeconds % 60;

  std::ostringstream output;

  output << std::setfill('0') << std::setw(2) << minutes << ":" << std::setw(2)
         << remainingSeconds;

  return output.str();
}

void Game::loadBestSurvivalTime() {
  std::ifstream file(recordPath);

  if (file.is_open()) {
    file >> bestSurvivalTime;

    if (!std::isfinite(bestSurvivalTime) || bestSurvivalTime < 0.0f) {
      bestSurvivalTime = 0.0f;
    }
  }
}

void Game::saveBestSurvivalTime() const {
  std::ofstream file(recordPath, std::ios::trunc);

  if (file.is_open()) {
    file << std::fixed << std::setprecision(3) << bestSurvivalTime;
  }
}

struct StoryFolder {
  const char *name;
  const StoryFile *files;
  int fileCount;
};
// alright

const StoryFile systemFiles[] = {
    {"readme.txt", "SYSTEM RECOVERY LOG \n\n"
                   "Date: 7 Nov\n"
                   "Status: CRITICAL\n\n"
                   "The network has gone silent.\n"
                   "No response from outside stations.\n"
                   "I am beginning to think we are the only ones left."},
    {
        "last_message.txt",
        "PERSONAL MESSAGE \n"
        "If someone finds this computer...\n"
        "Do not assume the town is empty.\n\n"
        "I heard the telephoner ring yesterday"
        "There is no telephone line connected.\n",
    },
    {

        "unknown_file.txt", "RECOVERED DATA\n\n"
                            "You found the first two msges.\n"
                            "That means someone is still searching...\n\n"
                            "Check the date of the last transmission.\n"
                            "It was not sent from this town."}};

const StoryFolder storyFolders[] = {{"Documents", systemFiles, 3}};

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
  telephone.cleanup();
  Mix_CloseAudio();
  TTF_Quit();
  // should work ig
  SDL_Quit();
  // alrigth let's go ahead now
  // alrighttt
}

bool Game::init() {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {

    std::cout << "SDL ERROR: " << SDL_GetError() << std::endl;

    return false;
  }

#ifdef __EMSCRIPTEN__
  recordPath = "survival_record.txt";
  loadBestSurvivalTime();
#else
  char *saveDirectory = SDL_GetPrefPath("Armaan", "TheLastUser");

  if (saveDirectory) {
    recordPath = std::string(saveDirectory) + "survival_record.txt";

    SDL_free(saveDirectory);

    loadBestSurvivalTime();
  }

  else {
    std::cout << "Could not create save directory: " << SDL_GetError() << "\n";
  }
#endif

  if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
    std::cout << "audio init faile: " << Mix_GetError() << "\n ";
    return false;

    return false;
  }

  if (!telephone.loadRingSound("assets/audio/telephone_ring.wav")) {
    std::cout << "telephne is silent" << "\n";
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
  font = TTF_OpenFont("assets/fonts/DejaVuSerif.ttf", 18);

  if (!font) {
    font = TTF_OpenFont("/usr/share/fonts/TTF/DejaVuSerif.ttf", 18);
  }

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

    // Window close
    if (e.type == SDL_QUIT) {
      running = false;
      continue;
    }

    if (e.type == SDL_KEYDOWN && !e.key.repeat) {

      // Main menu
      if (gameState == GameState::MainMenu) {
        switch (e.key.keysym.sym) {
        case SDLK_UP:
        case SDLK_w:
          selectedMenuOption = (selectedMenuOption + 2) % 3;
          break;

        case SDLK_DOWN:
        case SDLK_s:
          selectedMenuOption = (selectedMenuOption + 1) % 3;
          break;

        case SDLK_RETURN:
        case SDLK_KP_ENTER:
          if (selectedMenuOption == 0) {
            startNewGame();
          } else if (selectedMenuOption == 1) {
            gameState = GameState::HowToPlay;
          } else if (selectedMenuOption == 2) {
            running = false;
          }
          break;

        case SDLK_ESCAPE:
          running = false;
          break;
        }

        continue;
      }

      // How to play
      if (gameState == GameState::HowToPlay) {
        if (e.key.keysym.sym == SDLK_ESCAPE ||
            e.key.keysym.sym == SDLK_RETURN) {
          gameState = GameState::MainMenu;
        }

        continue;
      }

      // Game over
      if (gameState == GameState::GameOver) {
        if (e.key.keysym.scancode == SDL_SCANCODE_R) {
          startNewGame();
        } else if (e.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
          running = false;
        }

        continue;
      }

      // Don't process gameplay input after game over
      if (gameOver) {
        continue;
      }

      // Telephone dialogue
      if (telephoneMessageOpen) {
        if (e.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
          telephoneMessageOpen = false;
        }

        continue;
      }

      // Windows XP keyboard controls
      if (xpDesktopActive) {
        if (e.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {

          if (openedFile >= 0) {
            openedFile = -1;
          } else if (selectedFolder >= 0) {
            selectedFolder = -1;
          } else if (driveOpen) {
            driveOpen = false;
          } else if (myComputerOpen) {
            myComputerOpen = false;
          } else {
            xpDesktopActive = false;
          }
        }

        continue;
      }

      // World interaction
      if (e.key.keysym.scancode == SDL_SCANCODE_E) {
        if (telephone.isNear()) {
          telephoneMessageOpen = true;
        } else if (nearComputer) {
          xpDesktopActive = true;
          computerInteracted = true;
        }
      }

      continue;
    }

    if (e.type != SDL_MOUSEBUTTONDOWN || e.button.button != SDL_BUTTON_LEFT) {
      continue;
    }

    if (!xpDesktopActive || gameOver) {
      continue;
    }

    SDL_Point mouse{e.button.x, e.button.y};

    // Windows XP Start button
    SDL_Rect startButton{0, screenHeight - 40, 110, 40};

    if (SDL_PointInRect(&mouse, &startButton)) {
      startMenuOpen = !startMenuOpen;
      continue;
    }

    // Start menu options
    if (startMenuOpen) {
      SDL_Rect menu{0, screenHeight - 340, 270, 300};

      if (SDL_PointInRect(&mouse, &menu)) {

        SDL_Rect computerOption{15, screenHeight - 290, 240, 45};

        SDL_Rect documentsOption{15, screenHeight - 240, 240, 45};

        SDL_Rect shutdownOption{15, screenHeight - 100, 240, 45};

        // My Computer
        if (SDL_PointInRect(&mouse, &computerOption)) {
          myComputerOpen = true;
          driveOpen = false;
          selectedFolder = -1;
          openedFile = -1;
          startMenuOpen = false;
        }

        // Documents
        else if (SDL_PointInRect(&mouse, &documentsOption)) {
          myComputerOpen = true;
          driveOpen = true;
          selectedFolder = 0;
          openedFile = -1;
          startMenuOpen = false;
        }

        // Shutdown
        else if (SDL_PointInRect(&mouse, &shutdownOption)) {
          xpDesktopActive = false;
          startMenuOpen = false;
          myComputerOpen = false;
          driveOpen = false;
          selectedFolder = -1;
          openedFile = -1;
        }

      } else {
        startMenuOpen = false;
      }

      continue;
    }

    // Text file window
    if (openedFile >= 0) {
      SDL_Rect closeButton{screenWidth - 65, 55, 25, 25};

      if (SDL_PointInRect(&mouse, &closeButton)) {
        openedFile = -1;
      }

      continue;
    }

    // Folder window
    if (selectedFolder >= 0) {
      SDL_Rect closeButton{screenWidth - 65, 55, 25, 25};

      if (SDL_PointInRect(&mouse, &closeButton)) {
        selectedFolder = -1;
        continue;
      }

      bool allCluesFound = fileRead[0] && fileRead[1];
      int visibleRow = 0;

      for (int i = 0; i < storyFolders[selectedFolder].fileCount; ++i) {

        if (i == 2 && !allCluesFound) {
          continue;
        }

        SDL_Rect fileButton{135, 120 + visibleRow * 45, 300, 35};

        if (SDL_PointInRect(&mouse, &fileButton)) {
          openedFile = i;

          if (i < 2) {
            fileRead[i] = true;
          }

          break;
        }

        ++visibleRow;
      }

      continue;
    }

    if (driveOpen) {
      SDL_Rect closeButton{screenWidth - 65, 55, 25, 25};

      if (SDL_PointInRect(&mouse, &closeButton)) {
        driveOpen = false;
        continue;
      }

      SDL_Rect folderButton{135, 120, 300, 35};

      if (SDL_PointInRect(&mouse, &folderButton)) {
        selectedFolder = 0;
      }

      continue;
    }

    if (myComputerOpen) {

      // Matches the close button drawn in drawMyComputerWindow()
      SDL_Rect closeButton{screenWidth - 135, 59, 25, 25};

      if (SDL_PointInRect(&mouse, &closeButton)) {
        myComputerOpen = false;
        continue;
      }

      SDL_Rect driveButton{135, 120, 250, 60};

      if (SDL_PointInRect(&mouse, &driveButton)) {
        driveOpen = true;
      }

      continue;
    }

    SDL_Rect computerIcon{25, 25, 60, 60};

    if (SDL_PointInRect(&mouse, &computerIcon)) {
      myComputerOpen = true;
    }
  }
}
void Game::update(float deltaTime) {

  // Only update gameplay when actually playing
  if (gameState != GameState::Playing || gameOver) {
    return;
  }

  // Reaper
  reaper.update(player.getCenterX(), player.getCenterY(),
                fileRead[0] && fileRead[1], deltaTime);

  // Pause world interactions while interfaces are open
  if (xpDesktopActive || telephoneMessageOpen) {
    return;
  }

  // Player movement
  player.handleInput();

  std::vector<SDL_Rect> obstacles = world.getSkeletonBounds();
  obstacles.push_back(world.getComputerBounds());

  player.update(deltaTime, obstacles);
  player.constrainToWorld(2400, 1600);

  // Update bat swarm
  const float damage = batSwarm.update(player.getCenterX(), player.getCenterY(),
                                       fileRead[0] && fileRead[1], deltaTime);

  // Start survival timer when first wave appears
  if (batSwarm.getWave() > 0) {
    survivalStarted = true;
  }

  if (survivalStarted) {
    survivalTime += std::max(0.0f, deltaTime);
  }

  // Apply damage
  playerHealth = std::max(0.0f, playerHealth - damage);

  // Game over
  if (playerHealth <= 0.0f) {
    finalSurvivalTime = survivalTime;

    newPersonalRecord = survivalStarted && finalSurvivalTime > bestSurvivalTime;

    if (newPersonalRecord) {
      bestSurvivalTime = finalSurvivalTime;
      saveBestSurvivalTime();
    }

    gameOver = true;
    gameState = GameState::GameOver;

    return;
  }

  // Energy blaster
  const Uint8 *keyboard = SDL_GetKeyboardState(nullptr);

  blaster.update(player.getCenterX(), player.getCenterY(),
                 keyboard[SDL_SCANCODE_SPACE], deltaTime, batSwarm);

  // Telephone
  telephone.update(player.getCenterX(), player.getCenterY());

  // Computer interaction distance
  SDL_Rect computer = world.getComputerBounds();

  const float playerX = player.getCenterX();
  const float playerY = player.getCenterY();

  const float nearestX =
      std::clamp(playerX, static_cast<float>(computer.x),
                 static_cast<float>(computer.x + computer.w));

  const float nearestY =
      std::clamp(playerY, static_cast<float>(computer.y),
                 static_cast<float>(computer.y + computer.h));

  const float dx = playerX - nearestX;
  const float dy = playerY - nearestY;

  const float distance = std::sqrt(dx * dx + dy * dy);

  constexpr float INTERACTION_DISTANCE = 120.0f;

  nearComputer = distance <= INTERACTION_DISTANCE;

  // Camera
  camera.follow(playerX, playerY, deltaTime);
}
void Game::draw() {

  SDL_SetRenderDrawColor(renderer, 5, 7, 9, 255);
  SDL_RenderClear(renderer);

  if (gameState == GameState::MainMenu) {
    drawMainMenu();
    SDL_RenderPresent(renderer);
    return;
  }
  if (gameState == GameState::HowToPlay) {
    drawHowToPlay();
    SDL_RenderPresent(renderer);
    return;
  }
  // Main menu
  // Game over
  if (gameState == GameState::GameOver) {
    drawGameOver();
    SDL_RenderPresent(renderer);
    return;
  }

  // Windows XP desktop
  if (xpDesktopActive) {
    drawWindowsXP();
    SDL_RenderPresent(renderer);
    return;
  }

  // World
  world.draw(renderer, camera);

  // World objects
  telephone.render(renderer, camera);
  reaper.render(renderer, camera);
  batSwarm.render(renderer, camera);
  blaster.render(renderer, camera);

  // Player
  SDL_Rect screenPlayer = camera.apply(player.getBounds());

  SDL_SetRenderDrawColor(renderer, 180, 100, 50, 255);
  SDL_RenderFillRect(renderer, &screenPlayer);

  // Atmosphere and HUD
  drawAtmosphere();
  drawHealthBar();
  drawSurvivalInfo();
  drawWaveInfo();

  // Blaster status
  if (blaster.isCollected()) {

    SDL_Surface *surface = TTF_RenderUTF8_Blended(
        font, "LASER READY | HOLD SPACE TO FIRE", {100, 210, 240, 255});

    if (surface) {
      SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

      if (texture) {
        SDL_Rect destination{25, 85, surface->w, surface->h};

        SDL_RenderCopy(renderer, texture, nullptr, &destination);

        SDL_DestroyTexture(texture);
      }

      SDL_FreeSurface(surface);
    }
  }

  // Interaction prompt
  drawInteractionPrompt();

  // Telephone dialogue
  if (telephoneMessageOpen) {
    drawTelephoneMessage();
  }

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
  if (!nearComputer && !telephone.isNear()) {
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
  // LESSON 04: rememgber armaan, we ain't using SDL_RenderFillRect her, we
  // will use RenderDrawRect for the bordr

  SDL_RenderDrawRect(renderer, &promptPanel);
  ;

  const char *message =
      telephone.isNear()
          ? "Press [E] to answer"
          : (computerInteracted ? "COMPUTER READY" : "Press [E] to interact");

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
    std::cout << "some error came: " << SDL_GetError() << "\n";
    SDL_FreeSurface(surface);
    return;
    // some error
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

void Game::drawAtmosphere() {
  static SDL_Texture *vignette = nullptr;
  static int cachedWidth = 0;
  static int cachedHeight = 0;

  if (!vignette || cachedWidth != screenWidth || cachedHeight != screenHeight) {

    if (vignette) {
      SDL_DestroyTexture(vignette);
      vignette = nullptr;
    }

    // btw this is my first time using it and chatgpt suggested to use it
    SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(
        0, screenWidth, screenHeight, 32, SDL_PIXELFORMAT_RGBA32);

    if (!surface)
      return;

    auto *pixels = static_cast<Uint32 *>(surface->pixels);

    float centerX = screenWidth / 2.0f;
    float centerY = screenHeight / 2.0f;

    float maxDistance = std::sqrt(centerX * centerX + centerY + centerY);

    for (int y = 0; y < screenHeight; ++y) {
      for (int x = 0; x < screenWidth; ++x) {
        float dx = static_cast<float>(x) - centerX;
        float dy = static_cast<float>(y) - centerY;

        float distance = std::sqrt(dx * dx + dy * dy);
        float maxRadius = std::sqrt(centerX * centerX + centerY * centerY);
        float normalizeDistance = distance / maxRadius;
        float strength =
            std::clamp((normalizeDistance - 0.35f) / 0.65f, 0.0f, 1.0f);

        strength = strength * strength * (3.0f - 2.0f * strength);

        // smoothstep

        strength = strength * strength * (3.0 - 2.0f * strength);

        Uint8 alpha = static_cast<Uint8>(strength * 155.0f);

        pixels[y * (surface->pitch / 4) + x] =
            SDL_MapRGBA(surface->format, 0, 0, 0, alpha);
      }
    }

    vignette = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!vignette) {
      return;
    }

    SDL_SetTextureBlendMode(vignette, SDL_BLENDMODE_BLEND);

    cachedWidth = screenWidth;
    cachedHeight = screenHeight;
  }

  SDL_RenderCopy(renderer, vignette, nullptr, nullptr);

  // let us make dark stuff
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
                           icon.color.a
                           );

    SDL_RenderFillRect(renderer, &iconShape);

    SDL_SetRenderDrawColor(renderer, 235, 240, 245, 255);
    SDL_RenderDrawRect(renderer, &iconShape);

    drawText(icon.name, icon.x, icon.y + 38, {255, 255, 255, 255});
    // seperator

    SDL_SetRenderDrawColor(renderer, 120, 165, 210, 255);

    SDL_SetRenderDrawColor(renderer, 120, 165, 210, 255);
  }

  // alright let's move ahead now

  //

  // update here

  SDL_Rect taskbar{0, screenHeight - 40, screenWidth, 40};

  SDL_SetRenderDrawColor(renderer, 25, 75, 145, 255);
  SDL_RenderFillRect(renderer, &taskbar);

  // start btn

  SDL_Rect startButton{0, screenHeight - 40, 110, 40};

  SDL_SetRenderDrawColor(renderer, 35, 145, 65, 255);
  // uhhh i have a LOT TO DO, approx 60% to copmlete, im tireeed

  SDL_RenderFillRect(renderer, &startButton);

  SDL_SetRenderDrawColor(renderer, 100, 200, 110, 255);

  SDL_RenderDrawRect(renderer, &startButton);
  // AAYEEE IM TRYING IM TRRYING , lyrics of a song btw
  // so yeah
  //
  //

  drawText("start", 35, screenHeight - 30, {255, 255, 255, 255});

  drawText("2:00 AM", screenWidth - 95, screenHeight - 28,
           {255, 255, 255, 255});

  if (myComputerOpen) {
    drawMyComputerWindow();

    if (driveOpen) {
      drawDriveWindow();

      if (selectedFolder == 0) {
        drawFolderWindow();

        if (openedFile >= 0) {
          drawTextFile();
        }
      }
    }
  }

  if (startMenuOpen) {
    SDL_Rect menu{0, screenHeight - 340, 270, 300};

    SDL_SetRenderDrawColor(renderer, 225, 230, 225, 255);
    SDL_RenderFillRect(renderer, &menu);

    SDL_Rect menuHeader{0, screenHeight - 340, 270, 55};

    SDL_SetRenderDrawColor(renderer, 35, 105, 175, 255);

    SDL_RenderFillRect(renderer, &menuHeader);

    drawText("Windows XP", 15, screenHeight - 325, {255, 255, 255, 255});
    drawText("My Computer", 25, screenHeight - 280, {30, 30, 30, 255});

    drawText("Documents", 25, screenHeight - 230, {30, 30, 30, 255});

    SDL_SetRenderDrawColor(renderer, 180, 180, 180, 255);
    SDL_RenderDrawLine(renderer, 10, screenHeight - 115, 260,
                       screenHeight - 115);

    drawText("Turn off Computer", 25, screenHeight - 85, {150, 30, 30, 255});
    // alright
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

  SDL_Rect closeBtn{screenWidth - 135, 59, 25, 25};

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

  SDL_Rect driveIcon{contentArea.x + 20, contentArea.y + 20, 45, 35};

  SDL_SetRenderDrawColor(renderer, 90, 100, 115, 255);
  SDL_RenderFillRect(renderer, &driveIcon);

  SDL_SetRenderDrawColor(renderer, 180, 190, 205, 255);
  SDL_RenderDrawRect(renderer, &driveIcon);

  SDL_Surface *driveSurface =
      TTF_RenderUTF8_Blended(font, "C:", {30, 30, 30, 255});

  if (driveSurface) {
    SDL_Texture *driveTexture =
        SDL_CreateTextureFromSurface(renderer, driveSurface);

    if (driveTexture) {
      SDL_Rect dest{contentArea.x + 30, contentArea.y + 25, driveSurface->w,
                    driveSurface->h};

      SDL_RenderCopy(renderer, driveTexture, nullptr, &dest);
      SDL_DestroyTexture(driveTexture);
    }

    SDL_FreeSurface(driveSurface);
  }

  // update ^
  SDL_Color textColor{30, 30, 30, 255};

  SDL_Surface *contentSurface =
      TTF_RenderUTF8_Blended(font, "Local Disk (C:)", textColor);

  if (contentSurface) {
    SDL_Texture *contentTexture =
        SDL_CreateTextureFromSurface(renderer, contentSurface);

    if (contentTexture) {
      // man i really need hot reload or something, testing these
      SDL_Rect destination{contentArea.x + 80, contentArea.y + 30,
                           contentSurface->w, contentSurface->h};

      SDL_RenderCopy(renderer, contentTexture, nullptr, &destination);

      SDL_DestroyTexture(contentTexture);
    }

    SDL_FreeSurface(contentSurface);
  }
}

// now the drive window time

void Game::drawDriveWindow() {
  SDL_Rect windowRect{100, 55, screenWidth - 200, screenHeight - 110};

  SDL_SetRenderDrawColor(renderer, 230, 230, 220, 255);
  SDL_RenderFillRect(renderer, &windowRect);

  // title bar

  SDL_Rect titleBar{windowRect.x, windowRect.y, windowRect.w, 32};

  SDL_SetRenderDrawColor(renderer, 20, 75, 155, 255);
  SDL_RenderFillRect(renderer, &titleBar);

  // alright

  // title text

  SDL_Surface *surface =
      TTF_RenderUTF8_Blended(font, "Local Disk (C:)", {255, 255, 255, 255});
  // i should really make a white color global variable

  if (surface) {
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    if (texture) {
      SDL_Rect destination{titleBar.x + 10, titleBar.y + 7, surface->w,
                           surface->h};

      SDL_RenderCopy(renderer, texture, nullptr, &destination);

      SDL_DestroyTexture(texture);
    }

    SDL_FreeSurface(surface);
  }

  // close btn here
  // i shoud just have a helper function as a compoent or something

  SDL_Rect closeBtn{screenWidth - 65, 55, 25, 25};

  SDL_SetRenderDrawColor(renderer, 190, 55, 45, 255);
  SDL_RenderFillRect(renderer, &closeBtn);

  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  // white should def. help lol

  SDL_RenderDrawLine(renderer, closeBtn.x + 7, closeBtn.y + 7, closeBtn.x + 18,
                     closeBtn.y + 18);

  SDL_RenderDrawLine(renderer, closeBtn.x + 18, closeBtn.y + 7, closeBtn.x + 7,
                     closeBtn.y + 18);

  // again, the content

  SDL_Rect content{windowRect.x + 15, windowRect.y + 45, windowRect.w - 30,
                   windowRect.h - 60};

  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_RenderFillRect(renderer, &content);

  // SO FOR MY IDEA TO FETCH INFO FROM THE FOLDERS/FILES, we gonna need some

  const char *folders[] = {"Documents and settings", "Program files", "WINDOWS",
                           "Users", "System vol info"};

  int y = content.y + 20;

  for (const char *folder : folders) {
    SDL_Rect folderIcon{content.x + 20, y, 28, 22};
    SDL_SetRenderDrawColor(renderer, 220, 180, 65, 255);
    SDL_RenderFillRect(renderer, &folderIcon);

    SDL_Surface *folderSurface =
        TTF_RenderUTF8_Blended(font, folder, {35, 35, 35, 255});

    if (folderSurface) {
      SDL_Texture *folderTexture =
          SDL_CreateTextureFromSurface(renderer, folderSurface);

      if (folderTexture) {
        SDL_Rect destination{content.x + 65, y + 2, folderSurface->w,
                             folderSurface->h};

        SDL_RenderCopy(renderer, folderTexture, nullptr, &destination);

        SDL_DestroyTexture(folderTexture);
      }

      SDL_FreeSurface(folderSurface);
    }

    y += 42;
  }
}

void Game::drawFolderWindow() {
  // alright let's begin

  SDL_Rect panel{100, 55, screenWidth - 200, screenHeight - 110};

  SDL_SetRenderDrawColor(renderer, 235, 235, 225, 255);

  SDL_RenderFillRect(renderer, &panel);

  SDL_Rect title{100, 55, screenWidth - 200, 32};

  SDL_SetRenderDrawColor(renderer, 20, 75, 155, 255);

  SDL_RenderFillRect(renderer, &title);

  SDL_Surface *surface =
      TTF_RenderUTF8_Blended(font, "Documents", {255, 255, 255, 255});

  if (surface) {
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    if (texture) {
      SDL_Rect dst{110, 62, surface->w, surface->h};
      SDL_RenderCopy(renderer, texture, nullptr, &dst);
      SDL_DestroyTexture(texture);
    }

    SDL_FreeSurface(surface);
  }

  SDL_Rect content{115, 100, screenWidth - 230, screenHeight - 170};
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_RenderFillRect(renderer, &content);

  int y = 120;

  for (int i = 0; i < storyFolders[0].fileCount; ++i) {
    SDL_Surface *fileSurface = TTF_RenderUTF8_Blended(
        font, storyFolders[0].files[i].name, {30, 30, 30, 255});

    if (!fileSurface)
      continue;

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, fileSurface);

    if (texture) {
      SDL_Rect dst{145, y, fileSurface->w, fileSurface->h};
      SDL_RenderCopy(renderer, texture, nullptr, &dst);
      SDL_DestroyTexture(texture);
    }

    SDL_FreeSurface(fileSurface);
    y += 45;

    if (fileRead[i]) {
      SDL_Rect readIndicator{screenWidth - 155, y + 5, 12, 12};

      SDL_SetRenderDrawColor(renderer, 65, 160, 85, 255);
      SDL_RenderFillRect(renderer, &readIndicator);
    }
  }
}

void Game::drawTextFile() {
  SDL_Rect panel{80, 45, screenWidth - 160, screenHeight - 90};
  SDL_SetRenderDrawColor(renderer, 235, 235, 220, 255);

  SDL_RenderFillRect(renderer, &panel);

  SDL_Rect title{80, 45, screenWidth - 160, 32};
  SDL_SetRenderDrawColor(renderer, 20, 75, 155, 255);
  SDL_RenderFillRect(renderer, &title);

  const char *content = storyFolders[0].files[openedFile].content;

#ifdef __EMSCRIPTEN__
  SDL_Surface *surface = TTF_RenderUTF8_Blended(font, content, {25, 25, 25, 255});
#else
  SDL_Surface *surface = TTF_RenderUTF8_Blended_Wrapped(
      font, content, {25, 25, 25, 255}, screenWidth - 210);
#endif

  if (surface) {

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    if (texture) {
      SDL_Rect dst{105, 100, surface->w, surface->h};
      SDL_RenderCopy(renderer, texture, nullptr, &dst);
      SDL_DestroyTexture(texture);
    }

    SDL_FreeSurface(surface);
  }
}

void Game::drawTelephoneMessage() {
  SDL_Rect panel{screenWidth / 2 - 280, screenHeight - 190, 560, 140};

  SDL_SetRenderDrawColor(renderer, 12, 15, 14, 245);
  SDL_RenderFillRect(renderer, &panel);

  SDL_SetRenderDrawColor(renderer, 130, 115, 95, 255);
  SDL_RenderDrawRect(renderer, &panel);

  const char *message = nullptr;

  if (!fileRead[0] && !fileRead[1]) {
    message = "UNKNOWN CALLER\n\n"
              "You shouldn't have come back.\n"
              "The computer was never supposed to turn on.\n\n"
              "[ESC] Close";
  }

  else if (fileRead[0] && !fileRead[1]) {
    message = "UNKNOWN CALLER\n\n"
              "You found the recovery log.\n"
              "That wasn't meant for you.\n\n"
              "Stop searching.\n\n"
              "[ESC] Close";
  }

  else if (fileRead[0] && fileRead[1]) {
    message = "UNKNOWN CALLER\n\n"
              "SO you've read both messages.\n"
              "Then you've probably noticed the date.\n\n"
              "Look outside. Someone is waiting.\n\n"
              "[ESC] Close";
  }
  // okay now time to add reaper

#ifdef __EMSCRIPTEN__
  SDL_Surface *surface = TTF_RenderUTF8_Blended(font, message, {215, 210, 190, 255});
#else
  SDL_Surface *surface =
      TTF_RenderUTF8_Blended_Wrapped(font, message, {215, 210, 190, 255}, 510);
#endif

  if (!surface) {
    return;
  }

  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

  if (texture) {
    SDL_Rect destination{panel.x + 25, panel.y + 15, surface->w, surface->h};

    SDL_RenderCopy(renderer, texture, nullptr, &destination);
    SDL_DestroyTexture(texture);
  }

  SDL_FreeSurface(surface);
  // alright
}

void Game::drawHealthBar() {
  const int x = 25;
  const int y = 25;
  const int width = 220;
  const int height = 18;

  // bg

  SDL_Rect background{x, y, width, height};

  SDL_SetRenderDrawColor(renderer, 12, 12, 15, 220);
  SDL_RenderFillRect(renderer, &background);

  // health fill

  int fillWidth = static_cast<int>((playerHealth / 100.0f) * (width - 4));

  SDL_Rect fill{x + 2, y + 2, fillWidth, height - 4};

  if (playerHealth > 60.0f) {
    SDL_SetRenderDrawColor(renderer, 115, 145, 85, 255);
  } else if (playerHealth > 30.0f) {
    SDL_SetRenderDrawColor(renderer, 180, 125, 55, 255);

  } else {

    SDL_SetRenderDrawColor(renderer, 175, 45, 45, 255);
  }

  if (fillWidth > 0) {
    SDL_RenderFillRect(renderer, &fill);
  }

  // border

  SDL_SetRenderDrawColor(renderer, 190, 185, 165, 230);
  SDL_RenderDrawRect(renderer, &background);
}

void Game::drawGameOver() {
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

  SDL_SetRenderDrawColor(renderer, 3, 3, 6, 225);

  SDL_Rect overlay{0, 0, screenWidth, screenHeight};

  SDL_RenderFillRect(renderer, &overlay);

  SDL_Rect panel{screenWidth / 2 - 250, screenHeight / 2 - 165, 500, 330};

  SDL_SetRenderDrawColor(renderer, 14, 13, 18, 255);
  SDL_RenderFillRect(renderer, &panel);

  SDL_SetRenderDrawColor(renderer, 125, 45, 50, 255);
  SDL_RenderDrawRect(renderer, &panel);

  auto drawText = [&](const char *message, int y, SDL_Color color) {
    SDL_Surface *surface = TTF_RenderUTF8_Blended(font, message, color);

    if (!surface) {
      return;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    if (texture) {
      SDL_Rect destination{screenWidth / 2 - surface->w / 2, y, surface->w,
                           surface->h};

      SDL_RenderCopy(renderer, texture, nullptr, &destination);

      SDL_DestroyTexture(texture);
    }

    SDL_FreeSurface(surface);
  };

  drawText("GAME OVER", panel.y + 35, {205, 65, 65, 255});

  drawText("The town has claimed another visitor.", panel.y + 85,
           {195, 190, 180, 255});

  const std::string survived =
      "SURVIVAL TIME: " + formatSurvivalTime(finalSurvivalTime);

  const std::string record =
      "PERSONAL BEST: " + formatSurvivalTime(bestSurvivalTime);

  drawText(survived.c_str(), panel.y + 130, {110, 205, 220, 255});

  drawText(record.c_str(), panel.y + 165, {220, 190, 110, 255});

  drawText("[R] RESTART     [ESC]  QUIT", panel.y + 225, {195, 195, 185, 255});
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

void Game::drawWaveInfo() {
  if (batSwarm.getWave() <= 0) {
    return;
  }

  std::string info = "WAVE " + std::to_string(batSwarm.getWave()) +
                     "  |  BATS " + std::to_string(batSwarm.getRemaining());

  SDL_Surface *surface =
      TTF_RenderUTF8_Blended(font, info.c_str(), {210, 210, 195, 255});

  if (!surface) {
    return;
  }

  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

  if (texture) {
    SDL_Rect destination{25, 55, surface->w, surface->h};

    SDL_RenderCopy(renderer, texture, nullptr, &destination);

    SDL_DestroyTexture(texture);
  }

  SDL_FreeSurface(surface);
}

void Game::drawSurvivalInfo() {
  if (!survivalStarted) {
    return;
  }

  std::string info = "SURVIVED  " + formatSurvivalTime(survivalTime) +
                     "   |  BEST  " + formatSurvivalTime(bestSurvivalTime);

  SDL_Surface *surface =
      TTF_RenderUTF8_Blended(font, info.c_str(), {210, 210, 95, 255});

  if (!surface) {
    return;
  }

  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

  if (texture) {
    SDL_Rect destination{25, 115, surface->w, surface->h};

    SDL_RenderCopy(renderer, texture, nullptr, &destination);

    SDL_DestroyTexture(texture);
  }

  SDL_FreeSurface(surface);
}

void Game::startNewGame() {

  // Reset player
  player.reset();

  // Reset health and survival
  playerHealth = 100.0f;
  survivalTime = 0.0f;
  finalSurvivalTime = 0.0f;
  survivalStarted = false;
  newPersonalRecord = false;

  // Reset enemies and equipment
  batSwarm.reset();
  blaster.reset();

  // Reset story progress
  for (int i = 0; i < 3; ++i) {
    fileRead[i] = false;
  }

  // Reset computer interface
  xpDesktopActive = false;
  myComputerOpen = false;
  driveOpen = false;
  startMenuOpen = false;

  selectedFolder = -1;
  openedFile = -1;

  // Reset interactions
  telephoneMessageOpen = false;
  nearComputer = false;
  computerInteracted = false;

  // Reset game state
  gameOver = false;
  gameState = GameState::Playing;

  // Position camera immediately
  camera.snapTo(player.getCenterX(), player.getCenterY());

  // Update telephone proximity and stop any old ringing state
  telephone.update(player.getCenterX(), player.getCenterY());
}
void Game::drawMainMenu() {
  SDL_SetRenderDrawColor(renderer, 5, 7, 9, 255);
  SDL_RenderClear(renderer);

  // Background atmosphere
  SDL_SetRenderDrawColor(renderer, 17, 23, 24, 255);

  for (int y = 0; y < screenHeight; y += 5) {
    SDL_RenderDrawLine(renderer, 0, y, screenWidth, y);
  }

  // Central panel
  SDL_Rect panel{screenWidth / 2 - 250, screenHeight / 2 - 235, 500, 470};

  SDL_SetRenderDrawColor(renderer, 10, 13, 15, 255);
  SDL_RenderFillRect(renderer, &panel);

  SDL_SetRenderDrawColor(renderer, 75, 87, 82, 255);
  SDL_RenderDrawRect(renderer, &panel);

  // Text rendering helper
  auto drawText = [&](const char *message, int y, SDL_Color color) {
    SDL_Surface *surface = TTF_RenderUTF8_Blended(font, message, color);

    if (!surface) {
      return;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    if (texture) {
      SDL_Rect destination{screenWidth / 2 - surface->w / 2, y, surface->w,
                           surface->h};

      SDL_RenderCopy(renderer, texture, nullptr, &destination);
      SDL_DestroyTexture(texture);
    }

    SDL_FreeSurface(surface);
  };

  // Title
  drawText("THE LAST USER", panel.y + 65, {215, 220, 205, 255});

  drawText("A WORLD LEFT BEHIND", panel.y + 100, {105, 125, 115, 255});

  // Divider
  SDL_SetRenderDrawColor(renderer, 75, 90, 82, 255);

  SDL_RenderDrawLine(renderer, panel.x + 75, panel.y + 145,
                     panel.x + panel.w - 75, panel.y + 145);

  // Menu options
  const char *options[] = {"NEW GAME", "HOW TO PLAY", "QUIT"};

  for (int i = 0; i < 3; ++i) {
    const int y = panel.y + 190 + i * 65;

    SDL_Rect option{screenWidth / 2 - 155, y - 10, 310, 45};

    if (selectedMenuOption == i) {
      SDL_SetRenderDrawColor(renderer, 38, 49, 46, 255);
      SDL_RenderFillRect(renderer, &option);

      SDL_SetRenderDrawColor(renderer, 115, 140, 125, 255);
      SDL_RenderDrawRect(renderer, &option);
    }

    SDL_Color color = selectedMenuOption == i ? SDL_Color{230, 230, 210, 255}
                                              : SDL_Color{125, 135, 125, 255};

    drawText(options[i], y, color);

    if (selectedMenuOption == i) {
      SDL_SetRenderDrawColor(renderer, 190, 75, 65, 255);

      SDL_Rect marker{option.x + 18, option.y + 17, 7, 7};

      SDL_RenderFillRect(renderer, &marker);
    }
  }

  drawText("[ W / S ] SELECT     [ ENTER ] CONFIRM", panel.y + 415,
           {100, 115, 105, 255});
}
void Game::drawHowToPlay() {
  SDL_SetRenderDrawColor(renderer, 5, 7, 9, 255);
  SDL_RenderClear(renderer);

  SDL_Rect panel{screenWidth / 2 - 310, screenHeight / 2 - 235, 620, 470};

  SDL_SetRenderDrawColor(renderer, 10, 13, 15, 255);
  SDL_RenderFillRect(renderer, &panel);

  SDL_SetRenderDrawColor(renderer, 75, 87, 82, 255);
  SDL_RenderDrawRect(renderer, &panel);

  auto drawText = [&](const char *message, int y, SDL_Color color) {
    SDL_Surface *surface = TTF_RenderUTF8_Blended(font, message, color);

    if (!surface) {
      return;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    if (texture) {
      SDL_Rect destination{screenWidth / 2 - surface->w / 2, y, surface->w,
                           surface->h};

      SDL_RenderCopy(renderer, texture, nullptr, &destination);
      SDL_DestroyTexture(texture);
    }

    SDL_FreeSurface(surface);
  };

  drawText("HOW TO PLAY", panel.y + 35, {215, 220, 205, 255});

  SDL_SetRenderDrawColor(renderer, 75, 90, 82, 255);

  SDL_RenderDrawLine(renderer, panel.x + 65, panel.y + 75,
                     panel.x + panel.w - 65, panel.y + 75);

  // Controls
  drawText("CONTROLS", panel.y + 105, {190, 115, 95, 255});

  drawText("W A S D       Move", panel.y + 145, {190, 195, 185, 255});

  drawText("E                 Interact", panel.y + 180, {190, 195, 185, 255});

  drawText("SPACE         Fire energy blaster", panel.y + 215,
           {190, 195, 185, 255});

  // Gameplay
  drawText("SURVIVAL", panel.y + 270, {190, 115, 95, 255});

  drawText("Explore the town and investigate the computer.", panel.y + 305,
           {175, 185, 175, 255});

  drawText("Discover clues to uncover what happened.", panel.y + 335,
           {175, 185, 175, 255});

  drawText("Survive the creatures that hunt you.", panel.y + 365,
           {175, 185, 175, 255});

  drawText("[ ESC ] BACK TO MENU", panel.y + 420, {190, 100, 90, 255});
}
