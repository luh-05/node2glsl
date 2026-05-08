#include "gpu.hpp"
#include "absl/status/status.h"
#include "absl/strings/str_cat.h"
#include "sdl.hpp"
#include "shader/shader.hpp"
#include <SDL3/SDL.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_video.h>
#include <cassert>
#include <memory>
#include <string_view>

namespace ntg::viz {

struct GPUWrapper::Impl {
  struct GPUDeviceDeleter {
    void operator()(SDL_GPUDevice *device) const {
      SDL_DestroyGPUDevice(device);
    }
  };
  std::unique_ptr<SDL_GPUDevice, GPUDeviceDeleter> device;
};
GPUWrapper::GPUWrapper() : pimpl_(std::make_unique<Impl>()) {}
GPUWrapper::~GPUWrapper() {}

static auto ToSDLFormat(ShaderFormat f) -> SDL_GPUShaderFormat {
  switch (f) {
  case ShaderFormat::SpirV:
    return SDL_GPU_SHADERFORMAT_SPIRV;
  case ShaderFormat::Dxil:
    return SDL_GPU_SHADERFORMAT_DXIL;
  case ShaderFormat::Metal:
    return SDL_GPU_SHADERFORMAT_MSL | SDL_GPU_SHADERFORMAT_METALLIB;
  default:
    return SDL_GPU_SHADERFORMAT_INVALID;
  }
}

auto GPUWrapper::CreateGPUDevice(ShaderFormat format, char *driver)
    -> absl::Status {
  auto sdl_format = ToSDLFormat(format);
  std::unique_ptr<SDL_GPUDevice, Impl::GPUDeviceDeleter> device(
      SDL_CreateGPUDevice(sdl_format, true, driver));
  if (device == nullptr) {
    return absl::InternalError(
        absl::StrCat("Failed to create SDL GPU Device: ", SDL_GetError()));
  }

  this->pimpl_->device = std::move(device);
  return absl::OkStatus();
}
auto GPUWrapper::GetDriverName() -> absl::StatusOr<std::string_view> {
  if (!pimpl_->device) {
    return absl::UnavailableError("Device not initialized");
  }

  return std::string_view(SDL_GetGPUDeviceDriver(pimpl_->device.get()));
}
auto GPUWrapper::ClaimWindow(Window window) -> absl::Status {
  if (!SDL_ClaimWindowForGPUDevice(pimpl_->device.get(),
                                   reinterpret_cast<SDL_Window *>(window))) {
    return absl::InternalError(absl::StrCat(
        "Failed to claim window for SDL GPU Device: ", SDL_GetError()));
  }
  return absl::OkStatus();
}
static auto ToSDLSwapchainComposition(SwapchainComposition comp)
    -> SDL_GPUSwapchainComposition {
  switch (comp) {
  case SwapchainComposition::Sdr:
    return SDL_GPU_SWAPCHAINCOMPOSITION_SDR;
  case SwapchainComposition::SdrLinear:
    return SDL_GPU_SWAPCHAINCOMPOSITION_SDR_LINEAR;
  case SwapchainComposition::HdrExtendedLinear:
    return SDL_GPU_SWAPCHAINCOMPOSITION_HDR_EXTENDED_LINEAR;
  case SwapchainComposition::Hdr10St2084:
    return SDL_GPU_SWAPCHAINCOMPOSITION_HDR10_ST2084;
  }
  // FIXME: Currently defaulting to SDR quietly
  return SDL_GPU_SWAPCHAINCOMPOSITION_SDR;
}
static auto ToSDLPresentMode(PresentMode mode) {
  switch (mode) {
  case PresentMode::VSync:
    return SDL_GPU_PRESENTMODE_VSYNC;
  case PresentMode::Immediate:
    return SDL_GPU_PRESENTMODE_IMMEDIATE;
  case PresentMode::Mailbox:
    return SDL_GPU_PRESENTMODE_MAILBOX;
  }
  // FIXME: Currently defaulting to VSync quietly
  return SDL_GPU_PRESENTMODE_VSYNC;
}
auto GPUWrapper::SetSwapchainParameters(Window window,
                                        SwapchainComposition composition,
                                        PresentMode present_mode)
    -> absl::Status {
  auto sdl_composition = ToSDLSwapchainComposition(composition);
  auto sdl_present_mode = ToSDLPresentMode(present_mode);
  if (!SDL_SetGPUSwapchainParameters(pimpl_->device.get(),
                                     reinterpret_cast<SDL_Window *>(window),
                                     sdl_composition, sdl_present_mode)) {
    return absl::InternalError(absl::StrCat(
        "Failed to set SDL GPU swapchain parameters: ", SDL_GetError()));
  }

  return absl::OkStatus();
}
auto GPUWrapper::ReleaseShader(std::shared_ptr<Shader> shader) -> void {
  SDL_ReleaseGPUShader(pimpl_->device.get(), shader->getShader());
}

// TODO: Remove later
auto GPUWrapper::RawGetDevice() -> void * {
  return reinterpret_cast<void *>(pimpl_->device.get());
}
} // namespace ntg::viz
