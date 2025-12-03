#include "renderer.hpp"

#include <condition_variable>

#include "SDL.h"
#include "io/image.hpp"
#include "math/color.hpp"

void Renderer::updateImage8() {
  const int w = scene.getWidth();
  const int h = scene.getHeight();

  for (int y = 0; y < h; ++y) {
    if (backPixels.rowReady[y]) {
      for (int x = 0; x < w; ++x) {
        const int i = y * w + x;
        const Color& col = backPixels.pxColors[i] /
                           static_cast<double>(backPixels.pxSamples[i]);
        const int rIndex = i * 3;
        const auto bytes = col.clamp().getBytes();
        image8[rIndex] = bytes[0];
        image8[rIndex + 1] = bytes[1];
        image8[rIndex + 2] = bytes[2];
      }
      backPixels.rowReady[y] = false;
    }
  }
}

void Renderer::run() {
  const int w = scene.getWidth();
  const int h = scene.getHeight();

  SDL_Init(SDL_INIT_VIDEO);
  window = SDL_CreateWindow("Progressive Ray Tracer", SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED, w, h, 0);
  sdlRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  texture = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_RGB24,
                              SDL_TEXTUREACCESS_STREAMING, w, h);

  bool running = true;
  bool rotating = false;
  SDL_Event event;

  while (running) {
    bool cameraUpdate = false;
    uint32_t frameStart = SDL_GetTicks();

    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) running = false;

      if (event.type == SDL_MOUSEBUTTONDOWN &&
          event.button.button == SDL_BUTTON_LEFT) {
        rotating = true;
        // Reset relative mouse state to prevent jumps
        SDL_GetRelativeMouseState(nullptr, nullptr);
      } else if (event.type == SDL_MOUSEBUTTONUP &&
                 event.button.button == SDL_BUTTON_LEFT) {
        rotating = false;
      } else if (event.type == SDL_MOUSEWHEEL) {
        scene.zoomCamera(event.wheel.y);
        cameraUpdate = true;
      } else if (event.type == SDL_KEYDOWN && !event.key.repeat) {
        SDL_Keymod mod = SDL_GetModState();  // current modifiers

        // Ctrl + S or Cmd + S pressed, save image
        if (((mod & KMOD_CTRL) || (mod & KMOD_GUI)) &&
            event.key.keysym.scancode == SDL_SCANCODE_S) {
          Image image{*this};
          image.save();
        }
      }
    }

    // Copy back pixels to front if ready
    updateImage8();

    // Move camera with arrow keys/wasdqe
    Vector dir;

    // Scale move_speed by FPS for consistent speed
    double moveSpeed = MOVE_SPEED / static_cast<double>(FPS);

    // Update camera version on movement
    if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_W] ||
        SDL_GetKeyboardState(NULL)[SDL_SCANCODE_A] ||
        SDL_GetKeyboardState(NULL)[SDL_SCANCODE_S] ||
        SDL_GetKeyboardState(NULL)[SDL_SCANCODE_D] ||
        SDL_GetKeyboardState(NULL)[SDL_SCANCODE_Q] ||
        SDL_GetKeyboardState(NULL)[SDL_SCANCODE_E] ||
        SDL_GetKeyboardState(NULL)[SDL_SCANCODE_UP] ||
        SDL_GetKeyboardState(NULL)[SDL_SCANCODE_DOWN] ||
        SDL_GetKeyboardState(NULL)[SDL_SCANCODE_LEFT] ||
        SDL_GetKeyboardState(NULL)[SDL_SCANCODE_RIGHT]) {
      cameraUpdate = true;
    }
    if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_LEFT]) {
      dir += Vector(-1.0, 0.0, 0.0);
    } else if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_RIGHT]) {
      dir += Vector(1.0, 0.0, 0.0);
    }
    if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_UP]) {
      dir += Vector(0.0, 1.0, 0.0);
    } else if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_DOWN]) {
      dir += Vector(0.0, -1.0, 0.0);
    }
    if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_Q]) {
      dir += Vector(0.0, 0.0, 1.0);
    } else if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_E]) {
      dir += Vector(0.0, 0.0, -1.0);
    }

    // Check if mouse is down for rotation
    Uint32 buttons = SDL_GetMouseState(nullptr, nullptr);
    if (rotating && buttons & SDL_BUTTON(SDL_BUTTON_LEFT)) {
      int dx, dy;
      SDL_GetRelativeMouseState(&dx, &dy);
      if (dx != 0 || dy != 0) {
        scene.eulerRotateCamera(dx, dy);
        cameraUpdate = true;
      }
    }

    // Update camera position
    if (cameraUpdate) {
      Scene& sc = scene;
      sc.moveCameraPosition(dir.norm().scale(moveSpeed));

      // Clear all tasks in the tracer pool
      tracer.pool.clearTasks();

      // Reset backPixel buffer and tileReady flags
      std::fill(backPixels.pxColors.begin(), backPixels.pxColors.end(),
                Color());
      std::fill(backPixels.pxSamples.begin(), backPixels.pxSamples.end(), 0);
      std::fill(backPixels.rowReady.begin(), backPixels.rowReady.end(), false);
    }

    // Make sure tracer isn't overloaded with tasks
    if (tracer.pool.numTasks() <= scene.getHeight()) {
      tracer.refinePixels(backPixels);
    }

    SDL_UpdateTexture(texture, nullptr, image8.data(), w * 3);
    SDL_RenderClear(sdlRenderer);
    SDL_RenderCopy(sdlRenderer, texture, nullptr, nullptr);
    SDL_RenderPresent(sdlRenderer);

    const int frameTime = SDL_GetTicks() - frameStart;
    const int fps = (frameTime > 0) ? (1000 / frameTime) : 0;

    if (fps > FPS) {
      SDL_Delay(1000 / FPS - frameTime);
    }
    // frameTime = SDL_GetTicks() - frameStart;
    // fps = (frameTime > 0) ? (1000 / frameTime) : 0;
    // std::cout << "FPS: " << fps << std::endl;
  }

  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(sdlRenderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}
