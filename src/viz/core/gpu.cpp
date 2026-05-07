#include "gpu.hpp"
#include <SDL3/SDL_gpu.h>
#include <optional>
#include <vector>

namespace ntg::viz {
auto GPUWrapper::createGPUDevice(SDL_GPUShaderFormat shader_format,
                                 char *driver) -> void {
  this->device = SDL_CreateGPUDevice(shader_format, true, driver);
}
} // namespace ntg::viz
