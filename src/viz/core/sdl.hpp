#pragma ONCE

#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_video.h>

#include <spdlog/spdlog.h>

void LogSDLError() { spdlog::error("SDL Error: {}", SDL_GetError()); }

#define TRY_SDL(func)                                                          \
  if (!func) {                                                                 \
    LogSDLError();                                                             \
    return SDL_APP_FAILURE;                                                    \
  }
