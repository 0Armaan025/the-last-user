#pragma once

#include <SDL2/SDL.h>
#include <array>

#include "../core/Camera.hpp"

class World {
public:
  // soooo this function will basically draw our world including coputer and
  // skeleton corpses stuff, IM TOO TIRED TO PROPERLY DEFINE, OKAY???

  // EDIT: STUPID ARMAAN, YOU HAVE NO IDEA HOW TOC ONTINUE
  void draw(SDL_Renderer *renderer, const Camera &camera);
  // this is for the world ig

  // i really wish i'd make a components library for sdl2 later
  //

  // now let's finally draw the skeletons and the comptuer
  //

  // my idea is simpple, im gonna use camera.apply() for the monitor as well as
  // the corpses

  void drawComputer(SDL_Renderer *renderer, const Camera &camera);

  SDL_Rect getComputerBounds() const;

  void drawSkeletons(SDL_Renderer *renderer, const Camera &camera,
                     SDL_Point pos);
  // we just gonna pass the array and draw em

private:
  SDL_Rect computerBounds{1120, 720, 160, 130};

  // random coords
  std::array<SDL_Point, 5> skeletonPos{
      {{900, 700}, {1450, 700}, {850, 1000}, {1500, 1050}, {1200, 550}}};
};
