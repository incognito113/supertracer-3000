#pragma once

#include <SDL_render.h>
#include <SDL_video.h>
#include <stdint.h>

#include <vector>

#include "renderer/tracer.hpp"
#include "scene/scene.hpp"

class Renderer {
 private:
  Scene scene;
  Pixels backPixels;
  std::vector<uint8_t> image8;  // Raw image data for SDL
  Tracer tracer;
  const int FPS;
  SDL_Window* window = nullptr;
  SDL_Renderer* sdlRenderer = nullptr;
  SDL_Texture* texture = nullptr;
  static constexpr double MOVE_SPEED = 0.3;
  static constexpr int MAX_QUALITY = 129;

  void updateImage8();

 public:
  Renderer(Scene sc, int fps = 60)
      : scene(sc),
        backPixels(sc.getWidth(), sc.getHeight()),
        image8(sc.getWidth() * sc.getHeight() * 3, 0),
        tracer(scene),
        FPS(fps) {}

  void run();

  ~Renderer() = default;

  friend class Image;
};
