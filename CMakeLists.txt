cmake_minimum_required(VERSION 3.20)

project(TheLastUser
    VERSION 0.1.0
    DESCRIPTION "A 2D atmospheric horror mystery"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED IMPORTED_TARGET sdl2)

add_executable(the-last-user
  src/main.cpp
  src/core/Player.cpp
  src/core/Game.cpp
  src/core/Camera.cpp
)

target_include_directories(the-last-user
  PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(the-last-user
  PRIVATE
    PkgConfig::SDL2
)

target_compile_options(the-last-user PRIVATE
  -Wall
  -Wextra
  -Wpedantic
  -Wconversion
  -Wshadow
)

target_compile_definitions(the-last-user PRIVATE
  GAME_VERSION="${PROJECT_VERSION}"
)

set_target_properties(the-last-user PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

