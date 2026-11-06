#include "../../include/world/World.hpp"

void World::draw(SDL_Renderer *renderer, const Camera &camera) {
  constexpr int TILE_SIZE = 96;

  for (int y = 0; y < 1600; y += TILE_SIZE) {
    for (int x = 0; x < 2400; x += TILE_SIZE) {
      SDL_Rect tile = camera.apply({x, y, TILE_SIZE, TILE_SIZE});

      // now camera.apply will return me a good lil sdl_Rect that obeys the
      // fundamental rules of sorcery xD (camera following)

      if ((x / TILE_SIZE + y / TILE_SIZE) % 2 == 0) {
        // so we use random tiles for uhh putting skeletons/grass/anything we'd
        // like to put later, this is lesson 01 of game development :P

        // for now, i'll just put a random color

        SDL_SetRenderDrawColor(renderer, 35, 46, 39, 255);

      } else {
        SDL_SetRenderDrawColor(renderer, 39, 49, 42, 255);
      }

      SDL_RenderFillRect(renderer, &tile);
      // gonna run the loop and render each tile
    }
  }

  // now we need a border as well (boundary)

  SDL_Rect roomBorder = camera.apply(
      {0, 0, 2400,
       1600}); // this is basically the width and height we're using for the
               // world, this will later help with collisiosn stuff i hope so
  //

  SDL_SetRenderDrawColor(renderer, 13, 19, 16, 255);
  SDL_RenderDrawRect(renderer, &roomBorder);

  // after border, let us draw our main computer and the skeletons using a loop

  drawComputer(renderer, camera);

  for (const SDL_Point &pos : skeletonPos) {
    drawSkeletons(renderer, camera, pos);
  }
}

// finally comes the best part whree i cna showcase my drawing skills hehe

void World::drawComputer(SDL_Renderer *renderer, const Camera &camera) {
  // uhh so the way i created in excalidraw is first we need a lil desk to show
  // a laptop so im gonna uhh make a ton of rects

  // also gonna make sure each and every rect is returned after camera.apply()
  // so i don't go facing cam isues later
  SDL_Rect desk = camera.apply(computerBounds);

  SDL_SetRenderDrawColor(renderer, 75, 48, 32, 255);
  SDL_RenderFillRect(renderer, &desk);

  // now before screen, we need a monitor

  SDL_Rect monitor = camera.apply({1155, 730, 90, 65});

  SDL_SetRenderDrawColor(renderer, 48, 45, 39, 255);
  SDL_RenderFillRect(renderer, &monitor);

  // uh something feels wrong

  SDL_Rect screen = camera.apply({1163, 737, 75, 49});
  SDL_SetRenderDrawColor(renderer, 30, 65, 100, 255);
  SDL_RenderFillRect(renderer, &screen);

  // keyboard should be there for sure, i want to do a mechanical keyboard but i
  // don't wanna mess with grids again rn

  SDL_Rect keyboard = camera.apply({1155, 815, 85, 16});
  SDL_SetRenderDrawColor(renderer, 165, 158, 139, 255);
  SDL_RenderFillRect(renderer, &keyboard);

  // mouse ig, im using a mouse from coco sports btw

  SDL_Rect mouse = camera.apply({1250, 816, 10, 16});
  // just a lil to the side and a lil below

  SDL_SetRenderDrawColor(renderer, 165, 158, 139, 255);
  SDL_RenderFillRect(renderer, &mouse);
  /// YOOO SHALL WE ADD A WEBCAM TOO? THAT'D LOOK COOL, i do own a webcam but i
  /// don't use it often coz i don't talkt o anyone :P
}

// alr skeleton time

void World::drawSkeletons(SDL_Renderer *renderer, const Camera &camera,
                          SDL_Point position) {
  auto drawBone = [&](SDL_Rect rect) {
    SDL_Rect screenRect = camera.apply(rect);

    SDL_SetRenderDrawColor(renderer, 190, 181, 151, 255); // whitish ig
    SDL_RenderFillRect(renderer, &screenRect);
  };

  const int x = position.x;
  const int y = position.y;

  // so the skeletons are petty complex im just gonna have a skull and a spine
  // hehe

  drawBone({x + 25, y, 30, 30});
  // skull ^

  drawBone({x + 37, y + 30, 7, 55});
  // spine ^
}

SDL_Rect World::getComputerBounds() const { return computerBounds; }
