# the-last-user

A 2D atmospheric horror mystery game built with C++ and SDL2.

## Description

You are alone in a silent town. The network has gone dead. No response from outside stations. You might be the last one left. Explore the town, investigate an old Windows XP-style computer, answer a ringing telephone, and survive the creatures that hunt you.

Read story files on the computer to uncover what happened. Your best survival time is saved locally.

## Tech stack

- C++20
- CMake 3.20+
- SDL2 (window, rendering, input)
- SDL2_ttf (text rendering)
- SDL2_mixer (audio)
- pkg-config

## How to play

- Main menu: NEW GAME / HOW TO PLAY / QUIT (W/S to select, ENTER to confirm)
- WASD — move
- E — interact (answer telephone, open computer)
- SPACE — fire energy blaster (collect it first)
- Mouse — click through the desktop (My Computer -> C: -> Documents -> story files)
- ESC — close menus / quit
- R — restart on game over

## Build

Desktop:
```
cmake -B build && cmake --build build
```

Web (WASM):
```
em++ src/main.cpp src/core/*.cpp src/world/*.cpp -I include \
  --use-port=sdl2 --use-port=sdl2_ttf --use-port=sdl2_mixer \
  --preload-file assets/audio/telephone_ring.wav \
  --preload-file assets/fonts/DejaVuSerif.ttf \
  -s WASM=1 -s ALLOW_MEMORY_GROWTH=1 -s USE_SDL=2 -s USE_SDL_TTF=2 -s USE_SDL_MIXER=2 \
  -o docs/the-last-user.js
```

The WASM build outputs to `docs/` and is ready to deploy on GitHub Pages. Audio and font files are embedded in the `.data` file.

## Deploy on GitHub Pages

The site is served from the `docs/` folder on the `main` branch (Project Settings → Pages → Source: Deploy from branch → `main` / `docs`).

1. Build the WASM version (see Build section).
2. Commit the `docs/` output and push to `main`.
3. Pages publishes it at `https://0Armaan025.github.io/the-last-user/`.

## Support

If you like this project, please consider giving it a star on GitHub. It helps more people find it.
