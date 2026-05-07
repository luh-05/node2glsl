#pragma ONCE
#include "sdl.hpp"
#include <optional>
#include <vector>

namespace ntg::viz {
class GPUWrapper {
private:
  SDL_GPUDevice *device;

public:
  GPUWrapper() {}

  auto createGPUDevice(SDL_GPUShaderFormat shader_format, char *driver) -> void;
};
} // namespace ntg::viz
